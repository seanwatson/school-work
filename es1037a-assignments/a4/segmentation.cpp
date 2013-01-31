#include <iostream>     // for cout, rand
#include "List.h"
#include "Stack.h"
#include "Queue.h" 
#include "queue" // needed if using a "priority_queue" in function "computePaths()"
#include "Basics2D.h"
#include "Table2D.h"
#include "Math2D.h"
#include "Image2D.h"
#include "segmentation.h"
#include <queue>
#include <math.h>


// GLOBAL PARAMETERS AND SPECIALISED DATA TYPES
const double INFTY=1.e20;
static const Point shift[4]={Point(-1,0),Point(1,0),Point(0,-1),Point(0,1)};
enum Direction {LEFT=0, RIGHT=1, TOP=2, BOTTOM=3, NONE=10};
const Direction Reverse[4]={RIGHT,LEFT,BOTTOM,TOP}; 
double fn(const double t) {double w=0.1; return 1.0/(1.0+w*t);}  // function used for setting "pixel penalties" ("Sensitivity" w is a tuning parameter)
	
// declarations of global variables 
Table2D<RGB> image; // image is "loaded" from a BMP file by function "image_load" in "main.cpp" 
List<Point> contour; // list of "contour" points
bool closedContour=false; // a flag indicating if contour was closed 
Table2D<int> region;  // 2D array storing binary mask for pixels (1 = "in", 0 = "out", 2 = "active front")
Table2D<double> penalty; // 2d array of pixels' penalties computed in reset_segm() 
                         // Low penalty[x][y] implies high image "contrast" at image pixel p=(x,y).
Table2D<double>    dist;     // 2D table of "distances" for current paths to the last seed
Table2D<Direction> toParent; // 2D table of "directions" along current paths to the last seed. 


// GUI calls this function when button "Clear" is pressed, or when new image is loaded
// THIS FUNCTION IS FULLY IMPLEMENTED, YOU DO NOT NEED TO CHANGE THE CODE IN IT
void reset_segm()
{
	cout << "resetting 'contour' and 'region'" << endl;

	// removing all region markings
	region.reset(image.getWidth(),image.getHeight(),0);

	// remove all points from the "contour"
	while (!contour.isEmpty()) contour.popBack();
	closedContour=false;

	// resetting 2D tables "dist" and "toParent" (erazing paths) 
	dist.reset(image.getWidth(),image.getHeight(),INFTY);
	toParent.reset(image.getWidth(),image.getHeight(),NONE);

	// recomputing "penalties" from an estimate of image contrast at each pixel p=(x,y)
	if (image.isEmpty()) {penalty.resize(0,0); return;} 
	Table2D<double> contrast = grad2(image);  //(implicit conversion of RGB "image" to "Table2D<double>") 
	// NOTE: function grad2() (see Math2D.h) computes (at each pixel) expression  Ix*Ix+Iy*Iy  where Ix and Iy
	// are "horizontal" and "vertical" derivatives of image intensity I(x,y) - the average of RGB values.
	// This expression describes the "rate of change" of intensity, or local image "contrast". 
	penalty = convert(contrast,&fn); // "&fn" - address of function "fn" (defined at the top of this file) 
	// "convert" (see Math2D.h) sets penalty at each pixel according to formula "penalty[x][y] = fn (contrast[x][y])" 
}


/////////////////////////////////////////////////////////////////////////////////////////
// GUI calls this function as "mouse" moves over the image to any new pixel p. It is also 
// called inside "addToContour" for each mouse click in order to add the current live-wire
// to the contour. Function should allocate, initialize, and return a path (stack) of 
// adjacent points from p to the current "seed" (last contour point) but excluding the seed. 
// The path should follow directions in 2D table "toParent" computed in "computePaths()".
Stack<Point>* liveWire(Point p)
{
	if (contour.isEmpty()) return NULL;  
	
	// Create a new stack of Points dynamically.
	Stack<Point> * path = new Stack<Point>();

	// Add point 'p' into it. Then (iteratively) add its "parent" according to
	// 2D Table "toParent", then the parent of the parent, e.t.c....    until
	// you get a pixel that has parent "NONE". In case "toParent" table is 
	// computed correctly inside "computePaths(seed)", this pixel should be 
	// the current "seed", so you do not need to add it into the "path".
	// ....
	while (toParent[p]!=NONE){
		path->push(p);
		p = p + shift[toParent[p]];
	}

	// NOTE: PROVIDED CODE IN "computePaths()" INITIALIZES 2D TABLE "toParent"
	//       WITH *SOME* CONSISTENT PATHS (FROM ALL PIXELS) TO THE LAST SEED (last contour point).
	//       THUS, YOU CAN SAFELY USE "toParent" WHEN IMPLEMENTING THIS FUNCTION.  

	// Return the pointer to the stack.
	return path;
}

