/*******************************************************************************
  Course(s):
     CS1037, University of Western Ontario

  Author(s):
     Andrew Delong <firstname.lastname@gmail.com>

  Description:
     This file provides functions to make the following things easy:
      - draw shapes and bitmaps in a window,
      - poll keyboard and mouse for input, 
      - work with basic 'controls' (buttons, checkboxes etc), and
      - creating simple charts (for timing results, etc).

     For simplicity, all function interfaces exclusively use built-in 
     types (int, float, etc).

     To get started, the most basic program possible is something like...
        
        #include "cs1037util.h"
        
        void main() {
            SetWindowVisible(true);          // Show the main window.
            while (!WasWindowClosed()) { }   // Wait for user to close it.
        }
        
     If your program needs to poll the keyboard and mouse directly, paste 
     the program below into your project and run it to see how it works:
     
        #include "cs1037util.h"
        #include <iostream>
        using namespace std;
        
        void main() {
            SetWindowVisible(true);
            while (!WasWindowClosed()) { 
                char c;
                if (GetKeyboardInput(&c))
                    cout << "Key pressed; c = '" << c << "'" << endl;

                int mx, my, mb;
                if (GetMouseInput(&mx, &my, &mb)) {
                    cout << "Mouse moved; (mx,my) = (" << mx << "," << my << ")" << endl;
                    if (mb > 0)
                        cout << "Button pressed; mb = " << mb << endl;
                    else if (mb < 0)
                        cout << "Button released; mb = " << mb << endl;
                }
            }
        }


*******************************************************************************/

#ifndef __CS1037UTIL_H__
#define __CS1037UTIL_H__

////////////////////////////////////////////////////////////////////////////////
//                      Window Creation and Basic Drawing
////////////////////////////////////////////////////////////////////////////////

// SetWindowVisible:
//    Call SetWindowVisible(true) to make the main window show up on screen.
//
void SetWindowVisible(bool visible);

// SetWindowTitle:
//    Sets the text in the title bar of the window
//
void SetWindowTitle(const char* title);

// SetWindowPosition, SetWindowSize:
//    Repositions the top-left corner of the window, and resizes the window.
//
void SetWindowPosition(int x, int y);
void SetWindowSize(int sizeX, int sizeY);

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

// DrawLine:
//    Draws a line segment from (x0,y0) to (x1,y1).
//
void DrawLine(int x0, int y0, int x1, int y1);

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

// SetDrawAxis:
//    GetMouseInput() and all DrawXXX() functions place the origin (0,0) 
//    at the top-left of the window by default. SetDrawAxis lets you 
//    draw/receive all coordinates with respect to a different axis origin.
//
// Example:
//    SetWindowSize(512,512);
//    SetDrawAxis(256,256,true);  // Origin at center of window, Y increases upwards.
//
void SetDrawAxis(int originX, int originY, bool flipY);


////////////////////////////////////////////////////////////////////////////////
//                      Image Loading/Drawing
////////////////////////////////////////////////////////////////////////////////

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
//       "gray" -- use I pixel format (gray intensity, 1 byte per pixel)
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

// WriteImageToFile
//    Saves the image to disk, using the given file name.
//    Only BMP format is supported, so the fileName must end in ".bmp"
//  
void WriteImageToFile(int handle, const char* fileName);

// GetImageSizeX, GetImageSizeY
//    Returns the internal dimensions of the image, in pixels.
//
int GetImageSizeX(int handle);
int GetImageSizeY(int handle);

// GetImageBytesPerPixel
//    Returns how many bytes are used per pixel. Useful when
//    you've loaded an image file but need different code to handle
//    grayscale versus colour images.
//     * 4 bytes corresponds to BGRA format (A = alpha transparency)
//     * 3 bytes corresponds to BGR format
//     * 1 byte  corresponds to I format (I = gray intensity)
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
//    DrawImage(redline, 20, 10); // draw a 9x9 black box with horizontal red line inside
//
// Note (advanced): 
//    RGB and gray images with odd width will have padding bytes at end of row.
//
unsigned char* GetImagePixelPtr(int handle, int x, int y);

// DrawImage, DrawImageStretched:
//    The top-left corner will be at (x0,y0). If you stretch the image,
//    the bottom-right corner will be at (x1,y1).
//
void DrawImage(int handle, int x0, int y0);
void DrawImageStretched(int handle, int x0, int y0, int x1, int y1);

// DeleteImage
//    Be sure to call this to release memory when you're done with
//    an image. (images can take a LOT of memory!)
//
void DeleteImage(int handle);



