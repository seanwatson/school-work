/*******************************************************************************
  Course(s):
     CS1037, University of Western Ontario

  Author(s):
     Andrew Delong <firstname.lastname@gmail.com>

  Description:
     See cs1037util.h.

  History:
     Oct 12, 2009; - Can now specify default state for CreateDropList,CreateSlider,CreateCheckox

     Oct 10, 2009; - Added basic multi-threading functions

     Oct  9, 2009; - Added WriteImageToFile
                   - Added palettized BMP support
                   - Fixed possible initialization-order bugs

     Feb 15, 2009; - Fixed alpha blending on some video cards via premultiplied-alpha
                   - Force all images to internally use top-down raster order for pixels
                   - Fixed manifest problem on 64-bit targets

     Jan 23, 2009; - Compiles under VC71 (2003)
                   - Plot can have axis labels

     Dec 17, 2008; - Fixed leak of screen DC handle in CreateImage.

     Nov  8, 2008; - Added plotting functions, CreatePlot() etc.
                   - Added GetMicroseconds()

     Nov  5, 2008; - Added SetDrawAxis(), DrawLine(), SetWindowResizeCallback()
                   - Added GetMilliseconds(), GetRandomInt(), GetRandomFloat()

     Nov  4, 2008; - Added TextLabel, Slider controls
                   - Control callbacks now receive arguments (functionptr_string etc)

     Nov  3, 2008; - Button, CheckBox, DropList resize to accommodate text width
                   - SetWindowSize now refers to client size of window

     Nov  2, 2008; - Added TextBox control
                   - Set up ASSERT messages to be more helpful to students
                   - Made 'handle' always the first parameter, for consistency
                   - Got rid of bold default font for controls (why Windows retarded?)

     Nov  1, 2008; - Added DropList and CheckBox controls
                   - Added basic interface for managing controls

     Nov 13, 2007; - Added assertions on image routines
                   - Removed spurious 'Sleep' in SetDrawColour()

     Nov  2, 2007; - Discard WM_MOUSE_MOVE events if there are
                     newer events waiting
                   - Cache most recent handle->bitmap used

     Nov  1, 2007; - Added DrawPolyline(), DrawPolygonFilled()
                   - Added GetImageBytesPerPixel ()

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

#ifndef _CPPRTTI 
#error To use cs1037util, you must enable the Run-Time Type Information (RTTI) feature \
	under Project->Properties->C++->Language settings. \
	Don't forget to do it for both Debug and Release mode.
#endif

#define CS1037UTIL_DISABLE_ASSERT_MACROS
#include "cs1037util.h"
#include <conio.h>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define _WIN32_WINNT 0x501
#define WINVER       0x501
#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <process.h>
#include <tchar.h>
#include <time.h>
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)   ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)   ((int)(short)HIWORD(lp))
#endif
// Automatically link with comctl32 to make XP-style buttons etc the default
#pragma comment(lib, "comctl32") 
#ifdef _WIN64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
                         version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
                         version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma comment(lib, "msimg32") // link for AlphaBlend
#include <list>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

#ifdef _WIN64 // Stupid SetWindowLongPtr doesn't have the signature that the docs claim. Pfft.
#define SETWINDOWLONGPTR(hWnd, index, newLong) SetWindowLongPtr(hWnd, index, (LONG_PTR)(newLong))
#else
#define SETWINDOWLONGPTR(hWnd, index, newLong) (size_t)SetWindowLong(hWnd, index, (LONG)(size_t)(newLong))
#endif


#ifdef _UNICODE
typedef std::wstring wstr;
static wstr char2wstr(const char* str) {
	std::wstring result;
	if (str) {
		int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
		result.resize(len);
		MultiByteToWideChar(CP_UTF8, 0, str, -1, &result[0], len);
	}
	return result;
}

static std::string wchar2str(const TCHAR* str) {
	std::string result;
	if (str) {
		int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, 0, 0);
		result.resize(len-1);
		WideCharToMultiByte(CP_UTF8, 0, str, -1, &result[0], len, 0, 0);
	}
	return result;
}
#define sprintf_TEXT swprintf_s
#else
typedef std::string wstr;
static wstr char2wstr(const char* str) { return str; }
static std::string wchar2str(const char* str) { return str; }
#define sprintf_TEXT sprintf_s
#endif

#ifdef _WIN64
#define AssertBreak __debugbreak();
#else
#define AssertBreak __asm { int 0x3 }
#endif

#if _MSC_VER < 1400  // VC7.1 (2003) or below
// _s variants not defined until VC8 (2005)
static int vsprintf_s(char* dst, size_t, const char* format, va_list args)
{
	return vsprintf(dst, format, args);
}

static int sprintf_s(char* dst, size_t, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int result = vsprintf(dst, format, args);
	va_end(args);
	return result;
}
#endif

#define AssertMsg(expr,msg) { if (expr) { } else { sAssertDialog(msg, __FILE__, __LINE__); AssertBreak; } }
#define AssertUnreachable(msg) { sAssertDialog(msg, __FILE__, __LINE__); AssertBreak; }

static void sAssertDialog(const char* msg, const char* file, int line);

struct BitmapInfo {
	HANDLE osHandle;
	DIBSECTION dib;
};

static bool gQuit = false;

// For some alpha-blended excitement on XP/2000, uncomment this (oooooooh ghost!)
//#define WINDOW_ALPHA 70

//////////////////////////////////////////////////////////////////////
// Window
//
// Notes on window creation/deletion:
//  * To keep the implementation simple for CS1037, Window creation/deletion is NOT thread safe.
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
	virtual void  setPositionAndSize(int x, int y, int windowSizeX, int windowSizeY);
	virtual void  setSize(int windowSizeX, int windowSizeY);
	virtual void  setVisible(bool visible);

	int   screenX() const;
	int   screenY() const;
	int   relativeX() const; // relative to parent
	int   relativeY() const; // relative to parent
	int   windowSizeX() const;
	int   windowSizeY() const;
	int   clientSizeX() const;
	int   clientSizeY() const;
	void  fitSizeToText();

	void        setParent(Window* window);
	Window*     parent();
	Window*     child(const char* name);

	static Window* sWindowFromHandle(HWND hWnd);

	// Should use delegates but to keep the implementation simple 
	// this void* nonsense will be good enough, probably.
	typedef void (*OnClickHandler)(void* userData, Window* sender);
	typedef void (*OnSizeHandler)(void* userData, Window* sender, int windowSizeX, int windowSizeY);

	void setOnSize(OnSizeHandler onSize, void* userData);

	// ATTN: it is **NOT** safe to request handle() from within subclass 
	//       constructors (expects VMT of subclasses to be loaded!)
	HWND  handle() const; 

protected:
	enum { 
		 WM_COMMAND_REFLECT = WM_USER+0x0001
	    ,WM_SCROLL_REFLECT 
	};

	// All subclasses of Window should have protected/private destructors, and
	// rely on the destroy() method above.
	//
	// The destructor is not public so that programmers will not be able to create
	// a Window object on the stack or as global data. This way, we know all child
	// windows were created on the heap and so we can clean up after ourselves.
	//
	virtual ~Window();

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

	virtual void    adjustFitSizeToText(RECT& rect) const;

	static HFONT                    sDefaultFont;
	static HFONT                    sDefaultFontVertical;

private:
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
	OnSizeHandler mOnSize;
	void*         mOnSizeData;

private:
	// Window instance registration
	static const TCHAR*             cBaseWndClassName;
	static Window*                  sWindowBeingCreated;
	static Window*                  sWindowBeingCreatedParent;
	static LRESULT CALLBACK         sGlobalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

const TCHAR* Window::cBaseWndClassName = TEXT("cs1037window");
HFONT Window::sDefaultFont = 0;
HFONT Window::sDefaultFontVertical = 0;

Window::Window(const char* name)
: mHWND(0)
, mParent(0)
, mName(name)
, mBaseWndProc(0)
, mIsDestroyed(false)
, mOnSize(0)
, mOnSizeData(0)
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
	dwStyle   |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
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
	SetWindowPos(handle(), 0, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER);
}

void Window::setPositionAndSize(int x, int y, int windowSizeX, int windowSizeY)
{
	SetWindowPos(handle(), 0, x, y, windowSizeX, windowSizeY, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void Window::setSize(int windowSizeX, int windowSizeY)
{
	SetWindowPos(handle(), 0, 0, 0, windowSizeX, windowSizeY, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
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

void Window::fitSizeToText()
{
	HDC dc = GetDC(handle());
	int textLen = (int)SendMessage(handle(), WM_GETTEXTLENGTH, 0, 0);
	TCHAR* text = new TCHAR[textLen+1];
	SendMessage(handle(), WM_GETTEXT, textLen+1, (LPARAM)text);
	RECT r = { 0, 0, 0, 0 };
	DrawTextEx(dc, text, -1, &r, DT_LEFT | DT_TOP | DT_CALCRECT, 0);
	delete [] text;
	ReleaseDC(handle(), dc);
	adjustFitSizeToText(r);
	setSize(r.right, r.bottom);
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

void Window::setOnSize(OnSizeHandler onSize, void* userData)
{
	mOnSize = onSize;
	mOnSizeData = userData;
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
	SETWINDOWLONGPTR(mHWND, GWL_STYLE, dwStyle);

	if (!sDefaultFont) {
		sDefaultFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, \
			  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, \
			  FF_DONTCARE, 0);
		sDefaultFontVertical = CreateFont(15, 0, 900, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, \
			  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, \
			  FF_DONTCARE, 0);
	}

	SendMessage(mHWND, WM_SETFONT, (WPARAM)sDefaultFont, TRUE);
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
			if (wParam == IDOK && !lParam) {
				// If user hit enter on a control, reflect 
				// the IDOK event to that control so that it
				// can react to the ENTER key
				lParam = (LPARAM)GetFocus();
			}
			if (lParam) {
				SendMessage(reinterpret_cast<HWND>(lParam), WM_COMMAND_REFLECT, wParam, lParam);
				break;
			}
		}
		case WM_HSCROLL:
		case WM_VSCROLL: {
			if (lParam) {
				SendMessage(reinterpret_cast<HWND>(lParam), WM_SCROLL_REFLECT, wParam, lParam);
				break;
			}
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
bool Window::onKeyDown(unsigned /*key*/) { return true; }
bool Window::onKeyUp(unsigned /*key*/) { return true; }
bool Window::onMouseMove(int /*x*/, int /*y*/) { return true; }
bool Window::onMouseDown(int /*x*/, int /*y*/, int /*button*/) { return true; }
bool Window::onMouseUp(int /*x*/, int /*y*/, int /*button*/) { return true; }
bool Window::onPaint() { return true; }
bool Window::onParentChanged() { return true; }

bool Window::onSizeChanged() { 
	if (mOnSize)
		mOnSize(mOnSizeData, this, windowSizeX(), windowSizeY());
	return true; 
}

void Window::adjustFitSizeToText(RECT& /*rect*/) const { }

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
// StaticCtl

class StaticCtl: public Window {
	typedef Window Super;
public:
	StaticCtl(const char* name);
protected:
	virtual ~StaticCtl();
	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual void    adjustFitSizeToText(RECT& rect) const;
};

StaticCtl::StaticCtl(const char *name)
: Super(name)
{ }