// GUI calls this function when a user left-clicks on an image pixel while in "Contour" mode
void addToContour(Point p) 
{
	if (closedContour) return;

	// if contour is empty, append point p to the "contour", 
	// else (if contour is not empty) append to the "contour" 
	// all points returned by "live-wire(p)" (a path from p to last seed). 
	if (contour.isEmpty()) contour.append(p);
	else 
	{   // YOU NEED TO REPLACE THE LINE BELOW WITH YOUR OWN CODE
		// ....
		Stack <Point>* live_points = liveWire(p);
		while (!live_points->isEmpty())
			contour.append(live_points->pop()); 
		delete live_points;
	}

	// The call to computePaths(p) below should precompute "optimal" paths 
	// to the new seed p and store them in 2D table "toParent".
	computePaths(p);
}

// GUI calls this function when a user right-clicks on an image pixel while in "Contour" mode
void addToContourLast(Point p)
{
	if (closedContour || contour.isEmpty()) return;

	// REPLACE 2 LINES OF CODE BELOW TO CLOSE THE CONTOUR USING "livewire"
	// ...
	addToContour(p);
	addToContour(contour.retrieve(1));

	closedContour = true;
	draw();
	
	cout << "interior region has volume of " << contourInterior() << " pixels" << endl; 
}

// GUI calls this function when a user left-clicks on an image pixel while in "Region" mode
// The method computes a 'region' of pixels connected to 'seed'. The region is grown by adding
// to each pixel p (starting at 'seed') all neighbors q such that intensity difference is small,
// that is, |Ip-Iq|<T where T is some fixed threshold. Use Queue for active front. 
// HINT: to compute intensity difference Ip-Iq, use function dI(RGB,RGB) defined in "Image2D.h"   
void regionGrow(Point seed, double T)
{
	if (!image.pointIn(seed)) return;
	int counter = 0;                  
	Queue<Point> active;        
	active.enqueue(seed);
	
	// use BREADTH-FIRST_SEARCH (FIFO order) traversal - "Queue"
    //..
	while (!active.isEmpty())
	{
		Point p = active.dequeue();
		region[p]=1;  
		counter++;
		for (int i=0; i<4; i++)
		{   
			Point q = p + shift[i]; 
			if (image.pointIn(q) && region[q]==0 && dI(image[q], image[seed])>(T*-1) && dI(image[q], image[seed])<T) 
			{ 
				active.enqueue(q); 
				region[q]=2;  
			}                 
		}                     
		if (view && counter%60==0) {draw(); Pause(20);}
	}

	cout << "grown region of volume " << counter << " pixels    (threshold  " << T << ")" << endl;
}

// This function is called at the end of "addToContourLast()". Function 
// "contourInterior()" returns volume of the closed contour's interior region 
// (including points on the contor itself), or -1 if contour is not closed yet.
// HINT: live-wire() adds to the contour "water-tight" paths of neighboring pixels 
int contourInterior()
{
	if (!closedContour || image.isEmpty()) return -1;
	int counter = 0;

	// write your code for computing correct mask (region)
	// which should have 3 for pixels OUTSIDE the contour and 0
	// for pixels INSIDE of the contour
	// HINT: "grow" the "exterior region" from a single exterior point (e.g. a corner of the image). 
	// Make sure that the region does not grow across the contour (points in list "contour")
	// ...
	Queue<Point> active;
	Point q(1,1);
	active.enqueue(q);
	region.reset(0);
	for (unsigned i=1; i<=contour.getLength(); i++)
		region[contour.retrieve(i)]=2;
	while (!active.isEmpty())
	{
		Point p = active.dequeue();
		region[p]=3;  
		counter++;
		for (int i=0; i<4; i++)
		{   
			Point j = p + shift[i]; 
			if (image.pointIn(j) && region[j]!=2 && region[j]!=3)
			{ 
				active.enqueue(j); 
				region[j]=3;  
			}                 
		}  
		if (view && counter%60==0) {draw(); Pause(20);}
	}
	for (unsigned i=1; i<=contour.getLength(); i++)
		region[contour.retrieve(i)]=0;

	return region.getWidth() * region.getHeight() - counter;
}

