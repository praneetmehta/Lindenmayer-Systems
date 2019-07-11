#include <bits/stdc++.h>
#include <GLFW/glfw3.h>

using namespace std;

class DrawUtil{
public:
	struct RGB{
		double R;
		double G;
		double B;
		RGB():R(0),G(0),B(0){};
		RGB(double _r, double _g, double _b) : R(_r), G(_g), B(_b) {}; 
	};
	RGB Colors[4];

	void initializeColors()
	{
		RGB c1(0.0,0.5,0.0); 		//light green
		RGB c2(0.0,0.25,0.0);		//dark green
		RGB c3(0.26,0.13,0.09);		//brown
		RGB c4(.96,0.14,0.35);		//red
		Colors[0] = c1;
		Colors[1] = c2;
		Colors[2] = c3;
		Colors[3] = c4;
	}

	/* Function to plot individual pixels on the OpenGL window
	 {(0,0) is the lower left corner} */
	void PlotPixel(int x, int y)		
	{
		GLfloat pointerVertices[2]; // Glfloat type array to contain the x and y coordinates to plot
		pointerVertices[0] = x;
		pointerVertices[1] = y;
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glVertexPointer(2, GL_FLOAT, 0, pointerVertices);	// glVertexPointer(number_of_axes, type, 0, vertex)
	    glDrawArrays(GL_POINTS, 0, 1);	
	    glDisableClientState(GL_VERTEX_ARRAY);
	}

	/* 
	Bresenhamm's algorithm natively works for 1st octant(slope = 0 to slope = 1)
	So to generalize for all octants, all the line coordinates must be first transformed to octant 0,
	then obtain the points to be plotted, and transfer these points back to the initial octant.*/

	/* Step1 -> Identify the octant that the line belongs to */
	int findOctant(int dx, int dy)
	{
		int octant = 0;
		if(dx>=0) // check for octants 0, 1, 6, 7 where the value of x coordinate increments
		{
			if(dy>=0 && dy<=dx){octant = 0;}
			else if(dy>=0 && dy>dx){octant = 1;}
			else if(dy<0 && abs(dy)<=abs(dx)){octant = 7;}
			else{octant = 6;}
		}
		else //else choose from remaining octants i.e. 2, 3, 4, 5 where dx decrements
		{
			if(dy>=0 && abs(dy) > abs(dx)){octant = 2;}
			else if(dy>=0 && abs(dy) <= abs(dx)){octant = 3;}
			else if(dy<0 && abs(dy)<=abs(dx)){octant = 4;}
			else{octant = 5;}
		}
		return octant; // return the final octant the line lies in
	}

	/* Step2 -> transform the line from octant 'n' to octant 0 (octant 0 is where dy<=dx and dy>=0 and dx>=0)
	Then Bresenhamm's algorithm is applied on this set of points and the output of the algorithm needs to be transferred
	back to the original octant system*/ 


	/* 
		Octants:
		  \2|1/
		  3\|/0
		 ---+---
		  4/|\7
		  /5|6\  
  	*/
	tuple<int, int, int, int> switchOctantToZero(int x1, int y1, int x2, int y2, int n)
	{
		switch(n) // transform the line from octant n to octant 0 and return the final tuple
		{
			case 0: return make_tuple(x1, y1, x2, y2);
			case 1: return make_tuple(y1, x1, y2, x2);
			case 2: return make_tuple(y1, -x1, y2, -x2);
			case 3: return make_tuple(-x1, y1, -x2, y2);
			case 4: return make_tuple(-x1, -y1, -x2, -y2);
			case 5: return make_tuple(-y1, -x1, -y2, -x2);
			case 6: return make_tuple(-y1, x1, -y2, x2);
			case 7: return make_tuple(x1, -y1, x2, -y2);
		}
	}

	/* Step3 -> Transform the output from the Bresenhamm's Line drawing algorithms back to the
	initial octant of the line and then call the point plotting function on these points*/