////////////////////////////////////////////////////////////////////////////////
//                      Basic Controls
////////////////////////////////////////////////////////////////////////////////

// Define some shorthand names for various function-pointer types. 
// If you do not understand this weird C syntax, that is totally okay.
typedef void (*functionptr)();                    // pointer type for:  void foo()
typedef void (*functionptr_int)(int);             // pointer type for:  void foo(int index)
typedef void (*functionptr_bool)(bool);           // pointer type for:  void foo(bool state)
typedef void (*functionptr_float)(float);         // pointer type for:  void foo(float value)
typedef void (*functionptr_string)(const char*);  // pointer type for:  void foo(const char* text)
typedef void (*functionptr_intint)(int,int);      // pointer type for:  void foo(int a, int b)
typedef void (*functionptr_voidptr)(void*);       // pointer type for:  void foo(void* arg)
//
// For the curious, here's a ridiculously brief description of function pointers.
// In C/C++, functions such as
//    void foo1(int x) { cout <<  x; }
//    void foo2(int x) { cout << -x; }
// can be called by typing foo1(5) later on. Something else you can do with a 
// function name is to say &foo1. In English, &foo1 means "don't call foo1, just 
// give me the address of foo1's code." This makes sense because, just like
// variables, each chunk of code takes up memory and therefore has a location.
// For example, given foo1 and foo2 above we can later write
//    functionptr_int f = &foo1;  // f points to code of foo1
//    f(5);                       // calls foo1(5)
//    f = &foo2;                  // f points to code of foo2
//    f(5);                       // calls foo2(5)
// Above, f is a variable and takes 4 bytes just like any other pointer would.
// The type of f is "pointer to any function that takes one int argument."
//


// CreateTextLabel:
//    Creates a text label that sits "on top" any drawing that's going on in the 
//    main window. Returns a unique integer identifying that label (the label's 
//    "handle"). Useful for putting beside other controls such as TextBox.
//
// Example:
//    void main() {
//        SetWindowVisible(true);
//        int label = CreateTextLabel("Muaha you can't draw on top of me!");
//        while (!WasWindowClosed()) { }   // Wait for user to close main window.
//        DeleteControl(label);
//    }
//
int CreateTextLabel(const char* text);
void SetTextLabelString(int handle, const char* text);

// CreateButton:
//    Creates a Button 'control' inside the window and returns
//    a unique integer identifying that button (the button's "handle"). 
//    When clicked, the button calls whatever function you passed as onClick. 
//
// Example:
//    void tickle() { 
//        cout << "That tickles!" << endl;
//    } 
//
//    void main() {
//        SetWindowVisible(true);
//        int button = CreateButton("I'm a button", tickle);
//        while (!WasWindowClosed()) { }
//        DeleteControl(button);
//    }
//
int CreateButton(const char* text, functionptr onClick);

// CreateCheckBox:
//    Creates a checkbox control with text beside it. 
//    Each time the user checks/unchecks, the checkbox 
//    calls whatever function you passed as onStateChanged.
//
// Example:
//    void moodSwing(bool isHappy) { 
//        if (isHappy) cout << "Yes, I am." << endl;
//        else         cout << "No, I am not." << endl;
//    } 
//
//    void main() {
//        SetWindowVisible(true); 
//        int checkBox = CreateCheckBox("Are you happy?", true, &moodSwing);
//        while (!WasWindowClosed()) { }
//        DeleteControl(checkBox);
//    }
//
int CreateCheckBox(const char* text, bool checked, functionptr_bool onStateChanged);
bool GetCheckBoxState(int handle);
void SetCheckBoxState(int handle, bool checked);

// CreateTextBox:
//    Creates a textbox control. Each time the user edits the text,
//    the textbox calls whatever function you passed as onStringChanged.
//
// Example:
//    void movieChanged(const char* movieString) { 
//        cout << "Your favourite movie is " << movieString << endl;
//    } 
//
//    void main() {
//        SetWindowVisible(true); 
//        int movieBox = CreateTextBox("Being John Malkovich", &movieChanged);
//        while (!WasWindowClosed()) { }
//        DeleteControl(movieBox);
//    }
//
int CreateTextBox(const char* text, functionptr_string onStringChanged);
const char* GetTextBoxString(int handle);
void SetTextBoxString(int handle, const char* text);

