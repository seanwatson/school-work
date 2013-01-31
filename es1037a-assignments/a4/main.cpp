#include "cs1037util.h" // for basic drawing and keyboard/mouse input operations 
#include <iostream>     // for cout
#include "List.h"
#include "Stack.h"
#include "Basics2D.h"
#include "Table2D.h"
#include "Math2D.h"
#include "Image2D.h"
#include "segmentation.h" // declarations of global variables and functions shared with segmentation.cpp


// declarations of global variables used for GUI controls/buttons/dropLists
const char* image_names[] = { "logo.bmp" , "uwocampus.bmp", "canada.bmp", "original.bmp" , "model.bmp" , "liver.bmp" }; // an array of image file names
const char* mode_names[]  = { "CONTOUR", "REGION"}; // an array of mode names 
enum Mode {DRAW_CONTOUR=0, DRAW_REGION=1};
Mode mode = DRAW_CONTOUR; // index of the current mode (in the array 'mode_names')
bool view = false; // flag set by the check box
const int cp_height = 34; // height of "control panel" (area for buttons)
const int pad = 10; // width of extra "padding" around image (inside window)

// declarations of global functions used in GUI (see code below "main")
void left_click(Point click);   // call-back function for left mouse-clicks
void right_click(Point click);  // call-back function for right mouse-clicks
void image_load(int index); // call-back function for dropList selecting image file
void mode_set(int index);   // call-back function for dropList selecting mode 
void clear();  // call-back function for button "Clear"
void view_set(bool v);  // call-back function for check box for "view" 

                           

int main()
{
 	cout << " left/right clicks - enter contour points or region seeds " << endl; 
 	cout << " click 'Clear' or press 'c' to delete current 'contour' or 'region' " << endl; 
	cout << " click 'X'-box to close the application \n\n " << endl; 

	  // initializing buttons/dropLists and the window (using cs1037utils methods)
	int blank = CreateTextLabel(""); // adds grey "control panel" for buttons/dropLists, see "cs1037utils.h"
    SetControlPosition(blank,0,0); SetControlSize(blank,1280,cp_height); // see "cs1037utils.h"
	int dropList_files = CreateDropList(6, image_names, 0, image_load); // the last argument specifies the call-back function, see "cs1037utils.h"
	int label = CreateTextLabel("Mode:"); // see "cs1037utils.h"
	int dropList_modes = CreateDropList(2, mode_names, mode, mode_set); // the last argument specifies the call-back function, see "cs1037utils.h"
	int button_clear = CreateButton("Clear",clear); // the last argument specifies the call-back function, see "cs1037utils.h"
	int check_box_view = CreateCheckBox("view" , view, view_set); // see "cs1037utils.h"
	SetWindowTitle("CS1037 Segmentation");      // see "cs1037utils.h"
    SetDrawAxis(pad,cp_height+pad,false); // sets window's "coordinate center" for GetMouseInput(x,y) and for all DrawXXX(x,y) functions in "cs1037utils" 
	                                      // we set it in the left corner below the "control panel" with buttons
	  // initializing the application
	image_load(0);
	SetWindowVisible(true); // see "cs1037utils.h"

	  // while-loop processing keys/mouse interactions 
	while (!WasWindowClosed()) // WasWindowClosed() returns true when 'X'-box is clicked
	{
		char c;
		if (GetKeyboardInput(&c)) // check keyboard
		{ 
			if (c == 'c') clear(); // 
		}

		int x, y, button;
		if (GetMouseInput(&x, &y, &button)) // checks if there are mouse clicks or mouse moves
		{
			Point mouse(x,y); // STORES PIXEL OF MOUSE CLICK
			if      (button == 1) left_click(mouse);   // button 1 means the left mouse button
			else if (button == 2) right_click(mouse);  // button 2 means the right mouse button
			draw(mouse);
		}
	}

	  // deleting the controls
	DeleteControl(button_clear);    // see "cs1037utils.h"
	DeleteControl(dropList_files);
	DeleteControl(dropList_modes);     
	DeleteControl(label);
	DeleteControl(check_box_view);
	return 0;
}

// call-back function for the 'mode' selection dropList 
// 'int' argument specifies the index of the 'mode' selected by the user among 'mode_names'
void mode_set(int index)
{
	mode = (Mode) index;
	cout << "drawing mode is set to " << mode_names[index] << endl;
	reset_segm();
	draw();
}

// call-back function for the 'image file' selection dropList
// 'int' argument specifies the index of the 'file' selected by the user among 'image_names'
void image_load(int index) 
{
	cout << "loading image file " << image_names[index] << endl;
	image = loadImage<RGB>(image_names[index]); // global function defined in Image2D.h
	SetWindowSize(max(400,(int)image.getWidth())+2*pad,max(100,(int)image.getHeight())+cp_height+2*pad); // window height includes control panel ("cp")
	reset_segm();  // clears current "contour" and "region" objects - function in segmentation.cpp
	draw();
}

// call-back function for button "Clear"
void clear() { 
	reset_segm(); // clears current "contour" and "region" objects - function in segmentation.cpp
	draw();
}

// call-back function for check box for "view" check box 
void view_set(bool v) {view=v; draw();}  

// call-back function for left mouse-click
void left_click(Point click)
{
	if (!image.pointIn(click)) return;
	if      (mode==DRAW_CONTOUR) addToContour(click); // function in segmentation.cpp
	else if (mode==DRAW_REGION && region[click]==0) regionGrow(click,10); // function in segmentation.cpp
}

// call-back function for right mouse-click
void right_click(Point click)
{
	if (!image.pointIn(click)) return;
	if      (mode==DRAW_CONTOUR) addToContourLast(click); // function in segmentation.cpp
	else if (mode==DRAW_REGION  && region[click]==0)  regionGrow(click,3); // function in segmentation.cpp
}

// window drawing function
void draw(Point mouse)
{ 
	unsigned i;
	// Clear the window to white
	SetDrawColour(1.0,1.0,1.0); DrawRectangleFilled(-pad,-pad,1280,1024);
	
	if (!image.isEmpty()) drawImage(image); // draws image (object defined in wire.cpp) using global function in Image2D.h (1st draw method there)
	else {SetDrawColour(1.0, 0.0, 0.0); DrawString(2,2,"image was not found"); return;}

	// look-up tables specifying colors and transparancy for each possible integer value (0,1,2,3) in "region"
	RGB    colors[4]       = { black,  blue,  red,  white};
	double transparancy[4] = { 0,      0.2,   0.2,  1.0};
	if ((mode==DRAW_REGION || view || closedContour) && !region.isEmpty()) drawImage(region,colors,transparancy); // 4th draw() function in Image2D.h

	if (mode==DRAW_CONTOUR && !contour.isEmpty() && !closedContour) 
	{	// Draws "contour" - object declared in segmentation.cpp
		SetDrawColour(1.0,0.0,0.0); 
		for (i = 1; i<contour.getLength(); i++)  
			DrawLine(contour.retrieve(i).x,   contour.retrieve(i).y, 
					 contour.retrieve(i+1).x, contour.retrieve(i+1).y);

		// If contour is 'open' and mouse is over the image, draw extra 'preview' line segment to current mouse pos.
		if (image.pointIn(mouse) && !closedContour) 
		{
			Stack<Point> * path = liveWire(mouse);
			if (path) {
				Point cur, prev = contour.retrieve(i); // start from the last contour point
				while (!path->isEmpty()) {cur=path->pop(); DrawLine(prev.x,prev.y,cur.x,cur.y); prev=cur;}
				delete path;
			}
		}
	}
}