	/* 
		Octants:
		  \2|1/
		  3\|/0
		 ---+---
		  4/|\7
		  /5|6\  
  	*/
	tuple<int, int> switchOctantFromZero(int x1, int y1, int n)
	{
		switch(n) // transfer the line from octant 0 back to octant n and return the final tuple
		{
			case 0: return make_tuple(x1, y1);
			case 1: return make_tuple(y1, x1);
			case 2: return make_tuple(-y1, x1);
			case 3: return make_tuple(-x1, y1);
			case 4: return make_tuple(-x1, -y1);
			case 5: return make_tuple(-y1, -x1);
			case 6: return make_tuple(y1, -x1);
			case 7: return make_tuple(x1, -y1);
		}
	}

	/* Function to implement the Bresenhamm's Line Drawing Algorithm on the set of points
	i.e the Starting point of the line(x1, y1) and the ending point(x2, y2)*/
	void drawLine(int x1, int y1, int x2, int y2)
	{
		// if the line is vertical, then simply plot the vertical pixels withoug applying the Bresenhamm's Algorithm
		if(x1 == x2)
		{
			int ystart = y2 > y1 ? y1 : y2;
			int yend = y2 > y1 ? y2 : y1;
			while(ystart <= yend)
			{
				PlotPixel(x1, ystart);
				ystart++;	// plot the next vertical pixel
			}
		}
		else // apply the Bresenhamm's Line Drawing algorithm
		{
			int n = findOctant(x2-x1, y2-y1);	//find the octant the line lies in
			tie(x1, y1, x2, y2) = switchOctantToZero(x1, y1, x2, y2, n);	//switch the octant accordingly to the 0 octant
			int dy = y2 - y1;
			int dx = x2 - x1;
			int d_param = 2*dy - dx; // decision variable
			int x = x1;
			int y = y1;
			int incr_NE = dy - dx; //value to increment the decision variable if North East pixel is chosen
			int incr_E = dy;	//value to increment the decision variable if the East pixel is chosen
			int x_out;
			int y_out;
			while(x != x2) //keep drawing till the end point of the line
			{
				if(d_param >= 0) // choose the north east pixel
				{
					d_param += incr_NE;
					tie(x_out, y_out) = switchOctantFromZero(x,y,n);	// switch back to initial octant system
					PlotPixel(x_out, y_out);
					y++;	// increment the y coordinate
				}
				else 	// choose the east pixel
				{
					d_param += incr_E;
					tie(x_out, y_out) = switchOctantFromZero(x,y,n);	// switch back to the initial octant system
					PlotPixel(x_out, y_out);
				}
				x++; //move to the next x coordinate
			}
		} 
	}

	/* Draw the points on circle in all the 8 octants using the cirle's
	property of 8-way symmetry, i.e. the points in all 8 octants are symmetric*/

	/* 
		Octants:
		  \2|1/
		  3\|/0
		 ---+---
		  4/|\7
		  /5|6\  
  	*/

	void draw_circle_octants(int x, int y, int x0, int y0)
	{
		
		PlotPixel(x+x0, y+y0);	//draw in 1st octant
		PlotPixel(x0-x, y0+y);	// draw in the 2nd octant
		PlotPixel(x+x0, y0-y);	// draw in the 6th octant
		PlotPixel(x0-x, y0-y);;	// draw in the 5th octant
		PlotPixel(y+x0, x+y0);	//	draw in the 0th octant
		PlotPixel(x0-y, x+y0);	//	draw in the 7th octant
		PlotPixel(y+x0, y0-x);	//	draw in the 3rd octant
		PlotPixel(x0-y, y0-x);	//	draw in the 4th octant
	}

	/* Bresenhamm's Circle drawing algorith based on the midpoint algorithm*/
	void drawCircle(int x0, int y0, int r)	// arguments-> centre x coordinate, centre y coordinate, radius
	{
		int x=0;
		int y=r;
		int d=1-r;	// intial decision variable
		int deltaE=3;	// second difference value when the East pixel is chosen
		int deltaSE=-2*r+5;	// second difference value when the South East pixel is chosen
		
		// Draw the points that lie on the four principle axis
		PlotPixel(x+x0,y+y0);
		PlotPixel(y+x0, x+y0);
		PlotPixel(x+x0,y0-y);
		PlotPixel(x0-y,x+y0);
		
		while(x<=y) // draw in the 1st octant till x is less than the y coordinate
		{	
			if(d<0) //Choose East pixel
			{
				d+=deltaE; // increment the decision variable by amount deltaE
				deltaSE+=2;	//update the second difference values
				deltaE+=2;	
			}
			else //Choose South East pixel
			{
				d+=deltaSE; // update the decision variable
				deltaSE+=4; // update the second difference values
				deltaE+=2;
				y--; 	// move vertically downward by one pixel
			}
			x++;	// move forward on the x axis by 1 pixel
			draw_circle_octants(x,y, x0, y0); // draw all the symmetrical points
		}
	}