///////////////////////////////////////////////////////////////////////////////////
// computePaths() is a function for "precomputing" live-wire (shortest) paths from 
// any image pixel to the given "seed". This method is called inside "addToContour" 
// for each new mouse click. Optimal path from any pixel to the "seed" should accumulate 
// the least amount of "penalties" along the path (each pixel p=(x,y) on a path contributes 
// penalty[p] precomputed in "reset_segm()"). In this function you should use 2D tables 
// "toParent" and "dist" that store for each pixel its "path-towards-seed" direction and, 
// correspondingly, the sum of penalties on that path (a.k.a. "distance" to the seed). 
// The function iteratively improves paths by traversing pixels from the seed until 
// all nodes have direction "toParent" giving the shortest (cheapest) path to the "seed".
void computePaths(Point seed)
{
	if (!image.pointIn(seed)) return;
	region.reset(0); // resets 2D table "region" for visualization

	// Reset 2D arrays "dist" and "toParent"  (erazing all current paths)
	dist.reset(INFTY);
	toParent.reset(NONE);
	dist[seed] = 0;
	int counter = 0; // just for info printed at the bottom of the function
	
	// THE CODE BELOW GENERATES 2d TABLE toParent WITH PATHS BASED ON           !!!!!!!!
	// BASIC BREDTH-FIRST_SEARCH TRAVERSAL FROM THE SEED. REPLACE THIS CODE     !!!!!!!!
	// SO THAT toParent CONTAINS PATHS FOLLOWING HIGH CONSTAST IMAGE BOUNDARIES !!!!!!!!
	// 
	// Create a queue (or priority_queue) for "active" points/pixels and 
	// traverse pixels to improve paths stored in "toParent" (and "dist") 
	// .....
	Queue<Point> active;         
	active.enqueue(seed);        
	while(!active.isEmpty()) 
	{
		Point p = active.dequeue();
		region[p]=1; 
		counter++;
		for (int i=0; i<4; i++) 
		{
			Point q = p+shift[i]; //using overloaded operator+ for Points (see "Basic2D.h")
			double t = dist[p] + penalty[p];
			if (image.pointIn(q) && t<dist[q])
			{  
				dist[q]=t;
                toParent[q]=Reverse[i]; 
				region[q]=2; // to visualize pixels added to the queue
				active.enqueue(q); 
			}
		}
		if (view && counter%60==0) {draw(); Pause(20);}  // visualization, skipped if checkbox "view" is off
	}
//	MY PRIORITY QUEUE METHOD WORKS IN THE SAME FASHION AS THE DEMO
//	IT IS SLOWER THAN MY QUEUE METHOD, BUT I AM UNSURE IF THIS IS
//	AN ISSUE WITH RELEASE MODE COMPARED TO DEBUG MODE
//	COMMENT OUT THE ABOVE QUEUE METHOD AND UNCOMMENT THE BELOW METHOD
//	TO TEST FUNCTIONALITY OF PRIORITY QUEUE METHOD
	/*priority_queue<MyPoint> active;
	MyPoint j(seed, dist[seed]);
	active.push(j);        
	while(!active.empty()) 
	{
		Point p = active.top();
		active.pop();
		region[p]=1;
		counter++;
		for (int i=0; i<4; i++) 
		{
			Point q = p+shift[i]; //using overloaded operator+ for Points (see "Basic2D.h")
			double t = dist[p] + penalty[p];
			if (image.pointIn(q) && t<dist[q])
			{  
				dist[q]=t;
                toParent[q]=Reverse[i]; 
				region[q]=2; // to visualize pixels added to the queue
				MyPoint b(q, dist[q]);
				active.push(b); 
			}
		}
		if (view && counter%60==0) {draw(); Pause(20);}  // visualization, skipped if checkbox "view" is off
	}*/

	// you can print out the number of times a point was removed from the queue
	cout << "paths computed,  number of 'pops' = " << counter 
		 <<  ",  number of pixels = " << (region.getWidth()*region.getHeight()) << endl; 
}


