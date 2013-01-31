// ******************************************************
// Do not change this code. We will make changes to this
// file for testing your submissions. Hence, if you rely
// on changes that you have made yourself, your  program
// might fail some of our tests.
// ******************************************************

import java.io.*;
import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import javax.imageio.*;
import java.util.*;
import java.lang.Thread;

public class DrawLab extends JFrame{
  private Graphics display;    // Graphics object needed to draw
  private Board panel;         // Panel containing map

  private int numNodes;       // Number of nodes in the labyrinth
  private int wallsAllowed;   // Number of walls allowed in solution
  private int labWidth;      // Width of the labyrinth (in rooms)
  private int labLength;     // Length of the labyrinth (in rooms)

  private int roomSize;              //Length of the room in pixels

  private char[][] lab;

  private Color rockColor = new Color(0,0,100);
  private Color wallColor = new Color(255,100,100);
  private Color pathColor = new Color(255,0,0);
  private Color entranceColor = new Color(100,100,100);
  private Color exitColor = new Color(0,200,0);
  private Color backColor = new Color(255,255,150);
  private Color white = new Color(255,255,255);
  private Color door = new Color(128,60,60);

    private BufferedImage img, imgb;
  private boolean imageFile = true;
    private int startx, starty, endx, endy;
 
  /* ============================= */
  public DrawLab(String labFile) {
  /* ============================= */
    BufferedReader input;

    try {
      numNodes = 0;
      panel = new Board();
      getContentPane().add(panel);
 
      input = new BufferedReader(new FileReader(labFile));

      // Process first four lines of the file
      roomSize = Integer.parseInt(input.readLine());
      labWidth = Integer.parseInt(input.readLine());
      labLength = Integer.parseInt(input.readLine());
      wallsAllowed = Integer.parseInt(input.readLine());

      numNodes = labWidth * labLength;

      setSize((2*labWidth)*roomSize+10,(2*labLength)*roomSize+roomSize+10);
      setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
      setVisible(true);
      display = panel.getGraphics();

      Thread clock = new Thread();
      clock.sleep(2000);
      display.setColor(backColor);
      display.fillRect(0,0,(2*labWidth-1)*roomSize,(2*labLength-1)*roomSize);
      display.setColor(rockColor);
      display.drawRect(0,0,(2*labWidth-1)*roomSize,(2*labLength-1)*roomSize);
      display.drawRect(1,1,(2*labWidth-1)*roomSize-2,(2*labLength-1)*roomSize-2);
      display.drawRect(2,2,(2*labWidth-1)*roomSize-4,(2*labLength-1)*roomSize-4);
      display.drawRect(3,3,(2*labWidth-1)*roomSize-6,(2*labLength-1)*roomSize-6);

      img = ImageIO.read(new File("person.gif"));
      imgb = ImageIO.read(new File("personb.gif"));
      drawLabyrinth(input);
    }
    catch (InterruptedException e) {
      System.out.println("Error starting program");
    }
    catch (IOException e) {
	imageFile = false;
	System.out.println("Cannot read person");
    }
    catch (Exception e) {
      System.out.println("Error opening file "+labFile);
      labWidth = 1;
      labLength = 1;
      roomSize = 1;
    }
  }

 /* ================================================================= */
  public void drawLabyrinth(BufferedReader input) {
  /* ================================================================= */
     int  i, row = 0, col;
     String line;

     // Open the file
     try {

       lab = new char[2*labLength-1][2*labWidth-1];
       // Read the labyrinth from the file
       for (;;) {
           line = input.readLine();
           if (line == null) {             // End of file
               input.close();
               return;
           }

           /* Draw a row of the labyrinth */
           for (i = 0, col = 0; i < line.length(); ++i) {
	       lab[row][i] = line.charAt(i);
	         switch (line.charAt(i)) {
                  case 'h': drawHorWall(i,row);
                            if ((i % 2) == 0) ++col; break;
                  case 'v': drawVertWall(i,row); 
                            if ((i % 2)== 0) ++col; break;
                  case 's': drawStart(i,row); 
		            ++col; break;
                  case 'e': drawExit(i,row); 
		            ++col; break;
                  case ' ': drawRock(i,row); break;
                  case '-': drawHorHall(i,row);break;
                  case '|': drawVerHall(i,row);break;
		  case 'o': ++col;
                 }
	   }
           ++row; 

       }
     }
     catch (Exception e) {
        System.out.println("Error reading input file");
        return;
     }
   }


