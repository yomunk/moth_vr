#include <iostream>
#include <fstream>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <zmq.hpp>
#include <sstream>
using namespace::cv;
using namespace::std;
Point mousepoints= Point(346, 344); //used to record mouse clicks. the contour center is sent relative to this point

void CallbackFunc(int event1, int x, int y, int flags, void* userdata) //function used for mouse clicks
{
	if( event1 == EVENT_LBUTTONDOWN) //if left button clicke
	{
	    mousepoints.x = x; //set the point coordinates to the x and y of the mouse
		mousepoints.y = y;
	}

}



class tracker
{
public:
Mat supremewindow; //used later for drawing, not used in computation
Mat imagewithROI; // image frame of video
Mat mothframe0; //original frame from video
Mat blurredimage; //blurred image
Mat thresholdedframe; //converted frame by threshold
Mat contouredframe; // contoured frame
Rect ROI; //used to define region of interest
vector<vector<Point> > contourvector; // vector of contour vectors used by findcontours
vector<Vec4i> hierarchy; //used by canny
RotatedRect Rectaroundcontour; //rectangle used to find center
int trackedcontour; //variable for storing largest contour
double trackedcontourarea; //variable for storing largest contour area
int framenumber; //frame number
int thresholdvalue; //starting threshold, adjusted
int originalthresh;
bool reached; //used to indicate if threshold has been found
bool badlastframe;
int ROIcenter[2]; //starting roi coords, region should be in shape
int windowsize; //roi size
int currentoffsetx;//x must take initial window dimensions, used for relating ROI to parent image
int currentoffsety; //y
int mode; //indicates whether the program is in its initial phase
bool wehavestartedgettingvalues; //if true, contour values have started being collected
int estimatedarea; //used to initially set which contour should be tracked

tracker(int thresh, int roix, int roiy, int squarewindowwidth, int mode, int estimatedcontourarea) //constructor
{
framenumber = 0; //set initial frame to zero
thresholdvalue = thresh; //starting threshold
originalthresh = thresholdvalue;
ROIcenter[0] = roix; //x, starting roi coords, region should be within bouts of original image or else will throw error
ROIcenter[1] = roiy; //y
badlastframe = false;//used when loop breaks out because no contour found
windowsize = squarewindowwidth/2; //roi size, ROI will be be 2* windowsize x 2* windowsize
currentoffsetx = roix-windowsize; //x,s take initial ROI center location, used for relating ROI to parent image
currentoffsety = roiy-windowsize; //y
mode = mode; //mode, two options: largest contour(0) or similiar size contour(1)
wehavestartedgettingvalues = false;
estimatedarea = estimatedcontourarea; //used to find initial contour that is similiar to estimate, program will later use similiarity test
}

double findthecontouryouwant(vector<vector<Point> > vector) //goes through contourvector, returns contour number, indicating contour that best fits mode
{
	double number = 0; //number of important contour
	double area = 0; //area of important contour
	int thisarea; //current area

	for(unsigned int i = 0; i < vector.size(); i++)
	{
		thisarea = contourArea(vector[i]);
		if(mode == 0 || framenumber == 1) //if mode is 0 or it is the first loop, program finds largest area. This can be changed if the contour area is known
		{
		if(thisarea > area)
		{
			number = i;
			area = thisarea;
		}
		}
		else if(mode == 1) //if mode is 0, tracks contour with area closest to last one tracked
		{
			int lastarea;
			if(wehavestartedgettingvalues) //if we have gotten a contour size, use
			{
			lastarea = trackedcontourarea;
			}
			else //use the estimate
			{ lastarea = estimatedarea;
			}
			int bestdiff = abs(area - lastarea);
			int diff = abs(thisarea-lastarea);

			if(diff < bestdiff)
			{
				number = i;
				area = thisarea;
	 		}
		}
	}
	return  number; //returns which contour in the vector you want
}
void imageprocesscentr() //does all the image computation
{
//        struct timespec time;
       timespec time;
       long ms;
       long msnow;
       double timediff;
clock_gettime(CLOCK_REALTIME,  &time);
ms =time.tv_nsec;//gets time in millisec, used to break if stuck and next frame coming





	int adjustedwindowsize = windowsize; //this can be adjusted each frame if no contours are seen
	bool iseethatspot = false; //do i see that spot on the moth?
	bool thresholdneedschange = false;
	int ticker = 0; //records how many loops have been conducted, used to decide whether to expand ROI or bring down threshold
	while(!iseethatspot)
	{
//cout<<"TICKER: "<<ticker<<endl;

/*
if(ticker > 50)
{
  break;
}*/
//cout<<"LOOPING"<<endl;
      clock_gettime(CLOCK_REALTIME, &time);
        timediff = (time.tv_nsec - ms)/1e9;
      // Check for overflow on long subtraction; if time difference is negative, add 1s.
        if (timediff < 0) timediff += 1;
        cout<<"Time difference: "<<timediff<<endl;
        if( timediff >0.01) //if its been 10 milliseconds, and we still dont have a contour, break, and try again next frame
        {
          cout << "Took too long." << endl;
          thresholdvalue = originalthresh;
          badlastframe = true;
          //msnow = 0;
          //ms = 0;
          return;

        }

iseethatspot = true; //note that the spot is seen, loop broken
	ticker++;
	contourvector.clear(); //clears vector for next frame
	hierarchy.clear(); //clears vector used for findcontour function
	//the following lines are used to avoid an ROI out of bounds error; it makes sure that the window will always fit
if(framenumber > 0 && !badlastframe)
    {
        if(ROIcenter[0] > mothframe0.cols - adjustedwindowsize)
	{
		ROIcenter[0] = mothframe0.cols - adjustedwindowsize;
	}
	else if(ROIcenter[0] < adjustedwindowsize)
	{
		ROIcenter[0] = adjustedwindowsize;
	}
	if(ROIcenter[1] > mothframe0.rows-adjustedwindowsize)
	{
		ROIcenter[1] = mothframe0.rows - adjustedwindowsize ;
	}
	else if(ROIcenter[1] < adjustedwindowsize)
	{
		ROIcenter[1] = adjustedwindowsize;
	}
	///////////////////////////////////////////////////
	currentoffsetx = ROIcenter[0]- adjustedwindowsize;  //upper left corner of ROI-x
	currentoffsety = ROIcenter[1] - adjustedwindowsize; //upper left corner of ROI-y

	ROI = Rect(ROIcenter[0]-adjustedwindowsize, ROIcenter[1]-adjustedwindowsize, 2 * adjustedwindowsize,2 * adjustedwindowsize); //create the region of interes


     imagewithROI = mothframe0(ROI); //set roi of mothframe0 to a new mat



    }
else
{
  imagewithROI=mothframe0;//if its the first frame, do not use ROI, you can and can change this. if you do, the center of the ROI will be what is set in the constructor
  currentoffsetx = 0;
  currentoffsety = 0;
badlastframe = false;
}

	GaussianBlur(imagewithROI,blurredimage, Size(21,21),0,0); //blurs image to aid in contour finding, size refers to kernel size
	threshold(blurredimage, thresholdedframe,thresholdvalue, 255, THRESH_BINARY); // thresholds image to create black and white
	Canny(thresholdedframe,contouredframe, 1,255, 3); //marks edjes of blurred image
	findContours(contouredframe, contourvector, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0)); //finds and stores contours


	if(contourvector.size() > 0) //if we see some contours
	{
	if(!wehavestartedgettingvalues)
	{
		wehavestartedgettingvalues = true; //CONGRATULATIONS! the first contour data has been collected!
	}
	trackedcontour = findthecontouryouwant(contourvector); //find the one with the biggest area

	trackedcontourarea = contourArea(contourvector[trackedcontour]);
	Rectaroundcontour = minAreaRect(contourvector[trackedcontour]); //finds rectangle to find center



	ROIcenter[0] = Rectaroundcontour.center.x + currentoffsetx; //finds new ROI center x coord
	ROIcenter[1] = Rectaroundcontour.center.y + currentoffsety; //finds new ROI center y coord

	//cout<<"contour is seen"<<endl;
	iseethatspot = true; //note that the spot is seen, loop broken
	if(contourvector.size() > 3) //in this case, 3 is used because it allows for an occaisonal disturbance without disprupting the current threshold
	{
		thresholdvalue++; //if we have more than one contour, make the threshold more exclusive
//iseethatspot = false; //we see too many spots!
	}



        }


	else //if not one contour is seen
	{
          iseethatspot = false;
	//cout<<"no contours are seen"<<endl;




                  	if(adjustedwindowsize < mothframe0.rows/2 - 50 && ticker <2) //as long as the ROI window can fit comfortable in the image
	{
adjustedwindowsize = adjustedwindowsize +50;  //increase the window size
			}
			else
			{
				thresholdvalue -= 5;
			}


 //                           }

	}





if(thresholdvalue < 2) //makes sure that threshold doesn't go negative
{
  thresholdvalue = 255;
}






	}



}
};