// CreateDropList:
//    Creates a drop-list control. Each item in the list displays
//    a text string. Each time the user selects a different item from the
//    list, the list calls whatever function you passed as 'onSelectionChanged'.
//
// Example:
//    const char* candidates[] = { "Obama", "McCain", "Nader", "Barr" };
//
//    void vote(int index) { 
//        cout << "You voted for " << candidates[index] << endl;
//    } 
//
//    void main() {
//        SetWindowVisible(true);
//        int dropList = CreateDropList(4, candidates, 2, &vote);
//        while (!WasWindowClosed()) { }         
//        DeleteControl(dropList);
//    }
//
int CreateDropList(int numItems, const char* const items[], int selected, functionptr_int onSelectionChanged);
int GetDropListSelection(int handle);
void SetDropListSelection(int handle, int selected);

// CreateSlider:
//    Creates a slider control. Each time the user drags the slider left/right,
//    the slider calls whatever function you passed as onValueChanged.
//    The 'value' of the slider is always in the interval [0.0, 1.0].
//
//    By default, sliders are horizontal. To make a slider appear vertical, 
//    just apply SetControlSize() with sizeY > sizeX.
//
// Example:
//    void setVolume(float volume) { 
//        int percent = (int)(volume*100);
//        cout << "The volume is now at " << percent << "%" << endl;
//    } 
//
//    void main() {
//        SetWindowVisible(true); 
//        int volumeSlider = CreateSlider(0.5f, &setVolume, false);
//        while (!WasWindowClosed()) { }
//        DeleteControl(volumeSlider);
//    }
//
int CreateSlider(float value, functionptr_float onValueChanged, bool isHorizontal=true);
float GetSliderValue(int handle);
void SetSliderValue(int handle, float value);

// SetControlPosition, SetControlSize:
//    These can be called for any type of control (buttons, drop-lists etc).
//    Position is relative to the top-left corner of the window, in pixels.
//
void SetControlPosition(int handle, int x, int y);
void SetControlSize(int handle, int sizeX, int sizeY);

// GetControlPosition, GetControlSize:
//    If you need to align controls manually (heaven forbid), then these functions
//    will assign to the variables whose addresses you pass in.
//
void GetControlPosition(int handle, int* x, int* y);
void GetControlSize(int handle, int* sizeX, int* sizeY);

// DeleteControl:
//    Removes the control from the window and deletes it.
//
void DeleteControl(int handle);

// SetWindowResizeCallback:
//    Every time the main window is resized, the window will call the
//    function that you passed in for onWindowResize. So if the user is 
//    dragging the corner of the window to resize, your function is called.
//
// Example:
//    void resized(int sizeX, int sizeY) {
//        cout << "Window is now " << sizeX << " by " << sizeY << " pixels." << endl;
//    }
//
//    void main() {
//        SetWindowResizeCallback(&resized);
//        SetWindowVisible(true);
//        while (!WasWindowClosed()) { }         
//    }
//
void SetWindowResizeCallback(functionptr_intint onWindowResize);



////////////////////////////////////////////////////////////////////////////////
//                      MISCELLANEOUS FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// GetMilliseconds,GetMicroseconds:
//    Returns the number of milliseconds since the program started running.
//    Useful for benchmarking algorithms, and for getting animations/simulations 
//    to run at a consistent speed.
//
int GetMilliseconds();
int GetMicroseconds();

// Pause:
//    Causes the program to wait ('freeze') a certain amount of time. 
//    Useful for controlling animations.
//
void Pause(int milliseconds);

// GetRandomInt, GetRandomFloat:
//    GetRandomInt returns a random, non-negative integer.
//    GetRandomFloat returns a random float in the range [0,1).
//
// Example:
//    int   percent1 = GetRandomInt()   % 101;  // random integer in [0,100]
//    float percent2 = GetRandomFloat() * 100;  // random float   in [0,100)
//
int GetRandomInt();
float GetRandomFloat();



////////////////////////////////////////////////////////////////////////////////
//                      MULTI-THREADING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// StartParallelThread, StartParallelThreadWithArg:
//    Starts a parallel thread of execution, using 'func' as the
//    entry point (i.e. the bottom of the new thread's call stack).
//
// Example 1:
//    void printmarco() {
//        for (int i = 0; i < 5; ++i) {
//            cout << "Marco!\n";
//            Pause(2000);
//        }
//    }
//
//    void main() {
//        StartParallelThread(printmarco);  // Extra thread prints "Marco!"
//        Pause(500);
//        for (int i = 0; i < 5; ++i) {     // Main thread prints "Polo!"
//            cout << "Polo!\n";
//            Pause(2000);
//        }
//    }
//
// Example 2:
//    void printsomething(void* msg) {
//        for (int i = 0; i < 5; ++i) {
//            cout << (char*)msg;
//            Pause(2000);
//        }
//    }
//
//    void main() { // Extra threads print "Marco!" and "Polo!" via same function
//        StartParallelThreadWithArg(printsomething,"Marco!\n");
//        Pause(500);
//        StartParallelThreadWithArg(printsomething,"Polo!\n");
//        Pause(10000); // Main thread just waits
//    }
//
void StartParallelThread(functionptr func);
void StartParallelThreadWithArg(functionptr_voidptr func, void* arg);