  /* Draw a wall of the labyrinth */
  /* ================================================================= */
  private void drawVertWall(int x, int y) {
  /* ================================================================= */
      display.setColor(backColor);
      display.fillRect(x*roomSize,y*roomSize,roomSize,roomSize);
      display.setColor(wallColor);
      display.fillRect(x*roomSize,y*roomSize+roomSize/4,roomSize,roomSize/2);
      display.setColor(white);
      display.drawLine(x*roomSize,y*roomSize+roomSize/2,x*roomSize+roomSize/4,
                       y*roomSize+roomSize/2);
      display.drawLine(x*roomSize+roomSize/2,y*roomSize+roomSize/2,
                       x*roomSize+3*roomSize/4,y*roomSize+roomSize/2);
      display.drawLine(x*roomSize+roomSize/4, y*roomSize+roomSize/4,
		       x*roomSize+roomSize/4,y*roomSize+3*roomSize/4);
      display.drawLine(x*roomSize+roomSize/2, y*roomSize+roomSize/4,
		       x*roomSize+roomSize/2,y*roomSize+3*roomSize/4);
      display.drawLine(x*roomSize+3*roomSize/4, y*roomSize+roomSize/4,
		       x*roomSize+3*roomSize/4,y*roomSize+3*roomSize/4);

      display.setColor(rockColor);
      display.fillRect(x*roomSize,y*roomSize,roomSize/5,roomSize);
      display.fillRect(x*roomSize+4*roomSize/5,y*roomSize,roomSize/5,roomSize);
  }

 /* ================================================================= */
  private void drawHorWall(int x, int y) {
  /* ================================================================= */
      display.setColor(backColor);
      display.fillRect(x*roomSize,y*roomSize,roomSize,roomSize);
      display.setColor(wallColor);
      display.fillRect(x*roomSize+roomSize/4,y*roomSize,roomSize/2,roomSize);
      display.setColor(white);
      display.drawLine(x*roomSize+roomSize/2,y*roomSize,x*roomSize+roomSize/2,
                       y*roomSize+roomSize/4);
      display.drawLine(x*roomSize+roomSize/2,y*roomSize+roomSize/2,
                       x*roomSize+roomSize/2,y*roomSize+3*roomSize/4);
      display.drawLine(x*roomSize+roomSize/4, y*roomSize+roomSize/4,
		       x*roomSize+3*roomSize/4,y*roomSize+roomSize/4);
      display.drawLine(x*roomSize+roomSize/4, y*roomSize+roomSize/2,
		       x*roomSize+3*roomSize/4,y*roomSize+roomSize/2);
      display.drawLine(x*roomSize+roomSize/4, y*roomSize+3*roomSize/4,
		       x*roomSize+3*roomSize/4,y*roomSize+3*roomSize/4);

      display.setColor(rockColor);
      display.fillRect(x*roomSize,y*roomSize,roomSize,roomSize/5);
      display.fillRect(x*roomSize,y*roomSize+4*roomSize/5,roomSize,roomSize/5);
  }

 /* ================================================================= */
  private void drawHorHall(int x, int y) {
  /* ================================================================= */
     //display.drawImage(wallImage,x*roomSize,y*roomSize,roomSize,roomSize,this);
      display.setColor(backColor);
      display.fillRect(x*roomSize,y*roomSize,roomSize,roomSize);
      display.setColor(rockColor);
      display.fillRect(x*roomSize,y*roomSize,roomSize,roomSize/5);
      display.fillRect(x*roomSize,y*roomSize+4*roomSize/5,roomSize,roomSize/5);
  }


 /* ================================================================= */
  private void drawVerHall(int x, int y) {
  /* ================================================================= */
     //display.drawImage(wallImage,x*roomSize,y*roomSize,roomSize,roomSize,this);
      display.setColor(backColor);
      display.fillRect(x*roomSize,y*roomSize,roomSize,roomSize);
      display.setColor(rockColor);
      display.fillRect(x*roomSize,y*roomSize,roomSize/5,roomSize);
      display.fillRect(x*roomSize+4*roomSize/5,y*roomSize,roomSize/5,roomSize);
  }


 /* ================================================================= */
  private void drawRock(int x, int y) {
  /* ================================================================= */
     //display.drawImage(wallImage,x*roomSize,y*roomSize,roomSize,roomSize,this);
      display.setColor(rockColor);
      display.fillRect(x*roomSize,y*roomSize,roomSize,roomSize);
  }


  /* Draw the entrance room */
  /* ================================================================= */
  private void drawStart(int x, int y) {
  /* ================================================================= */
    startx = x;
    starty = y;

    display.setColor(entranceColor);
    display.fillRect(x*roomSize+2*roomSize/5,y*roomSize+roomSize/5,
		     2*roomSize/5,3*roomSize/5);
    int[] xcoor = {x*roomSize+2*roomSize/5, x*roomSize+roomSize/5,
		   x*roomSize+roomSize/5, x*roomSize+2*roomSize/5};
    int[] ycoor = {y*roomSize+roomSize/5, y*roomSize+roomSize/10,
		   y*roomSize+7*roomSize/10,y*roomSize+4*roomSize/5};
    display.setColor(door);
    display.fillPolygon(xcoor,ycoor,4);
    display.setColor(wallColor);
    display.fillOval(x*roomSize+2*roomSize/10,y*roomSize+4*roomSize/10,
             roomSize/10,roomSize/10);
  }