StaticCtl::~StaticCtl() { }


void StaticCtl::getWndClassStyle(WNDCLASS& wndClass) const
{
	Super::getWndClassStyle(wndClass);
	wndClass.lpszClassName = TEXT("STATIC");
}

void StaticCtl::getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const
{
	Super::getWndStyle(dwStyle, dwExStyle);
	dwStyle |= SS_CENTER | SS_CENTERIMAGE;
}

void StaticCtl::adjustFitSizeToText(RECT& rect) const
{
	rect.right  += 2;
	rect.bottom += 7;
}


//////////////////////////////////////////////////////////////////////
// ButtonCtl

class ButtonCtl: public Window {
	typedef Window Super;
public:
	ButtonCtl(const char* name);

	void setOnClick(OnClickHandler onClick, void* userData);

protected:
	virtual ~ButtonCtl();
	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void    adjustFitSizeToText(RECT& rect) const;

private:
	OnClickHandler mOnClick;
	void*          mOnClickData;
};

ButtonCtl::ButtonCtl(const char *name)
: Super(name)
, mOnClick(0)
, mOnClickData(0) 
{ }

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
	dwStyle |= BS_PUSHBUTTON | BS_TEXT | WS_TABSTOP;
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

void ButtonCtl::adjustFitSizeToText(RECT& rect) const
{
	if (rect.right < 50)
		rect.right = 50;
	rect.right  += 10;
	rect.bottom += 7;
}

//////////////////////////////////////////////////////////////////////
// ComboBoxCtl

class ComboBoxCtl: public Window {
	typedef Window Super;
public:
	ComboBoxCtl(const char* name);

	typedef void (*OnSelectionChangedHandler)(void* userData, Window* sender);
	void setOnSelectionChanged(OnSelectionChangedHandler onSelectionChanged, void* userData);
	void insertItem(int pos, const char* text);
	void setSelectedItem(int index);
	int  getSelectedItem() const;
	int  itemCount() const;
	void fitSizeToItems();

protected:
	virtual ~ComboBoxCtl();
	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	int                       mPrevSelectedItem;
	OnSelectionChangedHandler mOnSelectionChanged;
	void*                     mOnSelectionChangedData;
};

ComboBoxCtl::ComboBoxCtl(const char *name)
: Super(name)
, mOnSelectionChanged(0)
, mOnSelectionChangedData(0) 
, mPrevSelectedItem(0)
{ }

ComboBoxCtl::~ComboBoxCtl() { }

void ComboBoxCtl::setOnSelectionChanged(OnSelectionChangedHandler onSelectionChanged, void* userData)
{
	mOnSelectionChanged = onSelectionChanged;
	mOnSelectionChangedData = userData;
}

void ComboBoxCtl::insertItem(int pos, const char* text)
{
	SendMessage(handle(), CB_INSERTSTRING, (WPARAM)pos, (LPARAM)char2wstr(text).c_str());
}

int  ComboBoxCtl::getSelectedItem() const
{
	return (int)SendMessage(handle(), CB_GETCURSEL, 0, 0);
}

void ComboBoxCtl::setSelectedItem(int index)
{
	if (index != getSelectedItem()) {
		SendMessage(handle(), CB_SETCURSEL, (WPARAM)index, 0);
		if (mPrevSelectedItem != index) {
			mPrevSelectedItem = index;
			if (mOnSelectionChanged)
				mOnSelectionChanged(mOnSelectionChangedData, this);
		}
	}
}

int  ComboBoxCtl::itemCount() const
{
	return (int)SendMessage(handle(), CB_GETCOUNT, 0, 0);
}

void ComboBoxCtl::fitSizeToItems()
{
	int maxItemSizeX = 100;	
	int count = itemCount();
	HDC dc = GetDC(handle());
	for (int i = 0; i < count; ++i) {
		int itemLen = (int)SendMessage(handle(), CB_GETLBTEXTLEN, i, 0);
		TCHAR* item = new TCHAR[itemLen+1];
		SendMessage(handle(), CB_GETLBTEXT, i, (LPARAM)item);
		RECT r = { 0, 0, 0, 0 };
		DrawTextEx(dc, item, -1, &r, DT_LEFT | DT_TOP | DT_CALCRECT, 0);
		delete [] item;
		if (r.right > maxItemSizeX)
			maxItemSizeX = r.right;
	}
	ReleaseDC(handle(), dc);
	setSize(maxItemSizeX+10, windowSizeY());
}

void ComboBoxCtl::getWndClassStyle(WNDCLASS& wndClass) const
{
	Super::getWndClassStyle(wndClass);
	wndClass.lpszClassName = TEXT("COMBOBOX");
}

void ComboBoxCtl::getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const
{
	Super::getWndStyle(dwStyle, dwExStyle);
	dwStyle |= CBS_DROPDOWNLIST | WS_TABSTOP;
}

LRESULT ComboBoxCtl::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {
		case WM_COMMAND_REFLECT: {
			if (HIWORD(wParam) == CBN_SELENDOK && mOnSelectionChanged) {
				if (getSelectedItem() != mPrevSelectedItem) { // keep silent if user re-selects already selected item
					mPrevSelectedItem = getSelectedItem();
					mOnSelectionChanged(mOnSelectionChangedData, this);
				}
			}
			return 0;
		}
	}
	return Super::wndProc(uMsg, wParam, lParam);
}


//////////////////////////////////////////////////////////////////////
// CheckBoxCtl

class CheckBoxCtl: public Window {
	typedef Window Super;
public:
	CheckBoxCtl(const char* name);

	typedef void (*OnStateChangedHandler)(void* userData, Window* sender);
	void setOnStateChanged(OnStateChangedHandler onStateChanged, void* userData);
	void setState(bool selected);
	bool getState() const;

protected:
	virtual ~CheckBoxCtl();
	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void    adjustFitSizeToText(RECT& rect) const;

private:
	OnStateChangedHandler mOnStateChanged;
	void*                 mOnStateChangedData;
};

CheckBoxCtl::CheckBoxCtl(const char *name)
: Super(name)
, mOnStateChanged(0)
, mOnStateChangedData(0) 
{ }

CheckBoxCtl::~CheckBoxCtl() { }

void CheckBoxCtl::setOnStateChanged(OnStateChangedHandler onStateChanged, void* userData)
{
	mOnStateChanged = onStateChanged;
	mOnStateChangedData = userData;
}

bool CheckBoxCtl::getState() const
{
	return SendMessage(handle(), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void CheckBoxCtl::setState(bool selected)
{
	if (selected != getState()) {
		SendMessage(handle(), BM_SETCHECK, (WPARAM)selected ? BST_CHECKED : BST_UNCHECKED, 0);
		if (mOnStateChanged)
			mOnStateChanged(mOnStateChangedData, this);
	}
}

void CheckBoxCtl::getWndClassStyle(WNDCLASS& wndClass) const
{
	Super::getWndClassStyle(wndClass);
	wndClass.lpszClassName = TEXT("BUTTON");
}

void CheckBoxCtl::getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const
{
	Super::getWndStyle(dwStyle, dwExStyle);
	dwStyle |= BS_AUTOCHECKBOX | WS_TABSTOP;
}

LRESULT CheckBoxCtl::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {
		case WM_COMMAND_REFLECT: {
			if (HIWORD(wParam) == BN_CLICKED && mOnStateChanged) {
				mOnStateChanged(mOnStateChangedData, this);
			}
			return 0;
		}
	}
	return Super::wndProc(uMsg, wParam, lParam);
}

void CheckBoxCtl::adjustFitSizeToText(RECT& rect) const
{
	rect.right  += 25;
	rect.bottom += 7;
}


//////////////////////////////////////////////////////////////////////
// EditCtl

class EditCtl: public Window {
	typedef Window Super;
public:
	EditCtl(const char* name);

	typedef void (*OnStringChangedHandler)(void* userData, Window* sender);
	void setOnStringChanged(OnStringChangedHandler onStringChanged, void* userData);
	void setString(const char* value);
	const char* getString() const;

protected:
	virtual ~EditCtl();
	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	OnStringChangedHandler mOnStringChanged;
	void*                  mOnStringChangedData;
	std::string            mString;
};

EditCtl::EditCtl(const char *name)
: Super(name)
, mOnStringChanged(0)
, mOnStringChangedData(0) 
{ }

EditCtl::~EditCtl() { }

void EditCtl::setOnStringChanged(OnStringChangedHandler onStringChanged, void* userData)
{
	mOnStringChanged = onStringChanged;
	mOnStringChangedData = userData;
}

void EditCtl::setString(const char* str)
{
	if (mString != str) {
		mString = str;
		SendMessage(handle(), WM_SETTEXT, (WPARAM)0, (LPARAM)char2wstr(str).c_str());
		if (mOnStringChanged)
			mOnStringChanged(mOnStringChangedData, this);
	}
}

const char* EditCtl::getString() const
{
	return mString.c_str();
}

void EditCtl::getWndClassStyle(WNDCLASS& wndClass) const
{
	Super::getWndClassStyle(wndClass);
	wndClass.lpszClassName = TEXT("EDIT");
}

void EditCtl::getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const
{
	Super::getWndStyle(dwStyle, dwExStyle);
	dwStyle |= WS_TABSTOP | ES_AUTOHSCROLL;
	dwExStyle |= WS_EX_CLIENTEDGE;
}

LRESULT EditCtl::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	if (uMsg == WM_COMMAND_REFLECT && (HIWORD(wParam) == EN_KILLFOCUS || wParam == IDOK)) {
		int textLen = (int)SendMessage(handle(), WM_GETTEXTLENGTH, 0, 0);
		TCHAR* buffer = new TCHAR[textLen+1];
		SendMessage(handle(), WM_GETTEXT, (WPARAM)textLen+1, (LPARAM)buffer);
		std::string newStr = wchar2str(buffer);
		if (mString != newStr) {
			mString = newStr;
			if (mOnStringChanged)
				mOnStringChanged(mOnStringChangedData, this);
		}
		delete [] buffer;
		return 0;
	}
	return Super::wndProc(uMsg, wParam, lParam);
}


//////////////////////////////////////////////////////////////////////
// TrackbarCtl

class TrackbarCtl: public Window {
	typedef Window Super;
public:
	TrackbarCtl(const char* name, bool isHoriz);

	typedef void (*OnValueChangedHandler)(void* userData, Window* sender);
	void setOnValueChanged(OnValueChangedHandler onValueChanged, void* userData);
	void setValue(float value);
	float getValue() const;

protected:
	virtual ~TrackbarCtl();
	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual LRESULT wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	OnValueChangedHandler  mOnValueChanged;
	void*                  mOnValueChangedData;
	float                  mPrevValue;
	bool                   mIsHoriz;
};

TrackbarCtl::TrackbarCtl(const char *name, bool isHoriz)
: Super(name)
, mOnValueChanged(0)
, mOnValueChangedData(0) 
, mPrevValue(0.0f)
, mIsHoriz(isHoriz)
{ }

TrackbarCtl::~TrackbarCtl() { }

void TrackbarCtl::setOnValueChanged(OnValueChangedHandler onValueChanged, void* userData)
{
	mOnValueChanged = onValueChanged;
	mOnValueChangedData = userData;
}