	/* Function to plot the stars on the window background. It plots random points on the background 
	and modifies the color opacity of the pixel according to the height of that pixel */
	void drawStars(int W, int H, int count) 	// draw a total of 'count' number of stars(pixels)
	{
		int start = H;		// start from the topmost horizontal line
		while(count > 0)
		{
			int height = H - (rand() % static_cast<int>(H - 100 + 1));	//select a random height in the given range (H - (H-100))
			int x = (rand() % static_cast<int>(W));	//select a random x coordinte
			float intensity_factor = 1.0*float(height)/float(H);	// pixel intesity factor calculated according to the height 
			glColor3f(intensity_factor, intensity_factor, intensity_factor); //set the color
			PlotPixel(x, height);
			count--;
		}
	}

	/* Draw grass like structure on the bottom of the window with variable height */
	void drawGrass(int W)
	{
		glColor3f(0,0.4,0);
		int i=0;
		while(i<W)
		{
			int height = 5 + (rand() % static_cast<int>(25 - 5 + 1));
			drawLine(i,0, i+1, height);
			i++;
		}
	}

	/* Draw a sunset backdrop for the openGL window using variable color values based on the height */
	void drawSunset(int W, int H)
	{
		int j = 0;
		while(j <= H)
		{
			glColor3f(0.40,0.40*j/H,0);
			drawLine(0,j,W,j);
			j++;
		}
	}

	/* Draw a night Sky backdrop for the openGl window using variable color values based on the height */
	void drawNightSky(int W, int H)
	{
		int j = 0;
		while(j <= H)
		{
			glColor3f(0.02,0.02,.20*j/H);
			drawLine(0,j,W,j+1);
			j++;
		}
	}

	void fillCircle(int x, int y, int r){
		int i = r;
		while(i>0)
		{
			drawCircle(x,y,i);
			i--;
		}
	}
	
	/* Configure the OpenGL window */
	void glConfig(int W, int H)
	{
		const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
		const GLubyte* version = glGetString(GL_VERSION); // version as a string
		printf("Renderer: %s\n", renderer);
		printf("OpenGL version supported %s\n", version);

		glOrtho(0, W, 0, H, 0, 1); // specify the coordinate system and use the bottom-left point as (0,0) and (W,H) as the top right point of the window
		glColor3f(0,0,1);
		glfwSwapInterval(1); // Vertical Sync rate
	}

	void Stayawake(GLFWwindow* window)
	{
		while(!glfwWindowShouldClose(window)) // main loop for the window
	    {
	        glfwSwapBuffers(window);
	        glfwWaitEvents();
	        
	    }
	    glfwDestroyWindow(window); //Destroy the window and terminate the glfwInstance if the program exits the main loop
	    glfwTerminate();		   // The exit from the main loop is triggered when a user closes the window
	}
	void setColor(double r, double g, double b) //Helper function to set the color of pixels to draw using the r,g,b values
	{
		glColor3f(r,g,b);
	}

	void changePointSize(int psize){
		glPointSize(psize);
	}

	GLFWwindow* StartWindow(int _h, int _w) //initiate the OpenGL window
	{
	    if(glfwInit() == false) //check for glfw
	    {
	        fprintf(stderr, "Error initializing GLFW\n");
	        return NULL;
	    }

	    int H = _h; //height of the window
	    int W = _w; //width of the window
	    
	    //create a glfw window instance
	    GLFWwindow* window = glfwCreateWindow(W, H, "OpenGL Example", NULL, NULL);
	    if(!window)		//handle the error if the window instance could not be created and return
	    {
	        glfwTerminate();
	        fprintf(stderr, "Error while creating a window\n");
	        return NULL;
	    }

	    //set context
	    glfwMakeContextCurrent(window);

	    // configure gl window
	    glConfig(W,H);
		
		//return the final window object	    
	    return window;
	    
	}
};

