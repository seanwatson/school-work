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
#ifndef __CS037UTIL_H__
#define __CS037UTIL_H__

///////////////////////////////////////////////////////////////////////////
// Simple API for drawing in a window and polling for key/mouse input.
//

// ShowWindow: 
//    Call this first to create the window that you can draw into
//
void ShowWindow(const char* title);

// HideWindow: 
//    Self explanatory!
//
void HideWindow();

// WasWindowClosed:
//    Returns true if the main window was closed, via the little X button or otherwise.
//
bool WasWindowClosed();

// GetKeyboardInput:
//    Returns true if a key has been pressed since the last time this was called.
//    After returning true, 'key' will contain the (lower-case) ASCII character 
//    corresponding to the key that was pressed. The ENTER key returns '\r', by the way.
//    Keys that do not correspond to ASCII characters (pgup, arrows, etc) are ignored!
//
// Example:
//    char c;
//    if (GetKeyboardInput(&c))
//        cout << "Key '" << c << "' was pressed" << endl;
//
bool GetKeyboardInput(char* key);

// GetMouseInput:
//    Returns true if the mouse has moved or if a button has changed state (up/down).
//    After returning, mouseX and mouseY will contain the position of the mouse
//    relative to the top-left corner of the window. 
//    If a mouse button transitions from the up to down state, mouseButton will contain
//    the number of that button (1, 2, or 3 for left, right or middle). If the 
//    button transitions from down to up, then the number will be negative.
//
// Example:
//    int mx, my, mb;
//    if (GetMouseInput(&mx, &my, &mb))
//        if (mb > 0)
//           cout << "mouse button " << mb << " was pressed at " << mx << "," << my << endl;
//        else if (mb < 0)
//           cout << "mouse button " << mb << " was released at " << mx << "," << my << endl;
//        else
//           cout << "mouse moved to " << mx << "," << my << endl;
//
bool GetMouseInput(int* mouseX, int* mouseY, int* mouseButton);

// Pause:
//    Causes the program to wait a certain amount of time. Useful for controlling animations.
//
void Pause(int milliseconds);

// SetDrawColour:
//    All drawing functions will draw using the RGB colour specified here.
//    Each colour component should be from 0.0 to 1.0.
//
// Example:
//    SetDrawColour(1.0, 0.0, 0.0); // pure red
//    DrawString(0, 0, "this text is red");
//    DrawString(0, 16, "so is this text");
//    SetDrawColour(1.0, 1.0, 0.0); // yellow
//    DrawString(0, 32, "this text is yellow");
//
void SetDrawColour(double r, double g, double b);

// DrawString:
//    Coordinate (x0,y0) will be the top-left corner of the
//    text that is drawn, which is given by 'str'.
//    Newline characters '\n' are valid and will be recognized.
//
void DrawString(int x0, int y0, const char* str);

// DrawRectangleOutline, DrawRectangleFilled:
//    Coordinate (x0,y0) will be the top-left corner of the
//    rectangle and (x1-1, y1-1) will be the bottom left corner
//    (i.e. if x0==x1 or y0==y1 no drawing will take place).
//    Outlines are drawn 'inside' the shape, not outside.
//
void DrawRectangleOutline(int x0, int y0, int x1, int y1);
void DrawRectangleFilled(int x0, int y0, int x1, int y1);

// DrawEllipseOutline, DrawEllipseFilled:
//    The ellipse will be centered at (x,y) with a separate radius for
//    each axis (radiusX, radiusY).
//    Outlines are drawn 'inside' the shape, not outside.
//
void DrawEllipseOutline(int x, int y, int radiusX, int radiusY);
void DrawEllipseFilled(int x, int y, int radiusX, int radiusY);

// DrawPolyline, DrawPolygonFilled:
//    DrawPolyline draws 'count-1' line segments, from (x[i],y[i]) 
//    to (x[i+1],y[i+1]) for each i=0..count-2.
//    DrawPolygonFilled works the same way except it fills the
//    interior region defined by those lines.
//
// Example:
//    int x[] = { 50, 100, 150, 100 }; // (x,y) define a diamond shape
//    int y[] = { 100, 50, 100, 150 };
//    SetDrawColour(0.0, 1.0, 0.0);
//    DrawPolygonFilled(x, y, 4); // green diamond!
//
void DrawPolyline(int* x, int* y, int count);
void DrawPolygonFilled(int* x, int* y, int count);

// CreateImage, CreateImageFromFile
//    These allocate pixel data for a new image, and return
//    a positive integer (i.e. a 'handle') that can be used to 
//    uniquely identify this image later on.
//
//    CreateImage creates a black image using the specified 
//    parameters (width, height, colour format etc).
//    Strings recognized in 'options' include:
//       "bgra" -- use BGRA pixel format (4 bytes per pixel)
//       "bgr"  -- use BGR pixel format (3 bytes per pixel)
//       "grey" -- use I pixel format (grey intensity, 1 byte per pixel)
//
//    CreateImageFromFile initializes the pixels to match the
//    image file on disk (only supports BMP files for now!).
//    If the file could not be read, returns zero (failure).
//
//  Example: (assuming you have 'tux.bmp' in your project directory)
//    int tuxlogo = CreateImageFromFile("tux.bmp");
//    DrawImageStretched(0, 0, 1000, 1000, tuxlogo); // huge penguin
//
//  (for example of CreateImage, see GetImagePixelPtr)
//  
int CreateImage(int sizeX, int sizeY, const char* options);
int CreateImageFromFile(const char* fileName);

// GetImageSizeX, GetImageSizeY
//    Returns the internal dimensions of the image, in pixels.
//
int GetImageSizeX(int handle);
int GetImageSizeY(int handle);

// GetImageBytesPerPixel
//    Returns how many bytes are used per pixel. Useful when
//    you've loaded an image file and want to be able to
//    process both greyscale or colour images.
//     * 4 bytes corresponds to BGRA format (A = alpha transparency)
//     * 3 bytes corresponds to BGR format
//     * 1 byte corresponds to I format (I = grey intensity)
//
int GetImageBytesPerPixel(int handle);

// GetImagePixelPtr
//    Returns a pointer to the raw bytes that define the
//    image's colour data. Only needed if you want to 
//    inspect/modify specific pixels. 
//
// Example:
//    int redline = CreateImage(9, 9, "bgr");
//    for (int x = 0; x < 9; ++x) {
//        unsigned char* p = GetImagePixelPtr(redline, x, 4);
//        p[0] = 0; p[1] = 0; p[2] = 255;  // set pixel at (x,4) to red
//    }
//    DrawImage(20, 10, redline); // draw a 9x9 black box with horizontal red line inside
//
unsigned char* GetImagePixelPtr(int handle, int x, int y);

// DrawImage, DrawImageStretched:
//    The top-left corner will be at (x0,y0). DrawImage(x,y,h) is equivalent
//    to DrawImageStretched(x,y,x+GetImageSizeX(h),y+GetImageSizeX(h),h).
//
void DrawImage(int x0, int y0, int handle);
void DrawImageStretched(int x0, int y0, int x1, int y1, int handle);

// DeleteImage
//    Be sure to call this to release memory when you're done with
//    an image. (images can take a LOT of memory!)
//
void DeleteImage(int handle);

#endif // __CS037UTIL_H__
