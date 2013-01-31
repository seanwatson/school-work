/*******************************************************************************
  Course(s):
     CS037, University of Western Ontario

  Author(s):
     Andrew Delong <firstname.lastname@gmail.com>

  History:
     Nov 13, 2007; - Added assertions on image routines
                   - Removed spurious 'Sleep' in SetDrawColour

     Nov  2, 2007; - Discard WM_MOUSE_MOVE events if there are
                     newer events waiting
                   - Cache most recent handle->bitmap used

     Nov  1, 2007; - Added DrawPolyline, DrawPolygonFilled
                   - Added GetImageBytesPerPixel 

     Oct 29, 2007; - Added Image functions (create, draw, etc)

     Sep 26, 2007; - Added Pause()
                   - Compiles for both _UNICODE and _MBCS now
                   - Reorg parent/child window mechanism
                   - Added Button (no public interface yet)

     Sep 18, 2007; - GetKeyboardInput now reads from console too.
                   - Drawing now works before ShowWindow.
                   - Fixed flickering problem.

     Sep 10, 2007; - Initial window, drawing, event polling.

*******************************************************************************/

#include "cs037util.h"
#include <conio.h>
#include <cassert>
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#define WINVER       0x501
#define NOMINMAX
#include <windows.h>
#include <tchar.h>
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)   ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)   ((int)(short)HIWORD(lp))
#endif
// Automatically link with comctl32 to make XP-style buttons etc the default
#pragma comment(lib, "comctl32") 
#ifdef _WIN64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
                         version='6.0.0.0' processorArchitecture='IA64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
                         version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma comment(lib, "msimg32") // link for AlphaBlend
#include <list>
#include <map>
#include <stdexcept>
#include <string>

#ifdef _WIN64 // Stupid SetWindowLongPtr doesn't have the signature that the docs claim. Pfft.
#define SETWINDOWLONGPTR(hWnd, index, newLong) SetWindowLongPtr(hWnd, index, (LONG_PTR)(newLong))
#else
#define SETWINDOWLONGPTR(hWnd, index, newLong) (size_t)SetWindowLong(hWnd, index, (LONG)(size_t)(newLong))
#endif


#ifdef _UNICODE
std::wstring charToWstr(const char* str) {
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
	std::wstring result;
	result.resize(len);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, &result[0], len);
	return result;
}
#else
std::string charToWstr(const char* str) { return str; }
#endif

#define AssertBreak __asm { int 0x3 }
#define AssertMsg(expr,msg) { if (expr) { } else { AssertBreak } }

struct BitmapInfo {
	HANDLE osHandle;
	DIBSECTION dib;
};

// For some alpha-blended excitement on XP/2000, uncomment this (oooooooh ghost!)
//#define WINDOW_ALPHA 70

//////////////////////////////////////////////////////////////////////
// Window
//
// Notes on window creation/deletion:
//  * To keep the implementation simple for CS037, Window creation/deletion is NOT thread safe.
//  * The Window object does not create ('realize') the OS window (HWND etc) until
//    its handle is needed for something.
//  * OS windows can be destroyed separately from the corresponding Window object,
//    but the Window objects will still hang around (and will have a zero HWND handle).
//    The 'children()' method should help to explicitly traverse and to delete Window
//    objects that were created on the heap.
//

class Window {
public:
	Window(const char* name);

	// 'Destroys' this window (and all child windows via 'delete').
	// Any pointers to this Window object will be invalid after this call.
	//
	void destroy();

	const char*   name() const;
	virtual void  setText(const TCHAR* text);
	virtual void  setPosition(int x, int y);
	virtual void  setPositionAndSize(int x, int y, int sizeX, int sizeY);
	virtual void  setSize(int sizeX, int sizeY);
	virtual void  setVisible(bool visible);

	int   screenX() const;
	int   screenY() const;
	int   relativeX() const; // relative to parent
	int   relativeY() const; // relative to parent
	int   windowSizeX() const;
	int   windowSizeY() const;
	int   clientSizeX() const;
	int   clientSizeY() const;

	void        setParent(Window* window);
	Window*     parent();
	Window*     child(const char* name);

	static Window* sWindowFromHandle(HWND hWnd);

	// Should use delegates but to keep the implementation simple 
	// this void* and __thiscall nonsense will be good enough, probably.
	typedef void (*OnClickHandler)(void* userData, Window* sender);

	// ATTN: not safe to request handle() from within subclass constructors 
	// (expects VMT of subclasses to be loaded!)
	HWND  handle() const; 

protected:
	enum { WM_COMMAND_REFLECT = WM_USER+0x0001 };


	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual void    realize();