  /* Draw the exit */
  /* ================================================================= */
  private void drawExit(int x, int y) {
  /* ================================================================= */
    endx = x;
    endy = y;

    display.setColor(exitColor);
    display.fillRect(x*roomSize+2*roomSize/5,y*roomSize+roomSize/5,
		     2*roomSize/5,3*roomSize/5);
    int[] xcoor = {x*roomSize+2*roomSize/5, x*roomSize+roomSize/5,
		   x*roomSize+roomSize/5, x*roomSize+2*roomSize/5};
    int[] ycoor = {y*roomSize+roomSize/5, y*roomSize+roomSize/10,
		   y*roomSize+7*roomSize/10,y*roomSize+4*roomSize/5};
    display.setColor(door);
    display.fillPolygon(xcoor,ycoor,4);
    display.setColor(wallColor);
    display.fillOval(x*roomSize+2*roomSize/10,y*roomSize+4*roomSize/10,
             roomSize/10,roomSize/10);  
  }

  /* ========================================================== */
  private void erasePerson(int x, int y) {
  /* ========================================================== */
      if (x == (startx * roomSize) && y == (starty * roomSize)) drawStart(startx,starty);
      else {
        display.setColor(backColor);
        display.fillRect(x,y,roomSize,roomSize);
      }
  }

  /* ========================================================== */
    private void drawPerson(int x, int y,boolean right) {
  /* ========================================================== */

      if (x == (endx * roomSize) && y == (endy * roomSize)) {
	if (right)
          display.drawImage(img,x+2*roomSize/5,y+roomSize/5,x+4*roomSize/5,
			y+4*roomSize/5,1,1,img.getWidth(),img.getHeight(),null);
	else 
          display.drawImage(imgb,x+2*roomSize/5,y+roomSize/5,x+4*roomSize/5,
			y+4*roomSize/5,1,1,img.getWidth(),img.getHeight(),null);
      }
      else 
	if (right)
          display.drawImage(img,x,y,x+roomSize,
			   y+roomSize,1,1,img.getWidth(),img.getHeight(),backColor,null);
	else 
          display.drawImage(imgb,x,y,x+roomSize,
			   y+roomSize,1,1,img.getWidth(),img.getHeight(),backColor,null);
  }


  /* Draws an edge of the solution */
  /* ========================================================== */
  private void drawEdge(int u, int v) {
  /* ========================================================== */
  /* Input: edge (uv) */
   
      int x,  y, us, vs, width, height, offset, labx, laby;

     if (u > numNodes || v > numNodes) {
        System.out.println("Invalid edge ("+u+","+v+")");
        return;
     }
     offset = roomSize / 2 - 1;
     us = u;
     vs = v;
     if (u > v) {x = u; u = v; v = x;}

     x = (u % labWidth)*2*roomSize + offset;
     y = (u / labWidth)*2*roomSize + offset;

     if (imageFile) erasePerson((us % labWidth)*2*roomSize,(us / labWidth)*2*roomSize);

     if (v == (u+1)) { 
       width = roomSize*2;
       height = 3;
       labx = 2*(u % labWidth)+1;
       laby = 2*(u / labWidth);
       if (lab[laby][labx] == 'h') 
	   drawBomb(x+3*roomSize/4,y-roomSize/4,roomSize/2);
     }
     else if (v == (u+labWidth)) {
       width = 3;
       height = roomSize*2;
       labx = 2*(u % labWidth);
       laby = 2*(u / labWidth)+1;
       if (lab[laby][labx] == 'v') 
	   drawBomb(x-roomSize/4,y+3*roomSize/4,roomSize/2);
     }
     else {
        System.out.println("Invalid edge ("+u+","+v+")");
        return;
     }

     display.setColor(pathColor);
     display.fillRect(x,y,width,height);

     if (imageFile) 
	 if ((us % labWidth) <= (vs % labWidth))
	     drawPerson((vs % labWidth)*2*roomSize,(vs / labWidth)*2*roomSize,true);
	 else drawPerson((vs % labWidth)*2*roomSize,(vs / labWidth)*2*roomSize,false);

  }


  /* Draws an explosion */
  /* ========================================================== */
    private void drawBomb(int originx, int originy, int size) {
  /* ========================================================== */
      for (int i = 0; i < 2; ++i) {
	int[] polyx = {originx,originx+size/3,originx+size/2,
		       originx+2*size/3,originx+size,originx+5*size/6,
		       originx+size,originx+5*size/6,originx+size,
		       originx+2*size/3,originx+size/2,originx+size/3,
		       originx,originx+size/6,originx, originx+size/6};
	int[] polyy = {originy,originy+size/6,originy,originy+size/6,
		       originy,originy+size/3,originy+size/2,
		       originy+2*size/3,originy+size,originy+5*size/6,
		       originy+size,originy+5*size/6,originy+size,
		       originy+2*size/3,originy+size/2,originy+size/3};

	if (i == 0) {
	    display.setColor(Color.orange);
	    originx = originx+size/4;
	    originy = originy+size/4;
	    size = size/2;
	}
	else display.setColor(Color.red);
	display.fillPolygon(polyx,polyy,16);
      }
    }




  /* ==================================== */
  public void drawEdge (Node u, Node v) {
  /* ==================================== */
      if (u != v) drawEdge(u.getName(),v.getName());
  }
}