int main()
{
timespec programstarttime;

clock_gettime(CLOCK_REALTIME, &programstarttime);


  zmq::context_t context (1); //zeroMQ stuff
        zmq::socket_t publisher (context, ZMQ_PUB);
        publisher.bind("tcp://*:5556");
	 namedWindow("mainwindow", CV_WINDOW_AUTOSIZE); //names window
	namedWindow("contours", CV_WINDOW_AUTOSIZE); //names window
	setMouseCallback("mainwindow", CallbackFunc, NULL); //sets mouse callback function, defined earlier
        tracker program(180, 0, 0,210,1, 60); //creates tracker object
	VideoCapture cap(0); //camera

        // Firefly settings
        cap.set(CV_CAP_PROP_FPS, 60);
        cap.set(CV_CAP_PROP_BRIGHTNESS, 0);
        cap.set(CV_CAP_PROP_CONTRAST, 0);
        cap.set(CV_CAP_PROP_GAIN, 0);
        cap.set(CV_CAP_PROP_EXPOSURE, 20);
        //cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        //cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	if(!cap.isOpened()) //check to see if camera is found
	{
		cout<<"Camera not found."<<endl;
		return -1; //if not, exit
	}
try
{
cap>>program.mothframe0;//we had problems with reading the frame if this isnt in there

}
catch(...)
{
 cout<<"DANG! THAT FIRST FRAME WAS MESSED UP!!!"<<endl;
 cout<<"...in all seriousness, it did throw an exception."<<endl;
}



	while(true)
	{
cout<<program.framenumber<<endl;
        Mat original;
	bool shithitthefan = false;
        try
        {
        cap>>program.mothframe0;  //get latest frame
        }
        catch(...)
        {
          shithitthefan = true;
          cout<<"problem with frame,exception thrown"<<endl;
        }
        //cvtColor(original, program.mothframe0, CV_RGB2GRAY);
	//program.filename();
	//program.mothframe0 = imread(program.file);
        if(!shithitthefan)
        {
        program.imageprocesscentr(); //process frame

        //cout<<"Current Thresh: "<<program.thresholdvalue<<endl;
        //cout<<program.thresholdvalue<<endl;

	program.supremewindow = program.mothframe0; //again, used to draw on
			circle(program.supremewindow, Point(program.ROIcenter[0], program.ROIcenter[1]), 10, Scalar(0,80,0),3,8,0); //draw circle around center on supreme window,
	line(program.supremewindow, mousepoints, Point(program.ROIcenter[0], program.ROIcenter[1]),Scalar(0,80,0),3,8,0); //and a line from it to the relative point


ostringstream mousepointsstring;
mousepointsstring<<"ORIGIN:"<<mousepoints.x<< " , "<<mousepoints.y;
putText(program.supremewindow,mousepointsstring.str(), cvPoint(40,40), FONT_HERSHEY_COMPLEX_SMALL,0.8, cvScalar(200,0,0),1, CV_AA);
imshow("mainwindow", program.supremewindow); //displays window
imshow("contours", program.contouredframe); //displays window

        ostringstream picfilename;
        timespec current;
        clock_gettime(CLOCK_REALTIME, &current);
        int currentprogramtime = ((current.tv_sec + current.tv_nsec/1e9)-(programstarttime.tv_sec+programstarttime.tv_nsec/1e9))*1000;
        //cout<<currentprogramtime<<endl;
        picfilename<<"frames/"<<currentprogramtime<<".jpg";
        string filename = picfilename.str();
        imwrite(filename, program.supremewindow);
        }
	program.framenumber++; //increase frame number
	int SENDX = program.ROIcenter[0]-mousepoints.x; //sends the x and y coordinates of the largest contour center relative to the point selected by mouse
	int SENDY = program.ROIcenter[1]-mousepoints.y;
        zmq::message_t message_x(20);
        snprintf ((char *) message_x.data(), 20, "centroid_x %d", SENDX);
        publisher.send(message_x);

        zmq::message_t message_y(20);
        snprintf ((char *) message_y.data(), 20, "centroid_y %d", SENDY);
        publisher.send(message_y);
//cout<<mousepoints.x<<" , "<<mousepoints.y<<endl;
	if(waitKey(20) == 27) //required; escape to quit
	{
			break; //if esc key, break--REQUIRED FOR OPENCV TO SHOW IMAGES
	}
	}
return 0; //ALL DONE!
}