void TrackbarCtl::setValue(float value)
{
	if (mPrevValue != value) {
		int rmin = (int)SendMessage(handle(), TBM_GETRANGEMIN, 0, 0);
		int rmax = (int)SendMessage(handle(), TBM_GETRANGEMAX, 0, 0);
		int pos = rmin + (int)(value * (rmax - rmin));
		SendMessage(handle(), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)pos);
		if (mPrevValue != getValue()) {
			mPrevValue = getValue();
			if (mOnValueChanged)
				mOnValueChanged(mOnValueChangedData, this);
		}
	}
}

float TrackbarCtl::getValue() const
{
	int pos = (int)SendMessage(handle(), TBM_GETPOS, 0, 0);
	int rmin = (int)SendMessage(handle(), TBM_GETRANGEMIN, 0, 0);
	int rmax = (int)SendMessage(handle(), TBM_GETRANGEMAX, 0, 0);
	if (!mIsHoriz)
		pos = rmax - (pos-rmin); // flip so that [bottom,top] map to [0,1]
	if (rmin < rmax)
		return (float)(pos-rmin) / (float)(rmax-rmin);
	return 0;
}

void TrackbarCtl::getWndClassStyle(WNDCLASS& wndClass) const
{
	Super::getWndClassStyle(wndClass);
	wndClass.lpszClassName = TRACKBAR_CLASS;
}

void TrackbarCtl::getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const
{
	Super::getWndStyle(dwStyle, dwExStyle);
	dwStyle |= WS_TABSTOP | (mIsHoriz ? TBS_HORZ : TBS_VERT);
}

