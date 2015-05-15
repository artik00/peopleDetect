#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string>
#include <string.h>// std::string, std::to_string
#include <assert.h>

#define ODROID1 "http://192.168.0.101:8080/?action=stream"
#define ODROID2 "http://192.168.0.106:8080/?action=stream"
#define ODROID3 "http://192.168.0.102:8080/?action=stream"
#define ODROID4 "http://192.168.0.116:8080/?action=stream"

using namespace std;
using namespace cv;


#if 0
int __(int, char**) {

    cv::VideoCapture vcap(1);
    cv::Mat image;
    //const std::string video = "http://192.168.1.105:8080/?action=stream";

    if(!vcap.open(video)) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    for(;;) {
        if(!vcap.read(image)) {
            std::cout << "No frame" << std::endl;
            cv::waitKey();
        }
        cv::imshow("Output Window", image);
        if(cv::waitKey(1) >= 0) break;
    }
}
#endif


#if 0
int main(int, char**) {

    cv::VideoCapture vcap;
    cv::Mat image;
    const std::string video = "http://192.168.1.105:8080/?action=stream";

    if(!vcap.open(video)) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    for(;;) {
        if(!vcap.read(image)) {
            std::cout << "No frame" << std::endl;
            cv::waitKey();
        }
        cv::imshow("Output Window", image);
        if(cv::waitKey(1) >= 0) break;
    }
}

#endif


int globalX = 0;

struct Odroid {
	VideoCapture cap;
	Odroid* rightNeighbour;
	Odroid* leftNeighbour;
	int StartXCoordinate= 0;
	int EndXCoordinate=320;
	int leftOverlap =20;
	int rightOverlap=20;
};

int main (int argc, const char * argv[])
{
	printf("Hello\n");
	cout << "Hello Artyom" << endl << flush ;
    VideoCapture cap;
    Odroid *odroid1 = new Odroid();
    Odroid *odroid2 = new Odroid();
    Odroid *odroid3 = new Odroid();
    Odroid *odroid4 = new Odroid();
    odroid1->cap.open(ODROID1);
   // odroid2->cap.open(ODROID2);
  //  odroid3->cap.open(ODROID3);
 //   odroid4->cap.open(ODROID4);
    odroid1->leftNeighbour=odroid1;
    odroid1->rightNeighbour=odroid2;
    odroid1->leftOverlap=0;
    odroid2->rightNeighbour=odroid3;
    odroid2->leftNeighbour=odroid1;
    odroid3->leftNeighbour=odroid2;
    odroid3->rightNeighbour=odroid4;
    odroid4->leftNeighbour=odroid3;
    odroid4->rightNeighbour=odroid4;
    odroid4->rightOverlap = 0;

    Mat img;
    cap = odroid1->cap;


    string Pos = "";
    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    string posPoint = "";
    string posRect ="";
    int currentCamera= 1;
    while (true)
    {

    	if(false){


    		switch(currentCamera)
    		{
    		case 1:
    			currentCamera = 2;
    			//cap = cap2;
    			cout << "chagned to cam2" << endl;
    			break;
    		case 2:
    			currentCamera = 3;
    			//cap = cap3;
    			cout << "chagned to cam3" << endl;
    			break;
    		case 3:
    			currentCamera = 4;
    			//cap = cap4;
    			cout << "chagned to cam4" << endl;
    			break;
    		default:
    			break;
    		}

    	}
        cap >> img;
      //  cap1 >> img2;

        if (!img.data)
            continue;

        vector<Rect> found, found_filtered;
        //vector<Point> found1, found_filtered1;
        hog.detectMultiScale(img, found, 0, Size(8,8), Size(32,32), 1.05, 2);
        //hog.detect(img, found1, 0, Size(8,8), Size(0,0));

        size_t i, j;

        /*
        for (i = 0 ; i < found1.size(); i++){

        	Point tempPoint = found1[i];

    	    Rect r ;
    	    if (tempPoint.x > 0 && tempPoint.y > 0) {
    	    r.x += tempPoint.x;
            r.y += tempPoint.y;

    	    r.width = 10;
    	    r.height = 10;
        	rectangle(img, r.tl(), r.br(), cv::Scalar(255,0,0), 2);
    	    string x = to_string(r.x);
    	    string y = to_string(r.y);
    	    posPoint = "Pos: x:" + x+ " y: " + y;
    	    }


        }
*/

        for (i=0; i<found.size(); i++)
        {
            Rect r = found[i];
            for (j=0; j<found.size(); j++)
                if (j!=i && (r & found[j])==r)
                    break;
            if (j==found.size())
                found_filtered.push_back(r);
        }
        for (i=0; i<found_filtered.size(); i++)
        {
	    Rect r = found_filtered[i];
            r.x += cvRound(r.width*0.1);
	    r.width = cvRound(r.width*0.8);
	    r.y += cvRound(r.height*0.06);
	    r.height = cvRound(r.height*0.9);

//	    string x = to_string(r.x);
	    string y = to_string(r.y);
	  //  posRect = "Pos: x:" + x+ " y: " + y;

	    rectangle(img, r.tl(), r.br(), cv::Scalar(0,255,0), 2);
        }


        int number  = 5;
        char text[255];
        sprintf(text, "Score %d", (int)number);

        CvFont font;
        double hScale=1.0;
        double vScale=1.0;
        int    lineWidth=1;
        cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);

        IplImage* img1 = new IplImage(img);

        char* p = new char[posRect.length()+1];

        memcpy(p, posRect.c_str(), posRect.length()+1);

        cvPutText(img1, p, cvPoint(200,400), &font, cvScalar(0,255,0));

        char* p2 = new char[posPoint.length()+1];
        memcpy(p2, posPoint.c_str(), posPoint.length()+1);
        cvPutText(img1, p2, cvPoint(200,430), &font, cvScalar(255,255,255));

        imshow("video capture", img);
        if (waitKey(1) >= 0)
            break;

    }

    //namedWindow("video capture", CV_WINDOW_AUTOSIZE);

    return 0;
}