	// The WndProc callback is invoked whenever MS Windows sends us some kind
	// of event ("message").
	//
	virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Each should return true if the default WNDPROC for this class should handle 
	// the event, and return false if the event has been handled internally by the method.
	//
	virtual bool    onCreate();
	virtual bool    onDestroy();
	virtual bool    onClose();
	virtual bool    onKeyDown(unsigned key);
	virtual bool    onKeyUp(unsigned key);
	virtual bool    onMouseMove(int x, int y);
	virtual bool    onMouseDown(int x, int y, int button);
	virtual bool    onMouseUp(int x, int y, int button);
	virtual bool    onPaint();
	virtual bool    onParentChanged();
	virtual bool    onSizeChanged();

private:
	// All subclasses of Window should have protected/private destructors, and
	// rely on the destroy() method above.
	//
	// The destructor is not public so that programmers will not be able to create
	// a Window object on the stack or as global data. This way, we know all child
	// windows were created on the heap and so we can clean up after ourselves.
	//
	virtual ~Window();

	void registerWndClass(WNDCLASS& wndClass);
	void rebuildChildrenList();
	static BOOL    CALLBACK         sCollectChildrenCallback(HWND hWnd, LPARAM lParam);

	typedef std::list<Window*> ChildList;
	HWND        mHWND;
	Window*     mParent;
	ChildList   mChildren;
	std::string mName;
	WNDPROC     mBaseWndProc;
	bool        mIsDestroyed;

private:
	// Window instance registration
	static const TCHAR*             cBaseWndClassName;
	static Window*                  sWindowBeingCreated;
	static Window*                  sWindowBeingCreatedParent;
	static LRESULT CALLBACK         sGlobalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

const TCHAR* Window::cBaseWndClassName = TEXT("cs037window");


Window::Window(const char* name)
: mHWND(0)
, mParent(0)
, mName(name)
, mBaseWndProc(0)
, mIsDestroyed(false)
{
	// don't call virtual methods in constructors -- it doesn't work the way you expect!
}

Window::~Window()
{
	while (!mChildren.empty())
		mChildren.front()->destroy(); // this will rebuild mChildren, so avoid iterators
	
	if (mHWND) {
		DestroyWindow(mHWND);
	}
	if (mParent) 
		mParent->rebuildChildrenList();
}

void Window::destroy() { delete this; }


void Window::getWndClassStyle(WNDCLASS& wndClass) const
{
	wndClass.style         = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc   = sGlobalWndProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = 0;
	wndClass.hInstance     = GetModuleHandle(0);
	wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = 0;
	wndClass.lpszMenuName  = 0;
	wndClass.lpszClassName = cBaseWndClassName;
}

void Window::getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const
{
	dwExStyle |= 0;
	dwStyle   |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
}

const char* Window::name() const
{
	return mName.c_str();
}

void Window::setText(const TCHAR* title)
{
	SetWindowText(handle(), title);
}

void Window::setPosition(int x, int y)
{
	SetWindowPos(handle(), 0, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
}

void Window::setPositionAndSize(int x, int y, int windowSizeX, int windowSizeY)
{
	SetWindowPos(handle(), 0, x, y, windowSizeX, windowSizeY, SWP_NOACTIVATE);
}

void Window::setSize(int windowSizeX, int windowSizeY)
{
	SetWindowPos(handle(), 0, 0, 0, windowSizeX, windowSizeY, SWP_NOACTIVATE | SWP_NOMOVE);
}

void Window::setVisible(bool visible)
{
	ShowWindow(handle(), visible ? SW_SHOW : SW_HIDE);
}

int Window::screenX() const
{
	RECT r;
	GetWindowRect(handle(), &r);
	return r.left;
}

int Window::screenY() const
{
	RECT r;
	GetWindowRect(handle(), &r);
	return r.top;
}

int Window::relativeX() const
{
	int result = 0;
	RECT r;
	GetWindowRect(handle(), &r);
	result = r.left;
	HWND parentHWND = GetParent(handle());
	if (parentHWND) {
		GetWindowRect(parentHWND, &r);
		result -= r.left;
	}
	return result;
}

int Window::relativeY() const
{
	int result = 0;
	RECT r;
	GetWindowRect(handle(), &r);
	result = r.top;
	HWND parentHWND = GetParent(handle());
	if (parentHWND) {
		GetWindowRect(parentHWND, &r);
		result -= r.top;
	}
	return result;
}

int Window::windowSizeX() const
{
	RECT r;
	GetWindowRect(handle(), &r);
	return r.right - r.left;
}

int Window::windowSizeY() const
{
	RECT r;
	GetWindowRect(handle(), &r);
	return r.bottom - r.top;
}

int Window::clientSizeX() const
{
	RECT r;
	GetClientRect(handle(), &r);
	return r.right;
}

int Window::clientSizeY() const
{
	RECT r;
	GetClientRect(handle(), &r);
	return r.bottom;
}

void Window::setParent(Window* parent)
{
	if (!mHWND) {
		mParent = parent; // parent will be used in realize() later on
		mParent->mChildren.push_back(this);
		onParentChanged();
		return;
	}
	// Otherwise, reparent
	LONG currStyle = GetWindowLong(handle(), GWL_STYLE);
	if (parent) {
		SetParent(handle(), parent->handle());
		SETWINDOWLONGPTR(handle(), GWL_STYLE, currStyle | WS_CHILD);
	} else {
		SetParent(handle(), 0);
		SETWINDOWLONGPTR(handle(), GWL_STYLE, currStyle & ~WS_CHILD);
	}
	if (mParent)
		mParent->rebuildChildrenList();
	mParent = parent;
	if (mParent)
		mParent->rebuildChildrenList();
	onParentChanged();
}

Window* Window::parent()
{
	return mParent;
}

Window* Window::child(const char *name)
{
	for (ChildList::const_iterator i = mChildren.begin(); i != mChildren.end(); ++i)
		if (strcmp((*i)->name(), name) == 0)
			return *i;
	return 0;
}

HWND  Window::handle() const
{
	if (!mHWND && !mIsDestroyed)
		const_cast<Window*>(this)->realize();
	return mHWND;
}

void Window::rebuildChildrenList()
{
	mChildren.clear();
	EnumChildWindows(mHWND, sCollectChildrenCallback, reinterpret_cast<LPARAM>(this));
}

void Window::realize()
{
	WNDCLASS wndClass;
	memset(&wndClass, 0, sizeof(WNDCLASS));
	getWndClassStyle(wndClass);
	registerWndClass(wndClass);

	DWORD dwStyle = 0, dwExStyle = 0;
	getWndStyle(dwStyle, dwExStyle);

	HWND parentHWND = 0;
	if (mParent) {
		parentHWND = mParent->handle();
		dwStyle |= WS_CHILD;
	}

	mHWND = CreateWindowEx(dwExStyle, wndClass.lpszClassName, TEXT(""), dwStyle,
							   0, 0, 0, 0,
							   parentHWND, 0, GetModuleHandle(0), 0);

	if (!mHWND)
		throw std::runtime_error("CreateWindowEx failed");

	// Redirect all messages to our WndProc dispatcher, and for this particular
	// instance remember which window class behaviour was requested so that
	// we can make it the default (i.e. call the original, mBaseWndProc, by default).
	//
	mBaseWndProc = (WNDPROC)SETWINDOWLONGPTR(mHWND, GWLP_WNDPROC, sGlobalWndProc);

	// Allow casting back and forth between window handles and Window objects
	//
	SETWINDOWLONGPTR(mHWND, GWLP_USERDATA, static_cast<Window*>(this));

	// Force the style we actually requested, since CreateWindow sometimes adds
	// unwanted styles depending on if there is an initial parent, etc.
	//
	SETWINDOWLONGPTR(handle(), GWL_STYLE, dwStyle);
}

void Window::registerWndClass(WNDCLASS& wndClass)
{
	WNDCLASS existingClass;
	if (!GetClassInfo(GetModuleHandle(0), wndClass.lpszClassName, &existingClass))
		if (!RegisterClass(&wndClass))
			throw std::runtime_error("RegisterClass failed");
}

LRESULT Window::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {
		case WM_CREATE: { if (onCreate()) break; return 0; }
		case WM_DESTROY: { if (onDestroy()) break; return 0; }
		case WM_CLOSE: { if (onClose()) break; return 0; }
		case WM_MOUSEMOVE: { if (onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) break; return 0; }
		case WM_LBUTTONDOWN: { if (onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0)) break; return 0; }
		case WM_RBUTTONDOWN: { if (onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 1)) break; return 0; }
		case WM_MBUTTONDOWN: { if (onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 2)) break; return 0; }
		case WM_LBUTTONUP: { if (onMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0)) break; return 0; }
		case WM_RBUTTONUP: { if (onMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 1)) break; return 0; }
		case WM_MBUTTONUP: { if (onMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 2)) break; return 0; }
		case WM_PAINT: { if (onPaint()) break; return 0; }
		case WM_SIZE: { if (onSizeChanged()) break; return 0; }
		case WM_KEYDOWN: { if (onKeyDown((UINT)wParam)) break; return 0; }
		case WM_KEYUP: { if (onKeyUp((UINT)wParam)) break; return 0; }
		case WM_COMMAND: {
			if (SendMessage(reinterpret_cast<HWND>(lParam), WM_COMMAND_REFLECT, wParam, lParam))
				break;
			return 0;
		}
	}
	if (mBaseWndProc != sGlobalWndProc) {
		LRESULT r = CallWindowProc(mBaseWndProc, mHWND, uMsg, wParam, lParam);
		return r;
	} else {
		LRESULT r = DefWindowProc(mHWND, uMsg, wParam, lParam);
		return r;
	}
}