LRESULT TrackbarCtl::wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {
		case WM_SCROLL_REFLECT: {
			if (lParam && (LOWORD(wParam) >= TB_LINEUP && LOWORD(wParam) <= TB_ENDTRACK)) {
				float value = getValue();
				if (mPrevValue != value) {
					mPrevValue = value;
					if (mOnValueChanged)
						mOnValueChanged(mOnValueChangedData, this);
				}
				return 0;
			}
			break;
		}
		case WM_ERASEBKGND: { return 1; }
	}		
	return Super::wndProc(uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
// SimpleCanvas

class SimpleCanvas: public Window {
	typedef Window Super;
public:
	SimpleCanvas(const char* name);

	// The off screen canvas is, by default, the same size as the client area of the
	// window when it is first drawn. This method resizes the canvas area, preserving as 
	// much of the existing pixels as possible.
	void setBitmapSize(int sizeX, int sizeY);

	void setAxisOrigin(int x0, int y0);
	void setAxisFlipY(bool flip);
	int  axisOriginX() const;
	int  axisOriginY() const;
	bool axisFlipY() const;

	// Simple drawing methods. Relevant client area is automatically invalidated.
	void setColour(unsigned char r, unsigned char g, unsigned char b);
	POINT measureString(const TCHAR* str, bool vertical=false);
	void drawString(int x0, int y0, const TCHAR* str, bool centered=false, bool vertical=false);
	void drawRect(int x0, int y0, int x1, int y1);
	void fillRect(int x0, int y0, int x1, int y1);
	void drawEllipse(int x0, int y0, int x1, int y1);
	void fillEllipse(int x0, int y0, int x1, int y1);
	void drawLine(int x0, int y0, int x1, int y1);
	void drawPolyline(int* pts, int count);
	void drawPolyline(int* xs, int* ys, int count);
	void fillPoly(int* pts, int count);
	void drawBitmap(int x0, int y0, const BitmapInfo& bmp);
	void stretchBitmap(int x0, int y0, int x1, int y1, const BitmapInfo& bmp);

	HDC             DC();
	HDC             offScreenDC();

protected:
	virtual ~SimpleCanvas();

	virtual void    getWndClassStyle(WNDCLASS& wndClass) const;

	virtual bool    onMouseDown(int x, int y, int button);
	virtual bool    onPaint();

	void transformRect(RECT* r);

private:
	HDC     mOwnDC;
	HDC     mOffScreenDC;
	HBITMAP mOffScreenBM;
	HBRUSH  mActiveBrush; // must be explicitly selected when needed
	HPEN    mActivePen;   // must be explicitly selected when needed
	int     mX0, mY0;
	int     mYDir;
};


SimpleCanvas::SimpleCanvas(const char* name)
: Super(name)
, mOwnDC(0)
, mOffScreenDC(0)
, mOffScreenBM(0)
, mActiveBrush(0)
, mActivePen(0)
, mX0(0)
, mY0(0)
, mYDir(1)
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

void SimpleCanvas::setAxisOrigin(int x0, int y0) { mX0 = x0; mY0 = y0; }
void SimpleCanvas::setAxisFlipY(bool flip) { mYDir = flip ? -1 : 1; }
int  SimpleCanvas::axisOriginX() const { return mX0; }
int  SimpleCanvas::axisOriginY() const { return mY0; } 
bool SimpleCanvas::axisFlipY() const { return mYDir < 0; }

void SimpleCanvas::setBitmapSize(int sizeX, int sizeY)
{
	HBITMAP newBM = CreateCompatibleBitmap(DC(), sizeX, sizeY);
	if (!newBM)
		throw std::runtime_error("CreateCompatibleBitmap failed.");
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
		if (oldBM != mOffScreenBM)
			throw std::runtime_error("Unexpected HGDIOBJ was selected for off-screen bitmap.");
		DeleteObject(mOffScreenBM);
	} else {
		SelectObject(offScreenDC(), newBM);
		HGDIOBJ oldBM = SelectObject(offScreenDC(), newBM);
		if (oldBM != newBM)
			throw std::runtime_error("SelectObject failed to select new off-screen bitmap.");
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

POINT SimpleCanvas::measureString(const TCHAR* str, bool vertical)
{
	RECT r = { 0, 0, 0, 0 };
	HGDIOBJ oldFont = 0;
	if (vertical)
		oldFont = SelectObject(offScreenDC(), sDefaultFontVertical);
	DrawTextEx(offScreenDC(), const_cast<TCHAR*>(str), -1, &r, DT_LEFT | DT_TOP | DT_CALCRECT, 0);
	POINT p = { r.right-r.left, r.bottom-r.top };
	if (vertical) {
		SelectObject(offScreenDC(), oldFont);
		std::swap(p.x, p.y);
	}
	return p;
}

void SimpleCanvas::drawString(int x0, int y0, const TCHAR* str, bool centered, bool vertical)
{
	int bkMode = SetBkMode(offScreenDC(), TRANSPARENT);
	RECT r = { x0, y0, x0, y0 };
	transformRect(&r);
	HGDIOBJ oldFont = 0;
	if (vertical)
		oldFont = SelectObject(offScreenDC(), sDefaultFontVertical);
	DrawTextEx(offScreenDC(), const_cast<TCHAR*>(str), -1, &r, DT_LEFT | DT_TOP | DT_CALCRECT, 0);
	int w = r.right-r.left, h = r.bottom-r.top;
	if (vertical) {
		// Rotate the rectangle.
		r.right = r.left + h;
		r.bottom = r.top + w;
		std::swap(r.top, r.bottom);
		if (centered)
			OffsetRect(&r, -h/2, mYDir*w/2);
	} else {
		if (centered)
			OffsetRect(&r, -w/2, mYDir*h/2);
	}
	DrawTextEx(offScreenDC(), const_cast<TCHAR*>(str), -1, &r, DT_LEFT | DT_TOP, 0);
	if (vertical)
		SelectObject(offScreenDC(), oldFont);

	SetBkMode(offScreenDC(), bkMode);
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::fillRect(int x0, int y0, int x1, int y1)
{
	RECT r = { x0, y0, x1, y1 };
	transformRect(&r);
	FillRect(offScreenDC(), &r, mActiveBrush);
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::drawRect(int x0, int y0, int x1, int y1)
{
	RECT r = { x0, y0, x1, y1 };
	transformRect(&r);
	FrameRect(offScreenDC(), &r, mActiveBrush);
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::fillEllipse(int x0, int y0, int x1, int y1)
{
	RECT r = { x0, y0, x1, y1 };
	transformRect(&r);
	if (abs(x1-x0) == 1 || abs(y1-y0) == 1) {      // special case: for single-pixel, Ellipse() 
		FillRect(offScreenDC(), &r, mActiveBrush); // doesn't draw anything, so use FillRect()
	} else {
		HGDIOBJ oldBrush = SelectObject(offScreenDC(), mActiveBrush);
		HGDIOBJ oldPen = SelectObject(offScreenDC(), mActivePen);
		Ellipse(offScreenDC(), r.left, r.top, r.right, r.bottom);
		SelectObject(offScreenDC(), oldBrush);
		SelectObject(offScreenDC(), oldPen);
	}
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::drawEllipse(int x0, int y0, int x1, int y1)
{
	RECT r = { x0, y0, x1, y1 };
	transformRect(&r);
	if (abs(x1-x0) == 1 || abs(y1-y0) == 1) {      // special case: for single-pixel, Ellipse() 
		FillRect(offScreenDC(), &r, mActiveBrush); // doesn't draw anything, so use FillRect()
	} else {
		HGDIOBJ oldBrush = SelectObject(offScreenDC(), GetStockObject(NULL_BRUSH));
		HGDIOBJ oldPen = SelectObject(offScreenDC(), mActivePen);
		Ellipse(offScreenDC(), r.left, r.top, r.right, r.bottom);
		SelectObject(offScreenDC(), oldBrush);
		SelectObject(offScreenDC(), oldPen);
	}
	InvalidateRect(handle(), &r, FALSE);
}

void SimpleCanvas::drawLine(int x0, int y0, int x1, int y1)
{
	int pts[4] = { x0, y0, x1, y1 };
	drawPolyline(pts, 2);
}

void SimpleCanvas::fillPoly(int* pts, int count)
{
	HGDIOBJ oldBrush = SelectObject(offScreenDC(), mActiveBrush);
	HGDIOBJ oldPen = SelectObject(offScreenDC(), mActivePen);
	POINT* ptsXform = new POINT[count];
	for (int i = 0; i < count; ++i) {
		ptsXform[i].x =       pts[i*2  ] + mX0;
		ptsXform[i].y = mYDir*pts[i*2+1] + mY0;
	}
	Polygon(offScreenDC(), ptsXform, count);
	delete [] ptsXform;
	SelectObject(offScreenDC(), oldBrush);
	SelectObject(offScreenDC(), oldPen);
	InvalidateRect(handle(), NULL, FALSE);
}

void SimpleCanvas::drawPolyline(int* pts, int count)
{
	HGDIOBJ oldBrush = SelectObject(offScreenDC(), GetStockObject(NULL_BRUSH));
	HGDIOBJ oldPen = SelectObject(offScreenDC(), mActivePen);
	POINT* ptsXform = new POINT[count];
	for (int i = 0; i < count; ++i) {
		ptsXform[i].x =       pts[i*2  ] + mX0;
		ptsXform[i].y = mYDir*pts[i*2+1] + mY0;
	}
	Polyline(offScreenDC(), ptsXform, count);
	delete [] ptsXform;
	SelectObject(offScreenDC(), oldBrush);
	SelectObject(offScreenDC(), oldPen);
	InvalidateRect(handle(), NULL, FALSE);
}

void SimpleCanvas::drawPolyline(int* xs, int* ys, int count)
{
	if (count > 0) {
		int* pts = new int[2*count];
		for (int i = 0; i < count; ++i) {
			pts[2*i] = xs[i]; pts[2*i+1] = ys[i];
		}
		drawPolyline(pts, count);
		delete [] pts;
	}
}

void SimpleCanvas::drawBitmap(int x0, int y0, const BitmapInfo& bmp)
{
	stretchBitmap(x0, y0, x0 + mYDir*bmp.dib.dsBmih.biWidth, y0 + mYDir*bmp.dib.dsBmih.biHeight, bmp);
}

void SimpleCanvas::stretchBitmap(int x0, int y0, int x1, int y1, const BitmapInfo& bmp)
{
	RECT r = { x0, y0, x1, y1 };
	transformRect(&r);
	if (mYDir < 0) {
		OffsetRect(&r, 0, r.bottom-r.top);
	}
	if (bmp.dib.dsBm.bmBitsPixel == 32) {
		// alpha blending enabled ... but can be sloowwww because on windows the image must be pre-multiplied by alpha
		HBITMAP bm = (HBITMAP)CopyImage(bmp.osHandle, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		if (!bm)
			throw std::runtime_error("CopyImage failed");
		DIBSECTION dib;
		GetObject(bm, sizeof(DIBSECTION), &dib);
		for (int y = 0; y < dib.dsBm.bmHeight; ++y) {
			unsigned char* p = (unsigned char*)dib.dsBm.bmBits + dib.dsBm.bmWidthBytes*y;
			unsigned char* p_end = p + dib.dsBm.bmWidth*4;
			while (p < p_end) {
				p[0] = p[0]*p[3]/255;
				p[1] = p[1]*p[3]/255;
				p[2] = p[2]*p[3]/255;
				p += 4;
			}
		}

		HDC dc = CreateCompatibleDC(offScreenDC());
		SelectObject(dc, bm);
		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, (dib.dsBm.bmBitsPixel == 32) ? AC_SRC_ALPHA : 0};
		AlphaBlend(offScreenDC(), r.left, r.top, r.right-r.left, r.bottom-r.top, dc, 0, 0, 
			dib.dsBmih.biWidth, dib.dsBmih.biHeight, blend); 
		DeleteDC(dc);
		DeleteObject(bm);
	} else {
		struct {
			BITMAPINFOHEADER bmih;
			RGBQUAD pal[256];
		} bmi;
		bmi.bmih = bmp.dib.dsBmih;
		if (bmi.bmih.biHeight > 0)
			bmi.bmih.biHeight = -bmi.bmih.biHeight; // signify top-down bitmap
		if (bmi.bmih.biBitCount == 8) {
			// Build a grayscale palette since I can't figure out how to get freaking Windows to do this automatically.
			bmi.bmih.biClrUsed = 256;
			bmi.bmih.biClrImportant = 256;
			for (int i = 0; i < 256; ++i) { // grayscale palette
				bmi.pal[i].rgbBlue = bmi.pal[i].rgbGreen = bmi.pal[i].rgbRed = (unsigned char)i;
				bmi.pal[i].rgbReserved = 255;
			}
		}
		StretchDIBits(offScreenDC(), r.left, r.top, r.right-r.left, r.bottom-r.top, 
			0, 0, bmp.dib.dsBmih.biWidth, bmp.dib.dsBmih.biHeight,
			bmp.dib.dsBm.bmBits, (const BITMAPINFO*)&bmi, DIB_RGB_COLORS, SRCCOPY);
	}
	InvalidateRect(handle(), &r, FALSE);
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
		if (!mOffScreenBM)
			setBitmapSize(clientSizeX(), clientSizeY());
		SelectObject(mOffScreenDC, sDefaultFont);
	}
	return mOffScreenDC;
}

void SimpleCanvas::getWndClassStyle(WNDCLASS& wndClass) const
{
	Super::getWndClassStyle(wndClass);
	wndClass.style |= CS_OWNDC; // lets us keep our mOwnDC without releasing it
}

bool SimpleCanvas::onMouseDown(int x, int y, int button)
{
	SetFocus(handle());
	return Super::onMouseDown(x, y, button);
}

bool SimpleCanvas::onPaint()
{
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

void SimpleCanvas::transformRect(RECT* r)
{
	if (mYDir < 0) {
		int temp = r->bottom;
		r->bottom = -r->top+1;
		r->top = -temp+1;
	}
	OffsetRect(r, mX0, mY0);
}


//////////////////////////////////////////////////////////////////////
// SimpleCanvasApp

class SimpleCanvasApp: public SimpleCanvas {
	typedef SimpleCanvas Super;
public:
	SimpleCanvasApp(const char* name);
	virtual void  setPositionAndSize(int x, int y, int windowSizeX, int windowSizeY);
	virtual void  setSize(int windowSizeX, int windowSizeY);
protected:
	virtual ~SimpleCanvasApp();
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual void    realize();
	virtual bool    onClose();
};

SimpleCanvasApp::SimpleCanvasApp(const char* name)
: Super(name)
{
}

SimpleCanvasApp::~SimpleCanvasApp()
{
}

void SimpleCanvasApp::setPositionAndSize(int x, int y, int windowSizeX, int windowSizeY)
{
	RECT clientRect, windowRect;
	GetClientRect(handle(), &clientRect);
	GetWindowRect(handle(), &windowRect);  // setSize should set the size of the *client* area
	int marginX = (windowRect.right-windowRect.left)-(clientRect.right-clientRect.left);
	int marginY = (windowRect.bottom-windowRect.top)-(clientRect.bottom-clientRect.top);
	Super::setPositionAndSize(x, y, windowSizeX + marginX, windowSizeY + marginY);
}

void SimpleCanvasApp::setSize(int windowSizeX, int windowSizeY)
{
	RECT clientRect, windowRect;
	GetClientRect(handle(), &clientRect);
	GetWindowRect(handle(), &windowRect);  // setSize should set the size of the *client* area
	int marginX = (windowRect.right-windowRect.left)-(clientRect.right-clientRect.left);
	int marginY = (windowRect.bottom-windowRect.top)-(clientRect.bottom-clientRect.top);
	Super::setSize(windowSizeX + marginX, windowSizeY + marginY);
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
	dwStyle   &= ~(WS_CHILD | WS_CHILDWINDOW | WS_VISIBLE);
}

bool SimpleCanvasApp::onClose()
{
	gQuit = true;
	return true; // allow window to be destroyed
}


//////////////////////////////////////////////////////////////////////
// PlotCtl


class PlotCtl: public SimpleCanvas {
	typedef SimpleCanvas Super;
public:
	PlotCtl(const char* name);

	void setTitle(const char* title);
	void setAxisLabels(const char* labelX, const char* labelY);
	int  addSeries(float* x, float* y, int count, const char* title, const char* style);
	void addSeriesPoint(int seriesID, float x, float y);
	int  seriesCount() const;

protected:
	virtual ~PlotCtl();
	virtual void    getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const;
	virtual bool    onPaint();

	void redrawPlot();
	void setDirty();
	void formatAxisLabel(TCHAR* dst, int size, float value);

	enum Shape { cNoShape, cStar, cPlus, cCross, cPoint, cNumSeriesShapes };
	enum Line  { cNoLine, cSolid, cDotted, cNumSeriesLines };

	struct Series {
		std::vector<float> x;
		std::vector<float> y;
		wstr title;
		unsigned char colour[3];
		Shape shape;
		Line line;
	};

	int mNextSeriesStyle;
	std::map<int,Series> mSeries;
	wstr mTitle;
	wstr mAxisLabelX, mAxisLabelY;
	float mAxisMinX, mAxisMinY;
	float mAxisMaxX, mAxisMaxY;
	bool mAutoAxis;
	bool mDrawAxisLabels;
	bool mDrawLegend;
	bool mDirty;

	static int sNextSeriesID;
	static const int cNumSeriesColours = 6;
	static unsigned char sSeriesColours[cNumSeriesColours][3];
};

int PlotCtl::sNextSeriesID = 1;
unsigned char PlotCtl::sSeriesColours[][3] = {
	 {   0,   0, 220 }
	,{ 220,   0,   0 }
	,{   0, 220,   0 }
	,{ 220, 128,   0 }
	,{ 190,   0, 200 }
	,{ 128, 128, 128 }
};

PlotCtl::PlotCtl(const char* name)
: Super(name)
, mNextSeriesStyle(0)
, mDirty(true)
, mAxisMinX(0.0f), mAxisMinY(0.0f)
, mAxisMaxX(1.0f),  mAxisMaxY(1.0f)
, mAutoAxis(true)
, mDrawAxisLabels(true)
, mDrawLegend(true)
{
}

PlotCtl::~PlotCtl()
{
}

void PlotCtl::setTitle(const char* title)
{
	mTitle = char2wstr(title);
	setDirty();
}

void PlotCtl::setAxisLabels(const char* labelX, const char* labelY)
{
	mAxisLabelX = char2wstr(labelX);
	mAxisLabelY = char2wstr(labelY);
	setDirty();
}

int PlotCtl::addSeries(float* x, float* y, int count, const char* title, const char* style)
{
	setDirty();

	int series = sNextSeriesID++;
	Series& s = mSeries[series];
	if (title) {
		s.title = char2wstr(title);
	} else {
		char str[128];
		sprintf_s(str, 128, "Series %u", series);
		s.title = char2wstr(str);
	}

	// Copy x/y data, if any
	if (count > 0) {
		if (x) {
			s.x.assign(x, x+count);
		} else {
			s.x.resize(count);
			for (int i = 0; i < count; ++i)
				s.x[i] = (float)i;
		}
		s.y.assign(y, y+count);
	}

	// Set up either a custom line style, or generate a default.
	if (style && strstr(style, "*"))
		s.shape = cStar;
	else if (style && strstr(style, "."))
		s.shape = cPoint;
	else if (style && strstr(style, "+"))
		s.shape = cPlus;
	else if (style && strstr(style, "x"))
		s.shape = cCross;
	else
		s.shape = (Shape)(1+(mNextSeriesStyle / cNumSeriesColours) % (cNumSeriesShapes-1));

	// Set up either custom colour, or generate a default.
	memset(s.colour, 0, 3);
	if (style && strstr(style, "r"))
		s.colour[0] = 255;
	else if (style && strstr(style, "b"))
		s.colour[2] = 255;
	else if (style && strstr(style, "g"))
		s.colour[1] = 192;
	else
		memcpy(s.colour, sSeriesColours[mNextSeriesStyle], 3);

	
	// Line style
	if (style && strstr(style, "-"))
		s.line = cSolid;
	else if (style && strstr(style, ":"))
		s.line = cDotted;
	else 
		s.line = cNoLine;

	mNextSeriesStyle++;
	return series;
}

void PlotCtl::addSeriesPoint(int seriesID, float x, float y)
{
	std::map<int,Series>::iterator si = mSeries.find(seriesID);
	if (si == mSeries.end())
		throw std::runtime_error("Invalid series ID passed to addSeriesPoint.");
	Series& s = si->second;
	s.x.push_back(x);
	s.y.push_back(y);
	setDirty();
}

int  PlotCtl::seriesCount() const { return (int)mSeries.size(); }

void PlotCtl::getWndStyle(DWORD& dwStyle, DWORD& dwExStyle) const
{
	Super::getWndStyle(dwStyle, dwExStyle);
#ifdef WINDOW_ALPHA
	dwExStyle |= WS_EX_LAYERED;
#endif
}

bool PlotCtl::onPaint()
{
	if (mDirty)
		redrawPlot();
	return Super::onPaint();
}

void PlotCtl::redrawPlot()
{
	mDirty = false;
	int sizeX = clientSizeX(), sizeY = clientSizeY();
	setAxisOrigin(0, sizeY-1);
	setAxisFlipY(true);

	// Compute scale of data when plotting
	int plotMinX = 0, plotMaxX = sizeX-1;
	int plotMinY = 0, plotMaxY = sizeY-1;
	float minX = mAxisMinX, minY = mAxisMinY;
	float maxX = mAxisMaxX, maxY = mAxisMaxY;
	if (mAutoAxis) {
		minX = minY =  1e10;
		maxX = maxY = -1e10;
		// Calculate min/max data points
		for (std::map<int,Series>::const_iterator si = mSeries.begin(); si != mSeries.end(); ++si) {
			const Series& s = si->second;
			if (s.x.empty())
				continue;
			minX = std::min(minX, *std::min_element(s.x.begin(), s.x.end()));
			maxX = std::max(maxX, *std::max_element(s.x.begin(), s.x.end()));
			minY = std::min(minY, *std::min_element(s.y.begin(), s.y.end()));
			maxY = std::max(maxY, *std::max_element(s.y.begin(), s.y.end()));
		}
		if (minX > maxX) { // if all series were empty, default to [0,1] square
			minX = minY = 0;
			maxX = maxY = 1;
		} else if (minX == maxX) { // if all data points were the same, plot around them
			maxX += 1;
			maxY += 1;
		}
	}

	POINT ptitle = measureString(mTitle.c_str());

	// Clear drawing area
	setColour(255, 255, 255);
	fillRect(0, 0, sizeX, sizeY);

	// Frame and axis tickmarks
	TCHAR minXstr[32], maxXstr[32], minYstr[32], maxYstr[32];
	POINT pminX, pmaxX, pminY, pmaxY;
	if (mDrawAxisLabels) {
		formatAxisLabel(minXstr, 32, minX);    // Get string for each axis label.
		formatAxisLabel(maxXstr, 32, maxX);     
		formatAxisLabel(minYstr, 32, minY);    
		formatAxisLabel(maxYstr, 32, maxY);    
		pminX = measureString(minXstr);         // Measure the size that each label
		pmaxX = measureString(maxXstr);         // will take when we draw them.
		pminY = measureString(minYstr);
		pmaxY = measureString(maxYstr);
		plotMinX += std::max(pminX.x/2,std::max(pminY.x, pmaxY.x))+6;     // Add enough margin to fit axis labes
		plotMaxX -= pmaxX.x/2+2;                      // outside theplot area.
		plotMinY += pminX.y+2;
		plotMaxY -= ptitle.y+8;
	}

	POINT plabelX, plabelY;
	if (!mAxisLabelY.empty()) {
		plabelY = measureString(mAxisLabelY.c_str(), true);
		plotMinX = std::max(plotMinX, (int)plabelY.x+6);
	}
	if (!mAxisLabelX.empty()) {
		plabelX = measureString(mAxisLabelX.c_str());
		plotMinY = std::max(plotMinY, (int)plabelX.y+6);
	}

	int plotSizeX = plotMaxX-plotMinX; // finished computing position of 
	int plotSizeY = plotMaxY-plotMinY; // plot area, so start drawing

	if (mDrawAxisLabels) {
		setColour(192, 192, 192);
		drawRect(plotMinX, plotMinY, plotMaxX+1, plotMaxY+1);
		setColour(0, 0, 0);
		drawString(plotMinX, plotMinY-pminX.y/2-2, minXstr, true);
		drawString(plotMaxX, plotMinY-pmaxX.y/2-2, maxXstr, true);
		drawString(plotMinX-pminY.x/2-4, plotMinY+1, minYstr, true);
		drawString(plotMinX-pmaxY.x/2-4, plotMaxY+1, maxYstr, true);
	}

	if (!mAxisLabelY.empty())
		drawString(plotMinX-plabelY.x/2-4, plotMinY+plotSizeY/2, mAxisLabelY.c_str(), true, true);
	if (!mAxisLabelX.empty()) 
		drawString(plotMinX+plotSizeX/2, plotMinY-plabelX.y/2-4, mAxisLabelX.c_str(), true);

	// Origin axes
	setColour(0, 0, 0);
	if (minY <= 0 && maxY >= 0) {   // draw X axis if it's in view
		int y0 = plotMinY + (int)(-minY / (maxY-minY) * plotSizeY);
		drawLine(plotMinX, y0, plotMaxX+1, y0);
	}
	if (minX <= 0 && maxX >= 0) {   // draw Y axis if it's in view
		int x0 = plotMinX + (int)(-minX / (maxX-minX) * plotSizeX);
		drawLine(x0, plotMinY, x0, plotMaxY+1);
	}

	// Title
	setColour(0, 0, 0);
	drawString(plotMinX + plotSizeX/2, plotMaxY+ptitle.y/2+4, mTitle.c_str(), true);

	// Series points
	for (std::map<int,Series>::const_iterator si = mSeries.begin(); si != mSeries.end(); ++si) {
		const Series& s = si->second;
		int count = (int)s.x.size();
		if (count == 0)
			continue;

		// Convert from data coordinates to plot coordinates
		const float* xs = &s.x[0];
		const float* ys = &s.y[0];
		POINT* pts = new POINT[count];
		for (int i = 0; i < count; ++i) {
			pts[i].x = plotMinX + (int)((xs[i]-minX) / (maxX-minX) * plotSizeX + .4999f);
			pts[i].y = plotMinY + (int)((ys[i]-minY) / (maxY-minY) * plotSizeY + .4999f);
		}

		setColour(s.colour[0], s.colour[1], s.colour[2]);

		// Draw connected lines if style is set.
		if (s.line != cNoLine) { 
			drawPolyline((int*)pts, count);
		}

		// Draw shapes at each point in the series.
		if (s.shape == cNoShape)
			continue;
		for (int i = 0; i < count; ++i) {
			int x = pts[i].x;
			int y = pts[i].y;
			switch (s.shape) {
				case cPoint: { fillRect(x, y, x+1, y+1); break; }
				case cPlus:  { drawLine(x-3, y, x+4, y);     drawLine(x, y-3, x, y+4); break; }
				case cCross: { drawLine(x-3, y-3, x+4, y+4); drawLine(x-3, y+3, x+4, y-4); break; }
				case cStar: { drawLine(x-3, y, x+4, y);     drawLine(x, y-3, x, y+4);
				              drawLine(x-2, y-2, x+3, y+3); drawLine(x-2, y+2, x+3, y-3); break; }
			}
		}
		delete [] pts;
	}

	// Legend
	if (mDrawLegend && !mSeries.empty()) {
		int legendX = plotMinX+std::max(plotSizeX/10, 5),  legendY = plotMaxY-10;
		int legendSizeX = 0, legendSizeY = 0;

		int itemHeight = 0;
		// First calculate size of legend based on series titles.
		for (std::map<int,Series>::const_iterator si = mSeries.begin(); si != mSeries.end(); ++si) {
			const Series& s = si->second;
			POINT p = measureString(s.title.c_str());
			itemHeight = p.y;
			if (legendSizeX < p.x)
				legendSizeX = p.x;
			legendSizeY += p.y;
		}

		legendSizeX += 25;
		legendSizeY += 6;

		// Draw frame around legend
		setColour(255, 255, 255);
		fillRect(legendX, legendY-legendSizeY, legendX+legendSizeX, legendY);
		setColour(192, 192, 192);
		drawRect(legendX, legendY-legendSizeY, legendX+legendSizeX, legendY);

		// Draw each series title into the legend area
		int itemY = legendY-3;
		for (std::map<int,Series>::const_iterator si = mSeries.begin(); si != mSeries.end(); ++si) {
			const Series& s = si->second;
			setColour(s.colour[0], s.colour[1], s.colour[2]);

			// Draw title text
			drawString(legendX+22, itemY, s.title.c_str());

			// Draw representation of the data shapes/lines, if any
			int x = legendX+11, y = itemY-itemHeight/2-1;
			if (s.line != cNoLine)
				drawLine(x-8, y, x+9, y);

			// Draw shapes at each point in the series.
			switch (s.shape) {
				case cPoint: { fillRect(x, y, x+1, y+1); break; }
				case cPlus:  { drawLine(x-3, y, x+4, y);     drawLine(x, y-3, x, y+4); break; }
				case cCross: { drawLine(x-3, y-3, x+4, y+4); drawLine(x-3, y+3, x+4, y-4); break; }
				case cStar:  { drawLine(x-3, y, x+4, y);     drawLine(x, y-3, x, y+4);
				               drawLine(x-2, y-2, x+3, y+3); drawLine(x-2, y+2, x+3, y-3); break; }
			}

			itemY -= itemHeight;
		}
	}
}

void PlotCtl::setDirty()
{
	mDirty = true;
	InvalidateRect(handle(), NULL, FALSE);
}

void PlotCtl::formatAxisLabel(TCHAR *dst, int size, float value)
{
	int ival = (int)(value + .000000001f);
	if (value > 4096 && (ival & (~ival+1)) == ival) { // if ival is a power of two
		int e = -1;
		while (ival) {  // count the trailing zero bits 
			ival >>=  1;
			++e; 
		}
		sprintf_TEXT(dst, size, TEXT("2^%u"), e); 
	} else {
		sprintf_TEXT(dst, size, TEXT("%g"), value);
	}
}

//////////////////////////////////////////////////////////////////////
// Simple 1037 API redirects all calls to a default global Window object

int gLINE;
const char* gFILE;

static unsigned gMainThreadID = 0;
#define AssertIsMainThread() AssertMsg(gMainThreadID == 0 || _threadid == gMainThreadID, "Only the main() thread can call this function.")

static std::map<int, BitmapInfo>& sBitmapRegistry() {
	AssertIsMainThread();
	static std::map<int, BitmapInfo> registry;
	return registry;
}
static int gNextBitmapID = 1;

// These are global functions with static data so that students can initialize 
// global variables with calls to CreateButton etc:
//    int checkBox = CreateCheckBox("text", handler);
// and not run into any global data initialization-order problems 
// (std::map constructor for example).

#define DECL_REGISTRY(name,type)                 \
	static std::map<int, type*>& name() {        \
		AssertIsMainThread();                    \
		static std::map<int, type*> registry;    \
		return registry;                         \
	}

DECL_REGISTRY(sTextLabelRegistry, StaticCtl)
DECL_REGISTRY(sButtonRegistry,    ButtonCtl)
DECL_REGISTRY(sDropListRegistry,  ComboBoxCtl)
DECL_REGISTRY(sCheckBoxRegistry,  CheckBoxCtl)
DECL_REGISTRY(sTextBoxRegistry,   EditCtl)
DECL_REGISTRY(sSliderRegistry,    TrackbarCtl)
DECL_REGISTRY(sPlotRegistry,      PlotCtl)
DECL_REGISTRY(sPlotSeriesRegistry,PlotCtl)

static int gNextCtrlID = 1;

static std::list<MSG> gKeyboardInputQueue;
static std::list<MSG> gMouseInputQueue;

static SimpleCanvasApp* theMainWindow()
{
	AssertIsMainThread();
	// NOT thread safe, obviously
	static SimpleCanvasApp* sMainWindow = 0;
	if (!sMainWindow) {
		INITCOMMONCONTROLSEX initcc;
		initcc.dwSize = sizeof(initcc);
		initcc.dwICC = ICC_STANDARD_CLASSES;
		InitCommonControlsEx(&initcc);
		sMainWindow = new SimpleCanvasApp("main");
		sMainWindow->setPositionAndSize(200, 100, 640, 480);
		sMainWindow->setBitmapSize(2048, 2048);
		sMainWindow->setColour(255,255,255);
		sMainWindow->fillRect(0, 0, 1280, 1024);
		sMainWindow->setColour(0,0,0);
	}
	return sMainWindow;
}

static void sShowDialog_(const char* title, const char* format, va_list args)
{
	int textLen = _vscprintf(format, args);
	char* message = new char[textLen+1];
	vsprintf_s(message, textLen+1, format, args);
	theMainWindow()->setVisible(true);
	MessageBoxA(theMainWindow()->handle(), message, title, MB_OK);
	delete [] message;
}

static void sShowDialog(const char* title, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	sShowDialog_(title, format, args);
	va_end(args);
}

static void sAssertDialog(const char* msg, const char* file, int line)
{
#if _MSC_VER < 1400 // VC++ 2003 or below
	gFILE = file;
	gLINE = line; 
#else
	file; line; // avoid unreferenced formal param warning
#endif
	const char* nameptr = strrchr(gFILE, '\\');
	if (!nameptr) nameptr = strrchr(gFILE, '/');
	if (!nameptr) nameptr = gFILE;
	sShowDialog("ASSERT failed!", "%s line %d...\n\n%s", nameptr+1, gLINE, msg);
}

static Window* sGetControl(int handle, const char* callerName)
{
	AssertIsMainThread();
	Window* ctl = NULL;
	if (sTextLabelRegistry().find(handle) != sTextLabelRegistry().end())
		ctl = sTextLabelRegistry()[handle];
	else if (sButtonRegistry().find(handle) != sButtonRegistry().end())
		ctl = sButtonRegistry()[handle];
	else if (sDropListRegistry().find(handle) != sDropListRegistry().end())
		ctl = sDropListRegistry()[handle];
	else if (sCheckBoxRegistry().find(handle) != sCheckBoxRegistry().end())
		ctl = sCheckBoxRegistry()[handle];
	else if (sTextBoxRegistry().find(handle) != sTextBoxRegistry().end())
		ctl = sTextBoxRegistry()[handle];
	else if (sSliderRegistry().find(handle) != sSliderRegistry().end())
		ctl = sSliderRegistry()[handle];
	else if (sPlotRegistry().find(handle) != sPlotRegistry().end())
		ctl = sPlotRegistry()[handle];
	if (!ctl) {
		char msg[512];
		sprintf_s(msg, 512, "You called %s with an invalid control handle.", callerName);
		AssertUnreachable(msg);
	}
	return ctl;
}

inline static BitmapInfo* sGetBitmapInfo(int handle, const char* callerName)
{
	AssertIsMainThread();
	// Do a bit of caching so that subsequent calls on the same
	// image will avoid lookups in sBitmapRegistry().
	static BitmapInfo* bmp = 0;
	static int bmpHandle = 0;
	if (!bmp || handle != bmpHandle) {
		if (sBitmapRegistry().find(handle) != sBitmapRegistry().end()) {
			bmp = &sBitmapRegistry()[handle];
			bmpHandle = handle;
		} else {
			bmp = 0;
			bmpHandle = 0;
		}
	}
	if (!bmp) {
		char msg[512];
		sprintf_s(msg, 512, "You called %s with an invalid image handle.", callerName);
		AssertUnreachable(msg);
	}
	return bmp;
}

static void sPumpMessageQueue()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			gQuit = true;

		if (!IsDialogMessage(theMainWindow()->handle(), &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
			gKeyboardInputQueue.push_back(msg);
		else if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
			gMouseInputQueue.push_back(msg);
	}
}

static bool sWantKeyboardEvent(MSG& msg, char c)
{
	// Do not report non-ASCII keys, and do not report tabs
	if (!c || c == '\t') 
		return false;

	// Report any key event received directly by main window 
	Window* w = Window::sWindowFromHandle(msg.hwnd);
	if (w == static_cast<Window*>(theMainWindow()))
		return true;

	// Only report events for children of the main window
	if (!IsChild(theMainWindow()->handle(), msg.hwnd))
		return false;

	// ButtonCtls and CheckBoxCtls eat the spacebar
	if (dynamic_cast<ButtonCtl*>(w) || dynamic_cast<CheckBoxCtl*>(w))
		return c != ' ';

	// Text controls eat everything
	if (dynamic_cast<EditCtl*>(w))
		return false;

	return true;
}

bool GetKeyboardInput(char* key)
{
	AssertIsMainThread();
	sPumpMessageQueue();
	while (!gKeyboardInputQueue.empty()) {
		MSG msg = gKeyboardInputQueue.front(); gKeyboardInputQueue.pop_front();
		if (msg.message == WM_KEYDOWN) {
			char c = (char)MapVirtualKey((int)msg.wParam, 2); // 2 = MAPVK_VK_TO_CHAR
			c = (char)tolower(c);
			if (sWantKeyboardEvent(msg, c)) {
				if (key)
					*key = c;
				return true;
			}
		}
	}
	if (_kbhit()) { // don't let students get confused, poll both window and console
		if (key)
			*key = (char)_getch();
		return true;
	}
	Sleep(1); // hack to avoid saturating CPU
	return false;
}

bool GetMouseInput(int* mouseX, int* mouseY, int* mouseButton)
{
	AssertIsMainThread();
	int ydir = theMainWindow()->axisFlipY() ? -1 : 1;
	sPumpMessageQueue();
	while (!gMouseInputQueue.empty()) {
		MSG msg = gMouseInputQueue.front(); gMouseInputQueue.pop_front();
		if (Window::sWindowFromHandle(msg.hwnd) != static_cast<Window*>(theMainWindow()))
			continue;
		int x =  GET_X_LPARAM(msg.lParam) - theMainWindow()->axisOriginX();
		int y = (GET_Y_LPARAM(msg.lParam) - theMainWindow()->axisOriginY())*ydir;
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
	if (_threadid == gMainThreadID)
		sPumpMessageQueue();  // give the window a chance to redraw before sleep.
	Sleep(milliseconds);
	if (_threadid == gMainThreadID)
		sPumpMessageQueue();
}

bool WasWindowClosed()
{
	Sleep(1); // hack to avoid saturating CPU
	sPumpMessageQueue();
	return gQuit;
}

void SetWindowVisible(bool visible)
{
	theMainWindow()->setVisible(visible);
	sPumpMessageQueue();
}

void SetWindowTitle(const char* title)
{
	theMainWindow()->setText(char2wstr(title).c_str());
	sPumpMessageQueue();
}

void SetWindowPosition(int x, int y)
{
	theMainWindow()->setPosition(x, y);
	sPumpMessageQueue();
}

void SetWindowSize(int width, int height)
{
	theMainWindow()->setSize(width, height);
	sPumpMessageQueue();
}

void DrawString(int x0, int y0, const char* str)
{
	theMainWindow()->drawString(x0, y0, char2wstr(str).c_str());
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

void DrawLine(int x0, int y0, int x1, int y1)
{
	theMainWindow()->drawLine(x0, y0, x1, y1);
}

void DrawPolyline(int* x, int* y, int count)
{
	theMainWindow()->drawPolyline(x, y, count);
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

void SetDrawAxis(int originX, int originY, bool flipY)
{
	theMainWindow()->setAxisOrigin(originX, originY);
	theMainWindow()->setAxisFlipY(flipY);
}

void DrawImage(int handle, int x0, int y0)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle, "DrawImage");
	theMainWindow()->drawBitmap(x0, y0, *bmp);
}

void DrawImageStretched(int handle, int x0, int y0, int x1, int y1)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle, "DrawImageStretched");
	theMainWindow()->stretchBitmap(x0, y0, x1, y1, *bmp);
}

int CreateImage(int sizeX, int sizeY, const char* options)
{
	AssertIsMainThread();
	int bpp = strstr(options, "bgra") ? 4 : 
	          strstr(options, "bgr")  ? 3 : 
	          strstr(options, "grey") ? 1 : 
	          strstr(options, "gray") ? 1 : 0;

	if (!bpp)
		return 0; // invalid options

	// Set BITMAPINFOHEADER fields
	struct {
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD pal[256];
	} bmi;
	memset(&bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = sizeX;
	bmi.bmiHeader.biHeight = -sizeY;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = (WORD)(bpp*8);
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biClrUsed = (bpp == 1) ? 256 : 0;
	bmi.bmiHeader.biClrImportant = bmi.bmiHeader.biClrUsed;
	for (int i = 0; i < 256; ++i) { // grayscale palette
		bmi.pal[i].rgbBlue = bmi.pal[i].rgbGreen = bmi.pal[i].rgbRed = (unsigned char)i;
		bmi.pal[i].rgbReserved = 255;
	}

	// Tell windows to allocate a DIB from the parameters we've set up
	BitmapInfo info;
	info.osHandle = CreateDIBSection(theMainWindow()->offScreenDC(), (BITMAPINFO*)&bmi, DIB_RGB_COLORS, 0, 0, 0); 
	
	AssertMsg(info.osHandle, "Attempt to allocate bitmap failed. Probably ran out of OS handles.");
	GetObject(info.osHandle, sizeof(DIBSECTION), &info.dib);

	int handle = gNextBitmapID++;
	sBitmapRegistry()[handle] = info;
	return handle;
}

int CreateImageFromFile(const char* fileName)
{
	AssertIsMainThread();
	// Ask Windows to do the dirty work
	HANDLE wh = LoadImage(0, char2wstr(fileName).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (!wh)
		return 0;

	// Ask for the bitmap details and remember them in the registry for easy access
	BitmapInfo info;
	info.osHandle = wh;	
	if (GetObject(wh, sizeof(info.dib), &info.dib) != sizeof(info.dib)) {
		DeleteObject(wh);
		return 0;
	}

	int handle = 0;
	if (info.dib.dsBm.bmBitsPixel <= 8) {
		// For CreateImage and DrawImage functions, 8-bit images treated as grayscale. 
		// Any palettized image must therefore either be grayscale to begin with, OR 
		// be converted to BGR internally at load time.
		RGBQUAD palette[256];
		HDC tempDC = CreateCompatibleDC(0);
		HBITMAP oldBMP = (HBITMAP)SelectObject(tempDC, wh);
		GetDIBColorTable(tempDC, 0, info.dib.dsBmih.biClrUsed, palette);
		int bpp = 1;
		for (unsigned i = 0; i < info.dib.dsBmih.biClrUsed; ++i) {
			if (palette[i].rgbBlue != palette[i].rgbGreen || palette[i].rgbBlue != palette[i].rgbRed) {
				bpp = 3; // palette is not greyscale
				break;
			}
		}
		SelectObject(tempDC, oldBMP);
		DeleteDC(tempDC);

		handle = CreateImage(info.dib.dsBm.bmWidth, info.dib.dsBm.bmHeight, bpp == 1 ? "gray" : "bgr");
		for (int y = 0; y < info.dib.dsBm.bmHeight; ++y) {
			unsigned char* src = (unsigned char*)info.dib.dsBm.bmBits + info.dib.dsBm.bmWidthBytes*y;
			unsigned char* dst = GetImagePixelPtr(handle, 0, info.dib.dsBmih.biHeight < 0 ? y : info.dib.dsBmih.biHeight-y-1); // potentially flip row order
			for (int x = 0; x < info.dib.dsBm.bmWidth; ++x) {
				unsigned char srcByte = src[x*info.dib.dsBmih.biBitCount/8];
				unsigned pindex = (srcByte >> (8-info.dib.dsBmih.biBitCount-(info.dib.dsBmih.biBitCount*x)%8)) & ((1 << info.dib.dsBmih.biBitCount)-1);
				memcpy(dst+x*bpp, palette+pindex, bpp);
			}
		}

	} else {
		// The image is not palettized, so load it directly but flip row order if necessary
		handle = CreateImage(info.dib.dsBm.bmWidth, info.dib.dsBm.bmHeight, info.dib.dsBm.bmBitsPixel == 24 ? "bgr" : "bgra");
		for (int y = 0; y < info.dib.dsBm.bmHeight; ++y) {
			unsigned char* src = (unsigned char*)info.dib.dsBm.bmBits + info.dib.dsBm.bmWidthBytes*y;
			unsigned char* dst = GetImagePixelPtr(handle, 0, info.dib.dsBmih.biHeight < 0 ? y : info.dib.dsBmih.biHeight-y-1); // potentially flip row order
			memcpy(dst, src, info.dib.dsBm.bmWidthBytes);
		}
	}

	DeleteObject(wh);
	return handle;
}

void WriteImageToFile(int handle, const char* fileName)
{
	AssertIsMainThread();
	AssertMsg(strstr(fileName, ".bmp") == fileName+strlen(fileName)-4 
		   || strstr(fileName, ".BMP") == fileName+strlen(fileName)-4,
		   "Only BMP images are supported; file name must end in \".bmp\"");

	BitmapInfo* bmp = sGetBitmapInfo(handle, "WriteImageToFile");

	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 'B' | ('M' << 8);
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (bmp->dib.dsBmih.biBitCount == 8 ? 256*sizeof(RGBQUAD) : 0);
	bmfh.bfSize = bmfh.bfOffBits + bmp->dib.dsBmih.biSizeImage;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

	FILE* fh;
	fopen_s(&fh, fileName, "wb");
	AssertMsg(fh, "Could not open file for writing");
	fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, fh);
	bmp->dib.dsBmih.biHeight = -bmp->dib.dsBmih.biHeight; // Standard for on-disk BMP is to have bottom-first row order
	fwrite(&bmp->dib.dsBmih, sizeof(BITMAPINFOHEADER), 1, fh);
	bmp->dib.dsBmih.biHeight = -bmp->dib.dsBmih.biHeight;
	if (bmp->dib.dsBmih.biBitCount == 8) {
		RGBQUAD palette[256];
		for (int i = 0; i < 256; ++i) { // grayscale palette
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (unsigned char)i;
			palette[i].rgbReserved = 255;
		}
		fwrite(palette, 256*sizeof(RGBQUAD), 1, fh);
	}
	fwrite(bmp->dib.dsBm.bmBits, bmp->dib.dsBmih.biSizeImage, 1, fh);
	fclose(fh);
}


int GetImageSizeX(int handle)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle, "GetImageSizeX");
	return bmp->dib.dsBm.bmWidth;
}

int GetImageSizeY(int handle)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle, "GetImageSizeY");
	return bmp->dib.dsBm.bmHeight;
}

int GetImageBytesPerPixel(int handle)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle, "GetImageBytesPerPixel");
	return bmp->dib.dsBm.bmBitsPixel >> 3;
}