// BeginCriticalSection, EndCriticalSection:
//    If you have a portion of code that is not safe to execute
//    in parallel, then that code is a "critical section".
//    For example, even incrementing a variable is not 'atomic',
//    so a variable should only be modified by one thread at a time.
//
//    A program can have many critical sections at different places,
//    so the programmer should designate each with a unique 'id'.
//    If thread A calls BeginCriticalSection(0), then thread B can call 
//    BeginCriticalSection(1) and it will not have to wait for thread A.
//    Valid ids are from 0 to 99.
//
// Example:
//    volatile double counter = 0;       // data shared by threads is 'volatile'
//
//    void addtocounter() {
//        for (int i = 0; i < 10000000; ++i) {
//            BeginCriticalSection(0);   // make other threads wait...
//            counter++;
//            EndCriticalSection(0);     // ...until we finish our add
//        }
//    }
//
//    void main() {
//        for (int i = 0; i < 9; ++i)    // 9 threads trying to do 'counter++'
//            StartParallelThread(addtocounter); 
//        while (counter < 90000000) { } // without Begin/EndCriticalSection,
//    }                                  // this loop may wait forever!
//
void BeginCriticalSection(int id);
void EndCriticalSection(int id);



////////////////////////////////////////////////////////////////////////////////
//                      PLOTTING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// CreatePlot:
//    Creates a 2D chart area within the main window. 
//    If you then add data to this chart, it will automatically
//    be plotted in the chart's area. The plotted (x,y) range is
//    automatically adjusted to accommodate the min/max data range.
//
//    Each plot can be identified by a unique integer handle, just 
//    like for other controls.
//
//    By default, a new chart occupies the entire main window.
//    You can move chart via SetControlPosition, SetControlSize.
//
// Example:
//    int plot = CreatePlot("Climate Change", "year", "parts-per-million");
//    int co2 = CreatePlotSeries("CO2 concentration", "-", plot);
//    AddSeriesPoint(1900, 282, co2);
//    AddSeriesPoint(1950, 313, co2);
//    AddSeriesPoint(2000, 370, co2);
//
int CreatePlot(const char* title, const char* axisLabelX = 0, const char* axisLabelY = 0);

// CreatePlotSeries:
//    Starts a new data series in a plot. The 'title' appears in the
//    plot's legend. The (optional) line style is specified as a string 
//    that works just like Matlab:
//       "b" means colour the series blue.
//       "r" means colour the series red.
//       "g" means colour the series green.
//       "-" means draw lines between points.
//       "*" means draw a star at each data point.
//       "+" means draw a plus at each data point.
//       "x" means draw a cross at each data point.
//    You can combine the above in any sensible way, so "rx-"
//    means "red crosses at each point, lines between them."
//
//    All arguments are optional. If you omit the specific 'plot' handle,
//    then the series gets added to the most recently created plot.
//    (If no plot has yet been created, one will be created automatically.)
//
int CreatePlotSeries(const char* title, const char* style = 0, int plot = 0);

// AddSeriesPoint:
//    Adds an (x,y) point to a plot series. The plot will automatically
//    adjust its range to show the new data point, if necessary.
//
//    If you omit the specific 'series' handle, then the point will be 
//    added to the most recently created series.
//    (If no series has yet been created, one will be created autmatically.)
//
// Example:
//    void main() {
//        SetWindowVisible(true);        // show main window
//        for (int i = 0; i <= 100; ++i)
//            AddSeriesPoint(i, i*i);    // plot i^2 in the window
//        while (!WasWindowClosed()) { } // wait for user
//    }
//
void AddSeriesPoint(int x, int y, int series = 0);
void AddSeriesPoint(float x, float y, int series = 0);