bool Window::onCreate() { return true; }
bool Window::onDestroy() { 
	mHWND = 0;
	mIsDestroyed = true;
	return true; 
}
bool Window::onClose() { return true; }
bool Window::onKeyDown(unsigned key) { return true; }
bool Window::onKeyUp(unsigned key) { return true; }
bool Window::onMouseMove(int x, int y) { return true; }
bool Window::onMouseDown(int x, int y, int button) { return true; }
bool Window::onMouseUp(int x, int y, int button) { return true; }
bool Window::onPaint() { return true; }
bool Window::onParentChanged() { return true; }
bool Window::onSizeChanged() { return true; }

Window* Window::sWindowFromHandle(HWND hWnd)
{
	return reinterpret_cast<Window*>((LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

LRESULT CALLBACK Window::sGlobalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	Window* window = sWindowFromHandle(hWnd);
	if (window)
		return window->wndProc(uMsg, wParam, lParam);

	// Called with unregistered window handle. 
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


BOOL CALLBACK Window::sCollectChildrenCallback(HWND hWnd, LPARAM lParam)
{
	Window* parent = reinterpret_cast<Window*>(lParam);
	Window* child = reinterpret_cast<Window*>((LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (child)
		parent->mChildren.push_back(child);
	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// ButtonCtl

class ButtonCtl: virtual public Window {
	typedef Window Super;
public:
	ButtonCtl(const char* name);

	void setOnClick(OnClickHandler onClick, void* userData);

protected:
	virtual ~ButtonCtl();
	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	OnClickHandler mOnClick;
	void*          mOnClickData;
};

ButtonCtl::ButtonCtl(const char *name): Super(name) { }
ButtonCtl::~ButtonCtl() { }

void ButtonCtl::setOnClick(OnClickHandler onClick, void* userData)
{
	mOnClick = onClick;
	mOnClickData = userData;
}

void ButtonCtl::getWndClassStyle(WNDCLASS& wndClass) const
{
	Super::getWndClassStyle(wndClass);
	wndClass.lpszClassName = TEXT("BUTTON");
}

void ButtonCtl::getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const
{
	Super::getWndStyle(dwStyle, dwExStyle);
	dwStyle |= BS_TEXT | WS_VISIBLE;
	dwStyle |= BS_PUSHBUTTON;
}

LRESULT ButtonCtl::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {
		case WM_COMMAND_REFLECT: { 
			if (mOnClick)
				mOnClick(mOnClickData, this);
			return 0; 
		}
	}
	return Super::wndProc(uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
// SimpleCanvas

class SimpleCanvas: virtual public Window {
	typedef Window Super;
public:
	SimpleCanvas(const char* name);

	// The off screen canvas is, by default, the same size as the client area of the
	// window when it is first drawn. This method resizes the canvas area, preserving as 
	// much of the existing pixels as possible.
	void setBitmapSize(int sizeX, int sizeY);

	// Simple drawing methods. Relevant client area is automatically invalidated.
	void setColour(unsigned char r, unsigned char g, unsigned char b);
	void drawString(int x0, int y0, const TCHAR* str);
	void drawRect(int x0, int y0, int x1, int y1);
	void fillRect(int x0, int y0, int x1, int y1);
	void drawEllipse(int x0, int y0, int x1, int y1);
	void fillEllipse(int x0, int y0, int x1, int y1);
	void drawPolyline(int* pts, int count);
	void fillPoly(int* pts, int count);
	void drawBitmap(int x0, int y0, const BitmapInfo& bmp);
	void stretchBitmap(int x0, int y0, int x1, int y1, const BitmapInfo& bmp);

protected:
	virtual ~SimpleCanvas();

	HDC             DC();
	HDC             offScreenDC();
	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;

	virtual bool    onPaint();

private:
	HDC     mOwnDC;
	HDC     mOffScreenDC;
	HBITMAP mOffScreenBM;
	HBRUSH  mActiveBrush; // must be explicitly selected when needed
	HPEN    mActivePen;   // must be explicitly selected when needed
};


SimpleCanvas::SimpleCanvas(const char* name)
: Super(name)
, mOwnDC(0)
, mOffScreenDC(0)
, mOffScreenBM(0)
, mActiveBrush(0)
, mActivePen(0)
{
}

SimpleCanvas::~SimpleCanvas()
{
	// mOwnDC is destructed with the corresponding window handle
	if (mOffScreenDC) DeleteDC(mOffScreenDC);
	if (mOffScreenBM) DeleteObject(mOffScreenBM);
	if (mActiveBrush) DeleteObject(mActiveBrush);
	if (mActivePen) DeleteObject(mActivePen);
}

void SimpleCanvas::setBitmapSize(int sizeX, int sizeY)
{
	HBITMAP newBM = CreateCompatibleBitmap(DC(), sizeX, sizeY);
	SetBitmapDimensionEx(newBM, sizeX, sizeY, 0);
	if (mOffScreenBM) {
		// Copy as much of the old contents as possible into the new bitmap
		HDC tempDC = CreateCompatibleDC(DC());
		HGDIOBJ defaultBM = SelectObject(tempDC, newBM);

		// Perform the copy from old to new
		SIZE oldBMSize;
		GetBitmapDimensionEx(mOffScreenBM, &oldBMSize);
		BitBlt(tempDC, 0, 0, oldBMSize.cx, oldBMSize.cy, 
			   offScreenDC(), 0, 0, SRCCOPY);

		// Select clean up and select new bitmap
		SelectObject(tempDC, defaultBM);
		DeleteDC(tempDC);
		HGDIOBJ oldBM = SelectObject(offScreenDC(), newBM);
		assert(oldBM == mOffScreenBM);
		DeleteObject(mOffScreenBM);
	} else {
		SelectObject(offScreenDC(), newBM);
	}
	mOffScreenBM = newBM;
}

void SimpleCanvas::setColour(unsigned char r, unsigned char g, unsigned char b)
{
	DeleteObject(mActivePen);
	DeleteObject(mActiveBrush);
	mActiveBrush = CreateSolidBrush(RGB(r,g,b));
	mActivePen = CreatePen(PS_SOLID, 1, RGB(r,g,b));
	SetTextColor(offScreenDC(), RGB(r,g,b));
}

void SimpleCanvas::drawString(int x0, int y0, const TCHAR* str)
{
	int bkMode = SetBkMode(offScreenDC(), TRANSPARENT);
	RECT r = { x0, y0, x0, y0 };
	DrawTextEx(offScreenDC(), const_cast<TCHAR*>(str), -1, &r, DT_LEFT | DT_TOP | DT_CALCRECT, 0);
	DrawTextEx(offScreenDC(), const_cast<TCHAR*>(str), -1, &r, DT_LEFT | DT_TOP, 0);
	SetBkMode(offScreenDC(), bkMode);
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::fillRect(int x0, int y0, int x1, int y1)
{
	RECT r = { x0, y0, x1, y1 };
	FillRect(offScreenDC(), &r, mActiveBrush);
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::drawRect(int x0, int y0, int x1, int y1)
{
	RECT r = { x0, y0, x1, y1 };
	FrameRect(offScreenDC(), &r, mActiveBrush);
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::fillEllipse(int x0, int y0, int x1, int y1)
{
	HGDIOBJ oldBrush = SelectObject(offScreenDC(), mActiveBrush);
	HGDIOBJ oldPen = SelectObject(offScreenDC(), mActivePen);
	Ellipse(offScreenDC(), x0, y0, x1, y1);
	SelectObject(offScreenDC(), oldBrush);
	SelectObject(offScreenDC(), oldPen);
	RECT r = { x0, y0, x1, y1 };
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::drawEllipse(int x0, int y0, int x1, int y1)
{
	HGDIOBJ oldBrush = SelectObject(offScreenDC(), GetStockObject(NULL_BRUSH));
	HGDIOBJ oldPen = SelectObject(offScreenDC(), mActivePen);
	Ellipse(offScreenDC(), x0, y0, x1, y1);
	SelectObject(offScreenDC(), oldBrush);
	SelectObject(offScreenDC(), oldPen);
	RECT r = { x0, y0, x1, y1 };
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::fillPoly(int* pts, int count)
{
	HGDIOBJ oldBrush = SelectObject(offScreenDC(), mActiveBrush);
	HGDIOBJ oldPen = SelectObject(offScreenDC(), mActivePen);
	Polygon(offScreenDC(), reinterpret_cast<const POINT*>(pts), count);
	SelectObject(offScreenDC(), oldBrush);
	SelectObject(offScreenDC(), oldPen);
	InvalidateRect(handle(), NULL, FALSE);
}

void SimpleCanvas::drawPolyline(int* pts, int count)
{
	HGDIOBJ oldBrush = SelectObject(offScreenDC(), GetStockObject(NULL_BRUSH));
	HGDIOBJ oldPen = SelectObject(offScreenDC(), mActivePen);
	Polyline(offScreenDC(), reinterpret_cast<const POINT*>(pts), count);
	SelectObject(offScreenDC(), oldBrush);
	SelectObject(offScreenDC(), oldPen);
	InvalidateRect(handle(), NULL, FALSE);
}

void SimpleCanvas::drawBitmap(int x0, int y0, const BitmapInfo& bmp)
{
	stretchBitmap(x0, y0, x0 + bmp.dib.dsBmih.biWidth, y0 + bmp.dib.dsBmih.biHeight, bmp);
}

void SimpleCanvas::stretchBitmap(int x0, int y0, int x1, int y1, const BitmapInfo& bmp)
{
#if 1
	HDC dc = CreateCompatibleDC(offScreenDC());
	SelectObject(dc, bmp.osHandle);
	BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, (bmp.dib.dsBm.bmBitsPixel == 32) ? AC_SRC_ALPHA : 0};
	AlphaBlend(offScreenDC(), x0, y0, x1-x0, y1-y0, dc, 0, 0, 
		bmp.dib.dsBmih.biWidth, bmp.dib.dsBmih.biHeight, blend); 
	DeleteDC(dc);
#else
	StretchDIBits(offScreenDC(), x0, y0, x1-x0, y1-y0, 
		0, 0, bmp.dib.dsBmih.biWidth, bmp.dib.dsBmih.biHeight,
		bmp.dib.dsBm.bmBits, (const BITMAPINFO*)&bmp.dib.dsBmih, DIB_RGB_COLORS, SRCCOPY);
#endif
}


HDC SimpleCanvas::DC()
{
	if (!mOwnDC) {
		mOwnDC = GetDC(handle());
		if (!mOwnDC)
			throw std::runtime_error("GetDC failed");
	}
	return mOwnDC;
}

HDC SimpleCanvas::offScreenDC()
{
	if (!mOffScreenDC) {
		mOffScreenDC = CreateCompatibleDC(DC());
		if (!mOffScreenDC)
			throw std::runtime_error("CreateCompatibleDC failed");
	}
	return mOffScreenDC;
}

void SimpleCanvas::getWndClassStyle(WNDCLASS& wndClass) const
{
	Super::getWndClassStyle(wndClass);
	wndClass.style |= CS_OWNDC; // lets us keep our mOwnDC without releasing it
}

bool SimpleCanvas::onPaint()
{
	if (!mOffScreenBM)
		setBitmapSize(clientSizeX(), clientSizeY());
	
	// Copy the off-screen bitmap onto the screen
	SIZE sizeOfBM;
	GetBitmapDimensionEx(mOffScreenBM, &sizeOfBM);
	BitBlt(DC(), 0, 0, sizeOfBM.cx, sizeOfBM.cy,
	       offScreenDC(), 0, 0, SRCCOPY);

	// Fill any unused area with some default colour
	RECT fillX = { sizeOfBM.cx, 0, std::max((int)sizeOfBM.cx, clientSizeX()), clientSizeY() };
	RECT fillY = { 0, sizeOfBM.cy, clientSizeX(), std::max((int)sizeOfBM.cy, clientSizeY()) };
	FillRect(DC(), &fillX, 0);
	FillRect(DC(), &fillY, 0);
	return true;
}

//////////////////////////////////////////////////////////////////////
// SimpleCanvasAppWindow

class SimpleCanvasApp: public SimpleCanvas {
	typedef SimpleCanvas Super;
public:
	SimpleCanvasApp(const char* name);
protected:
	virtual ~SimpleCanvasApp();
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual void    realize();
	virtual bool    onClose();
};

SimpleCanvasApp::SimpleCanvasApp(const char* name)
: Super(name),
  Window(name)
{
}

SimpleCanvasApp::~SimpleCanvasApp()
{
}

void SimpleCanvasApp::realize()
{
	Super::realize();
#ifdef WINDOW_ALPHA
	SetLayeredWindowAttributes(handle(), 0, (255 * WINDOW_ALPHA) / 100, LWA_ALPHA);
#endif
}

void SimpleCanvasApp::getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const
{
	Super::getWndStyle(dwStyle, dwExStyle);
	dwExStyle |= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
#ifdef WINDOW_ALPHA
	dwExStyle |= WS_EX_LAYERED;
#endif
	dwStyle   |= WS_OVERLAPPEDWINDOW;
	dwStyle   &= ~(WS_CHILD | WS_CHILDWINDOW);
}

bool SimpleCanvasApp::onClose()
{
	PostQuitMessage(0);
	return true;
}

//////////////////////////////////////////////////////////////////////
// Simple 037 API redirects all calls to a default global Window object

std::map<int, BitmapInfo> gBitmapRegistry;
int gNextBitmapID = 1;

std::list<MSG> gKeyboardInputQueue;
std::list<MSG> gMouseInputQueue;
bool gQuit = false;

static SimpleCanvasApp* theMainWindow()
{
	// NOT thread safe, obviously
	static SimpleCanvasApp* sMainWindow = 0;
	if (!sMainWindow) {
		sMainWindow = new SimpleCanvasApp("main");
		sMainWindow->setPositionAndSize(300, 100, 640, 480);
		sMainWindow->setBitmapSize(1280, 1024);
	}
	return sMainWindow;
}

void ShowDialog_(const char* title, const char* format, va_list args)
{
	int textLen = _vscprintf(format, args);
	char* message = new char[textLen+1];
	vsprintf_s(message, textLen+1, format, args);
	MessageBoxA(theMainWindow()->handle(), message, title, MB_OK);
	delete [] message;
}

inline static BitmapInfo* sGetBitmapInfo(int handle)
{
	// Do a bit of caching so that subsequent calls on the same
	// image will avoid lookups in gBitmapRegistry.
	static BitmapInfo* bmp = 0;
	static int bmpHandle = 0;
	if (!bmp || handle != bmpHandle) {
		if (gBitmapRegistry.find(handle) != gBitmapRegistry.end()) {
			bmp = &gBitmapRegistry[handle];
			bmpHandle = handle;
		} else {
			bmp = 0;
			bmpHandle = 0;
		}
	}
	AssertMsg(bmp != NULL, "Did not find an image corresponding to 'handle'");
	return bmp;
}

static void sPumpMessageQueue()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			gQuit = true;

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (Window::sWindowFromHandle(msg.hwnd) == static_cast<Window*>(theMainWindow())) {
			if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
				gKeyboardInputQueue.push_back(msg);
			else if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
				gMouseInputQueue.push_back(msg);
		}
	}
}


bool GetKeyboardInput(char* key)
{
	sPumpMessageQueue();
	while (!gKeyboardInputQueue.empty()) {
		MSG msg = gKeyboardInputQueue.front(); gKeyboardInputQueue.pop_front();
		if (msg.message == WM_KEYDOWN) {
			char c = MapVirtualKey((int)msg.wParam, 2); // 2 = MAPVK_VK_TO_CHAR
			if (c) {
				*key = tolower(c);
				return true;
			}
		}
	}
	if (_kbhit()) { // don't let students get confused, poll both window and console
		*key = _getch();
		return true;
	}
	Sleep(1); // hack to avoid saturating CPU
	return false;
}

bool GetMouseInput(int* mouseX, int* mouseY, int* mouseButton)
{
	sPumpMessageQueue();
	while (!gMouseInputQueue.empty()) {
		MSG msg = gMouseInputQueue.front(); gMouseInputQueue.pop_front();
		int x = GET_X_LPARAM(msg.lParam);
		int y = GET_Y_LPARAM(msg.lParam);
		switch (msg.message) {
			case WM_MOUSEMOVE:   { if (!gMouseInputQueue.empty()) break;
			                       *mouseX = x; *mouseY = y; *mouseButton =  0; return true; }
			case WM_LBUTTONDOWN: { *mouseX = x; *mouseY = y; *mouseButton =  1; return true; }
			case WM_RBUTTONDOWN: { *mouseX = x; *mouseY = y; *mouseButton =  2; return true; }
			case WM_MBUTTONDOWN: { *mouseX = x; *mouseY = y; *mouseButton =  3; return true; }
			case WM_LBUTTONUP:   { *mouseX = x; *mouseY = y; *mouseButton = -1; return true; }
			case WM_RBUTTONUP:   { *mouseX = x; *mouseY = y; *mouseButton = -2; return true; }
			case WM_MBUTTONUP:   { *mouseX = x; *mouseY = y; *mouseButton = -3; return true; }
		}
	}
	Sleep(1); // hack to avoid saturating CPU
	return false;
}

void Pause(int milliseconds)
{
	sPumpMessageQueue();  // give the window a chance to redraw before sleep.
	Sleep(milliseconds);
	sPumpMessageQueue();
}

bool WasWindowClosed()
{
	Sleep(1); // hack to avoid saturating CPU
	sPumpMessageQueue();
	return gQuit;
}

void ShowWindow(const char* title)
{
	theMainWindow()->setText(charToWstr(title).c_str());
	theMainWindow()->setVisible(true);
	sPumpMessageQueue();
}

void HideWindow()
{
	theMainWindow()->setVisible(false);
	sPumpMessageQueue();
}

void DrawString(int x0, int y0, const char* str)
{
	theMainWindow()->drawString(x0, y0, charToWstr(str).c_str());
}

void SetDrawColour(double r, double g, double b)
{
	unsigned char cr = (unsigned char)(255.99*std::max(0.0, std::min(1.0, r)));
	unsigned char cg = (unsigned char)(255.99*std::max(0.0, std::min(1.0, g)));
	unsigned char cb = (unsigned char)(255.99*std::max(0.0, std::min(1.0, b)));
	theMainWindow()->setColour(cr, cg, cb);
}

void DrawRectangleOutline(int x0, int y0, int x1, int y1)
{
	theMainWindow()->drawRect(x0, y0, x1, y1);
}

void DrawRectangleFilled(int x0, int y0, int x1, int y1)
{
	theMainWindow()->fillRect(x0, y0, x1, y1);
}

void DrawEllipseOutline(int x, int y, int radiusX, int radiusY)
{
	theMainWindow()->drawEllipse(x-radiusX+1, y-radiusY+1, x+radiusX, y+radiusY);
}

void DrawEllipseFilled(int x, int y, int radiusX, int radiusY)
{
	theMainWindow()->fillEllipse(x-radiusX+1, y-radiusY+1, x+radiusX, y+radiusY);
}

void DrawPolyline(int* x, int* y, int count)
{
	if (count > 0) {
		int* pts = new int[2*count];
		for (int i = 0; i < count; ++i) {
			pts[2*i] = x[i]; pts[2*i+1] = y[i];
		}
		theMainWindow()->drawPolyline(pts, count);
		delete [] pts;
	}
}

void DrawPolygonFilled(int* x, int* y, int count)
{
	if (count > 0) {
		int* pts = new int[2*count];
		for (int i = 0; i < count; ++i) {
			pts[2*i] = x[i]; pts[2*i+1] = y[i];
		}
		theMainWindow()->fillPoly(pts, count);
		delete [] pts;
	}
}


void DrawImage(int x0, int y0, int handle)
{
	if (gBitmapRegistry.find(handle) != gBitmapRegistry.end())
		theMainWindow()->drawBitmap(x0, y0, gBitmapRegistry[handle]);
}

void DrawImageStretched(int x0, int y0, int x1, int y1, int handle)
{
	if (gBitmapRegistry.find(handle) != gBitmapRegistry.end())
		theMainWindow()->stretchBitmap(x0, y0, x1, y1, gBitmapRegistry[handle]);
}


int CreateImage(int sizeX, int sizeY, const char* options)
{
	int bpp = strstr(options, "bgra") ? 4 : 
	          strstr(options, "bgr")  ? 3 : 
			  strstr(options, "grey") ? 1 : 0;

	if (!bpp)
		return 0; // invalid options

	// Set BITMAPINFOHEADER fields
	BITMAPINFO bmi;
	memset(&bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = sizeX;
	bmi.bmiHeader.biHeight = sizeY;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = bpp*8;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Tell windows to allocate a DIB from the parameters we've set up
	BitmapInfo info;
	info.osHandle = CreateDIBSection(GetDC(0), &bmi, DIB_RGB_COLORS, 0, 0, 0); 
	GetObject(info.osHandle, sizeof(DIBSECTION), &info.dib);

	int handle = gNextBitmapID++;
	gBitmapRegistry[handle] = info;
	return handle;
}

int CreateImageFromFile(const char* fileName)
{
	// Ask Windows to do the dirty work
	HANDLE wh = LoadImage(0, charToWstr(fileName).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (!wh)
		return 0;

	// Ask for the bitmap details and remember them in the registry for easy access
	BitmapInfo info;
	info.osHandle = wh;	
	if (GetObject(wh, sizeof(info.dib), &info.dib) != sizeof(info.dib)) {
		DeleteObject(wh);
		return 0;
	}
	int handle = gNextBitmapID++;
	gBitmapRegistry[handle] = info;
	return handle;
}

int GetImageSizeX(int handle)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle);
	return bmp->dib.dsBm.bmWidth;
}

int GetImageSizeY(int handle)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle);
	return bmp->dib.dsBm.bmHeight;
}

int GetImageBytesPerPixel(int handle)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle);
	return bmp->dib.dsBm.bmBitsPixel >> 3;
}

unsigned char* GetImagePixelPtr(int handle, int x, int y)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle);
	AssertMsg(bmp, "Invalid image handle");
	int bpp = bmp->dib.dsBm.bmBitsPixel>>3;
	int flipy = bmp->dib.dsBm.bmHeight-y-1;
	AssertMsg(x >= 0 && y >= 0 && x < bmp->dib.dsBm.bmWidth && y < bmp->dib.dsBm.bmHeight,
	          "Requested pixel is outside image boundary");
	return (unsigned char*)bmp->dib.dsBm.bmBits 
			+ bmp->dib.dsBm.bmWidthBytes*flipy + x*bpp;
}

void DeleteImage(int handle)
{
	if (gBitmapRegistry.find(handle) != gBitmapRegistry.end()) {
		BitmapInfo& info = gBitmapRegistry[handle];
		DeleteObject(info.osHandle);
		gBitmapRegistry.erase(handle); // redundant searching but who cares
	}
}