unsigned char* GetImagePixelPtr(int handle, int x, int y)
{
	BitmapInfo* bmp = sGetBitmapInfo(handle, "GetImagePixelPtr");
	int bpp = bmp->dib.dsBm.bmBitsPixel>>3;
	AssertMsg(x >= 0 && y >= 0 && x < bmp->dib.dsBm.bmWidth && y < bmp->dib.dsBm.bmHeight,
	          "Requested pixel is outside image boundary");
	return (unsigned char*)bmp->dib.dsBm.bmBits 
			+ bmp->dib.dsBm.bmWidthBytes*y + x*bpp;
}

void DeleteImage(int handle)
{
	AssertMsg(sBitmapRegistry().find(handle) != sBitmapRegistry().end(),
			"You called DeleteImage with an invalid image handle.");
	BitmapInfo& info = sBitmapRegistry()[handle];
	BOOL deleted = DeleteObject(info.osHandle);
	AssertMsg(deleted, "Failed to delete image.");
	sBitmapRegistry().erase(handle); // redundant searching but who cares
}

// These "Ctrl" variables provide a reasonable default position for 
// controls in the order that they were created. The basic idea is to 
// place the controls in left-right order on a row, then wrap to the next 
// row once a control no longer fits in the *current* main window size.
// 
// Not intended to be robust. Makes typical apps work without any
// SetControlPosition nonsense.
//
static const int sCtrlSpacingX = 5, sCtrlSpacingY = 5;
static int sCreateCtrlX = sCtrlSpacingX, sCreateCtrlY = sCtrlSpacingY;
static int sDefaultRowCtrlSize = 23, sMaxRowCtrlSizeY = 23;
static int sPrevCreateCtrlX = sCreateCtrlX, sPrevCreateCtrlY = sCreateCtrlY;