///////////////////////////////////////////////////////////////////////////////////////////////////
// The macros below allow us to supply students with a precise error message whenever
// they call one of the above functions incorrectly. If an assertion fails inside a  
// cs1037util function, the error message will include the file name and line number of
// the *student's* code that caused the crash, rather than the line of code inside
// cs1037utils.cpp that detected the problem.
//
#if !defined(CS1037UTIL_DISABLE_ASSERT_MACROS) && (_MSC_VER >= 1400)
// Note that "variadic macros" are quirky for any function that accepts zero arguments.
// So, this macro trick will only compile for functions that take >= 1 arguments.
extern int gLINE;
extern const char* gFILE;
#define CreateImage(...)          (gLINE=__LINE__, gFILE=__FILE__, ::CreateImage(__VA_ARGS__))
#define CreateImageFromFile(...)  (gLINE=__LINE__, gFILE=__FILE__, ::CreateImageFromFile(__VA_ARGS__))
#define WriteImageToFile(...)     (gLINE=__LINE__, gFILE=__FILE__, ::WriteImageToFile(__VA_ARGS__))
#define GetImageSizeX(...)        (gLINE=__LINE__, gFILE=__FILE__, ::GetImageSizeX(__VA_ARGS__))
#define GetImageSizeY(...)        (gLINE=__LINE__, gFILE=__FILE__, ::GetImageSizeY(__VA_ARGS__))
#define GetImageBytesPerPixel(...)(gLINE=__LINE__, gFILE=__FILE__, ::GetImageBytesPerPixel(__VA_ARGS__))
#define GetImagePixelPointer(...) (gLINE=__LINE__, gFILE=__FILE__, ::GetImagePixelPointer(__VA_ARGS__))
#define DrawImage(...)            (gLINE=__LINE__, gFILE=__FILE__, ::DrawImage(__VA_ARGS__))
#define DrawImageStretched(...)   (gLINE=__LINE__, gFILE=__FILE__, ::DrawImageStretched(__VA_ARGS__))
#define DeleteImage(...)          (gLINE=__LINE__, gFILE=__FILE__, ::DeleteImage(__VA_ARGS__))
#define CreateButton(...)         (gLINE=__LINE__, gFILE=__FILE__, ::CreateButton(__VA_ARGS__))
#define CreateDropList(...)       (gLINE=__LINE__, gFILE=__FILE__, ::CreateDropList(__VA_ARGS__))
#define GetDropListSelection(...) (gLINE=__LINE__, gFILE=__FILE__, ::GetDropListSelection(__VA_ARGS__))
#define SetDropListSelection(...) (gLINE=__LINE__, gFILE=__FILE__, ::SetDropListSelection(__VA_ARGS__))
#define CreateCheckBox(...)       (gLINE=__LINE__, gFILE=__FILE__, ::CreateCheckBox(__VA_ARGS__))
#define GetCheckBoxState(...)     (gLINE=__LINE__, gFILE=__FILE__, ::GetCheckBoxState(__VA_ARGS__))
#define SetCheckBoxState(...)     (gLINE=__LINE__, gFILE=__FILE__, ::SetCheckBoxState(__VA_ARGS__))
#define SetControlPosition(...)   (gLINE=__LINE__, gFILE=__FILE__, ::SetControlPosition(__VA_ARGS__))
#define SetControlSize(...)       (gLINE=__LINE__, gFILE=__FILE__, ::SetControlSize(__VA_ARGS__))
#define DeleteControl(...)        (gLINE=__LINE__, gFILE=__FILE__, ::DeleteControl(__VA_ARGS__))
#define StartParallelThread(...)  (gLINE=__LINE__, gFILE=__FILE__, ::StartParallelThread(__VA_ARGS__))
#define StartParallelThreadWithArg(...) (gLINE=__LINE__, gFILE=__FILE__, ::StartParallelThreadWithArg(__VA_ARGS__))
#define BeginCriticalSection(...) (gLINE=__LINE__, gFILE=__FILE__, ::BeginCriticalSection(__VA_ARGS__))
#define EndCriticalSection(...)   (gLINE=__LINE__, gFILE=__FILE__, ::EndCriticalSection(__VA_ARGS__))
#define CreatePlot(...)           (gLINE=__LINE__, gFILE=__FILE__, ::CreatePlot(__VA_ARGS__))
#define CreatePlotSeries(...)     (gLINE=__LINE__, gFILE=__FILE__, ::CreatePlotSeries(__VA_ARGS__))
#define AddSeriesPoint(...)       (gLINE=__LINE__, gFILE=__FILE__, ::AddSeriesPoint(__VA_ARGS__))
#endif

#endif // __CS1037UTIL_H__