static void sSetDefaultCtrlPos(Window* ctrl, bool paddingX=true)
{
	sPrevCreateCtrlX = sCreateCtrlX;
	sPrevCreateCtrlY = sCreateCtrlY;
	int dx = ctrl->windowSizeX();
	int dy = ctrl->windowSizeY();
	if (dy > sMaxRowCtrlSizeY)
		sMaxRowCtrlSizeY = dy;
	if (sCreateCtrlX + dx >= theMainWindow()->clientSizeX()) {
		// No more space on current row. Wrap new control to next row.
		sCreateCtrlX = sCtrlSpacingX;
		sCreateCtrlY += sMaxRowCtrlSizeY + sCtrlSpacingY;
		sMaxRowCtrlSizeY = sDefaultRowCtrlSize;
	}
	ctrl->setPosition(sCreateCtrlX, sCreateCtrlY);
	sCreateCtrlX += dx;
	if (paddingX)
		sCreateCtrlX += sCtrlSpacingX;
}


int CreateTextLabel(const char* text)
{
	int handle = gNextCtrlID++;
	char name[64];
	sprintf_s(name, 64, "TextLabel%d", handle);
	StaticCtl* s = new StaticCtl(name);
	s->setParent(theMainWindow());
	s->setText(char2wstr(text).c_str());
	s->fitSizeToText();
	sSetDefaultCtrlPos(s, false);
	sTextLabelRegistry()[handle] = s;
	return handle;
}

void SetTextLabelString(int handle, const char* text)
{
	AssertMsg(sTextLabelRegistry().find(handle) != sTextLabelRegistry().end(),
			"You called SetTextLabelString with an invalid handle.");
	StaticCtl* s = sTextLabelRegistry()[handle];
	s->setText(char2wstr(text).c_str());
}

static void sButtonCallback(void* userData, Window* /*sender*/)
{
	if (userData)
		((functionptr)userData)();
}

int CreateButton(const char* text, functionptr onClick)
{
	int handle = gNextCtrlID++;
	char name[64];
	sprintf_s(name, 64, "Button%d", handle);
	ButtonCtl* b = new ButtonCtl(name);
	b->setParent(theMainWindow());
	b->setText(char2wstr(text).c_str());
	b->fitSizeToText();
	sSetDefaultCtrlPos(b);
	b->setOnClick(sButtonCallback, onClick);
	sButtonRegistry()[handle] = b;
	return handle;
}

static void sCheckBoxCallback(void* userData, Window* sender)
{
	bool state = static_cast<CheckBoxCtl*>(sender)->getState();
	if (userData)
		((functionptr_bool)userData)(state);
}

int CreateCheckBox(const char* text, bool checked, functionptr_bool onStateChanged)
{
	int handle = gNextCtrlID++;
	char name[64];
	sprintf_s(name, 64, "CheckBox%d", handle);
	CheckBoxCtl* cb = new CheckBoxCtl(name);
	cb->setParent(theMainWindow());
	cb->setText(char2wstr(text).c_str());
	cb->fitSizeToText();
	sSetDefaultCtrlPos(cb);
	cb->setState(checked);
	cb->setOnStateChanged(sCheckBoxCallback, onStateChanged);
	sCheckBoxRegistry()[handle] = cb;
	return handle;
}

bool GetCheckBoxState(int handle)
{
	AssertMsg(sCheckBoxRegistry().find(handle) != sCheckBoxRegistry().end(),
			"You called GetCheckBoxState with an invalid handle.");
	CheckBoxCtl* cb = sCheckBoxRegistry()[handle];
	return cb->getState();
}

void SetCheckBoxState(int handle, bool checked)
{
	AssertMsg(sCheckBoxRegistry().find(handle) != sCheckBoxRegistry().end(),
			"You called SetCheckBoxState with an invalid handle.");
	CheckBoxCtl* cb = sCheckBoxRegistry()[handle];
	cb->setState(checked);
}


static void sTextBoxCallback(void* userData, Window* sender)
{
	const char* text = static_cast<EditCtl*>(sender)->getString();
	if (userData)
		((functionptr_string)userData)(text);
}

int CreateTextBox(const char* text, functionptr_string onStringChanged)
{
	const int cDefaultSizeX = 75, cDefaultSizeY = sDefaultRowCtrlSize;
	int handle = gNextCtrlID++;
	char name[64];
	sprintf_s(name, 64, "TextBox%d", handle);
	EditCtl* tb = new EditCtl(name);
	tb->setParent(theMainWindow());
	tb->setString(text);
	tb->setSize(cDefaultSizeX, cDefaultSizeY);
	sSetDefaultCtrlPos(tb);
	tb->setOnStringChanged(sTextBoxCallback, onStringChanged);
	sTextBoxRegistry()[handle] = tb;
	return handle;
}

const char* GetTextBoxString(int handle)
{
	AssertMsg(sTextBoxRegistry().find(handle) != sTextBoxRegistry().end(),
			"You called GetTextBoxState with an invalid handle.");
	EditCtl* tb = sTextBoxRegistry()[handle];
	return tb->getString();
}

void SetTextBoxString(int handle, const char* text)
{
	AssertMsg(sTextBoxRegistry().find(handle) != sTextBoxRegistry().end(),
			"You called SetTextBoxState with an invalid handle.");
	EditCtl* tb = sTextBoxRegistry()[handle];
	tb->setString(text);
}


static void sDropListCallback(void* userData, Window* sender)
{
	int index = static_cast<ComboBoxCtl*>(sender)->getSelectedItem();
	if (userData)
		((functionptr_int)userData)(index);
}

int CreateDropList(int numItems, const char* const items[], int selected, functionptr_int onSelectionChanged)
{
	int handle = gNextCtrlID++;
	char name[64];
	sprintf_s(name, 64, "DropList%d", handle);
	ComboBoxCtl* cb = new ComboBoxCtl(name);
	cb->setParent(theMainWindow());
	for (int i = 0; i < numItems; ++i)
		cb->insertItem(i, items[i]);
	cb->setSelectedItem(selected);
	cb->setOnSelectionChanged(sDropListCallback, onSelectionChanged);
	cb->fitSizeToItems();
	sSetDefaultCtrlPos(cb);
	sDropListRegistry()[handle] = cb;
	return handle;
}

int GetDropListSelection(int handle)
{
	AssertMsg(sDropListRegistry().find(handle) != sDropListRegistry().end(),
			"You called GetDropListSelection with an invalid handle.");
	ComboBoxCtl* cb = sDropListRegistry()[handle];
	return cb->getSelectedItem();
}

void SetDropListSelection(int handle, int selected)
{
	AssertMsg(sDropListRegistry().find(handle) != sDropListRegistry().end(),
			"You called SetDropListSelection with an invalid handle.");
	ComboBoxCtl* cb = sDropListRegistry()[handle];
	AssertMsg(selected >= 0 && selected < cb->itemCount(),
			"You called SetDropListSelection with an invalid index.");
	cb->setSelectedItem(selected);
}


static void sSliderCallback(void* userData, Window* sender)
{
	float value = static_cast<TrackbarCtl*>(sender)->getValue();
	if (userData)
		((functionptr_float)userData)(value);
}

int CreateSlider(float value, functionptr_float onValueChanged, bool isHorizontal)
{
	const int cDefaultSizeX = 100, cDefaultSizeY = sDefaultRowCtrlSize;
	int handle = gNextCtrlID++;
	char name[64];
	sprintf_s(name, 64, "Slider%d", handle);
	TrackbarCtl* tb = new TrackbarCtl(name, isHorizontal);
	tb->setParent(theMainWindow());
	if (isHorizontal)
		tb->setSize(cDefaultSizeX, cDefaultSizeY);
	else
		tb->setSize(cDefaultSizeY, cDefaultSizeX);
	tb->setValue(value);
	tb->setOnValueChanged(sSliderCallback, onValueChanged);
	sSetDefaultCtrlPos(tb);
	sSliderRegistry()[handle] = tb;
	return handle;
}

float GetSliderValue(int handle)
{
	AssertMsg(sSliderRegistry().find(handle) != sSliderRegistry().end(),
	      "You called GetSliderValue with an invalid handle.");
	TrackbarCtl* tb = sSliderRegistry()[handle];
	return tb->getValue();
}

void SetSliderValue(int handle, float value)
{
	AssertMsg(sSliderRegistry().find(handle) != sSliderRegistry().end(),
	      "You called SetSliderValue with an invalid handle.");
	TrackbarCtl* tb = sSliderRegistry()[handle];
	tb->setValue(value);
}

void SetControlPosition(int handle, int x, int y)
{
	if (sPrevCreateCtrlX != sCreateCtrlX || sPrevCreateCtrlY != sCreateCtrlY) {
		sMaxRowCtrlSizeY = sDefaultRowCtrlSize;
		sCreateCtrlX = sPrevCreateCtrlX; // HEURISTIC: if the student created a control, then immediately moved it,
		sCreateCtrlY = sPrevCreateCtrlY; // assume that the next control can go in the blank spot left for this one.
	}
	sGetControl(handle,"SetControlPosition")->setPosition(x, y);
}

void SetControlSize(int handle, int sizeX, int sizeY)
{
	Window* ctrl = sGetControl(handle,"SetControlSize");
	if (sPrevCreateCtrlX != sCreateCtrlX || sPrevCreateCtrlY != sCreateCtrlY) {
		sCreateCtrlX += sizeX - ctrl->windowSizeX(); // HEURISTIC: if the student creates a control, then immediately resizes it,
		sCreateCtrlY += sizeY - ctrl->windowSizeY(); // then make sure the next control is pushed over when it gets created.
		if (sizeY > sMaxRowCtrlSizeY)
			sMaxRowCtrlSizeY = sizeY;
	}
	ctrl->setSize(sizeX, sizeY);
}

void GetControlPosition(int handle, int* x, int* y)
{
	Window* ctrl = sGetControl(handle,"GetControlPosition");
	*x = ctrl->relativeX();
	*y = ctrl->relativeY();
}

void GetControlSize(int handle, int* sizeX, int* sizeY)
{
	Window* ctrl = sGetControl(handle,"GetControlSize");
	*sizeX = ctrl->windowSizeX();
	*sizeY = ctrl->windowSizeY();
}

void DeleteControl(int handle)
{
	if (sTextLabelRegistry().find(handle) != sTextLabelRegistry().end()) {
		sTextLabelRegistry()[handle]->destroy();
		sTextLabelRegistry().erase(handle);
	} else if (sButtonRegistry().find(handle) != sButtonRegistry().end()) {
		sButtonRegistry()[handle]->destroy();
		sButtonRegistry().erase(handle);
	} else if (sDropListRegistry().find(handle) != sDropListRegistry().end()) {
		sDropListRegistry()[handle]->destroy();
		sDropListRegistry().erase(handle);
	} else if (sCheckBoxRegistry().find(handle) != sCheckBoxRegistry().end()) {
		sCheckBoxRegistry()[handle]->destroy();
		sCheckBoxRegistry().erase(handle);
	} else if (sTextBoxRegistry().find(handle) != sTextBoxRegistry().end()) {
		sTextBoxRegistry()[handle]->destroy();
		sTextBoxRegistry().erase(handle);
	} else if (sSliderRegistry().find(handle) != sSliderRegistry().end()) {
		sSliderRegistry()[handle]->destroy();
		sSliderRegistry().erase(handle);
	} else if (sPlotRegistry().find(handle) != sPlotRegistry().end()) {
		sPlotRegistry()[handle]->destroy();
		sPlotRegistry().erase(handle);
	} else {
		AssertUnreachable("You called DeleteControl with an invalid handle. (control was already deleted?)");
	}
}

static void sWindowResizeCallback(void* userData, Window* sender, int, int)
{
	((functionptr_intint)userData)(sender->clientSizeX(), sender->clientSizeY());
}

void SetWindowResizeCallback(functionptr_intint onWindowResize)
{
	theMainWindow()->setOnSize(sWindowResizeCallback, onWindowResize);
}

struct QPCInitializer {
	QPCInitializer() 
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&ticks0);
	}
	LARGE_INTEGER ticks0;
	LARGE_INTEGER frequency;
};

static QPCInitializer sQPC; // constructor is called before main(), to record the tick counter at program startup.

int GetMilliseconds()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (int)(1000*(now.QuadPart-sQPC.ticks0.QuadPart) / sQPC.frequency.QuadPart);
}

int GetMicroseconds()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (int)(1000000i64*(now.QuadPart-sQPC.ticks0.QuadPart) / sQPC.frequency.QuadPart);
}

static bool sRandInitialized = false;

int GetRandomInt()
{
	if (!sRandInitialized) {
		srand((unsigned)time(0));
		sRandInitialized = true;
	}
	// randomize all 31 bits to make an unsigned random integer
	return ((rand() & 0x7fff) | (rand() << 16)) & ~(1 << 31);
}

float GetRandomFloat()
{
	return (float)GetRandomInt() / (float)~(1 << 31);
}

////////////////////////////////////////////////////

static void sThreadStart(void* arg)
{
	((functionptr)arg)();
}

void StartParallelThread(functionptr func)
{
	_beginthread(sThreadStart, 0, func);
}

void StartParallelThreadWithArg(functionptr_voidptr func, void* arg)
{
	_beginthread(func, 0, arg);
}

static const int cMaxCriticalSections = 100;
static CRITICAL_SECTION sCriticalSections[cMaxCriticalSections];

// Initialize critical sections before main() gets a chance to start
struct InitCriticalSections {
	InitCriticalSections() {
		for (int i = 0; i < cMaxCriticalSections; ++i)
			InitializeCriticalSectionAndSpinCount(&sCriticalSections[i], 50);
		gMainThreadID = _threadid;
	}
};
InitCriticalSections gInitCriticalSections;

void BeginCriticalSection(int id) {
	AssertMsg(id >= 0 && id < cMaxCriticalSections, "Invalid critical section id; valid ids are 0..99");
	EnterCriticalSection(&sCriticalSections[id]);
}

void EndCriticalSection(int id) {
	AssertMsg(id >= 0 && id < cMaxCriticalSections, "Invalid critical section id; valid ids are 0..99");
	AssertMsg(sCriticalSections[id].LockCount >= 0, "EndCriticalSection was called outside of a critical section");
	LeaveCriticalSection(&sCriticalSections[id]);
}


////////////////////////////////////////////////////


static PlotCtl* sActivePlot = 0;   // These variables introduce the convenient notion of a "default," or "active" plot
static int sActivePlotID = 0;      // much like how Matlab has a current figure/axes to which commands apply by default.
static int sActivePlotSeries = 0;

int CreatePlot(const char* title, const char* axisLabelX, const char* axisLabelY)
{
	int handle = gNextCtrlID++;
	char name[64];
	sprintf_s(name, 64, "Plot%d", handle);
	PlotCtl* p = new PlotCtl(name);
	p->setParent(theMainWindow());
	p->setPositionAndSize(0, 0, 
	                      theMainWindow()->clientSizeX(), 
	                      theMainWindow()->clientSizeY());
	if (title)
		p->setTitle(title);
	if (axisLabelX || axisLabelY)
		p->setAxisLabels(axisLabelX, axisLabelY);
	sPlotRegistry()[handle] = p;
	sActivePlot = p;
	sActivePlotID = handle;
	return handle;
}

int CreatePlotSeries(const char* title, const char* style, int plot)
{
	PlotCtl* p = 0;
	if (!plot) {
		if (!sActivePlot)
			CreatePlot(0); // create a default plot if no active plot yet
		p = sActivePlot;
	} else {
		AssertMsg(sPlotRegistry().find(plot) != sPlotRegistry().end(),
			"You called CreatePlotSeries with an invalid plot handle.");
		p = sPlotRegistry()[plot];
	}
	int series = p->addSeries(0, 0, 0, title, style);
	sPlotSeriesRegistry()[series] = p; // remember which plot owns 'series'
	if (p == sActivePlot)
		sActivePlotSeries = series; // make the new series the 'default' series
	return series;
}

void AddSeriesPoint(float x, float y, int series)
{
	PlotCtl* p = 0;
	if (!series) {
		if (!sActivePlotSeries)
			CreatePlotSeries(0, 0, sActivePlotID); // create default series if no active series yet
		p = sActivePlot;
		series = sActivePlotSeries;
	} else {
		AssertMsg(sPlotSeriesRegistry().find(series) != sPlotSeriesRegistry().end(),
			"You called AddSeriesPoint with an invalid series handle.");
		p = sPlotSeriesRegistry()[series];
	}
	p->addSeriesPoint(series, x, y);
}

void AddSeriesPoint(int x, int y, int series)
{
	AddSeriesPoint((float)x, (float)y, series);
}

