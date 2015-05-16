#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string>
#include <string.h>// std::string, std::to_string
#include <assert.h>

#define ODROID1 "http://192.168.0.102:8080/?action=stream"
#define ODROID2 "http://192.168.0.106:8080/?action=stream"
#define ODROID3 "http://192.168.0.116:8080/?action=stream"
#define ODROID4 "http://192.168.0.115:8080/?action=stream"

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
int globalXToPrint=0;

struct Odroid {
	VideoCapture cap;
	Odroid* rightNeighbour;
	Odroid* leftNeighbour;
	int StartXCoordinate= 0;
	int EndXCoordinate=320;
	int leftOverlap =20;
	int rightOverlap=50;
	int currentX;
	int length = 320;
};

int main (int argc, const char * argv[])
{


	cout << "Hello Artyom" << endl << flush ;
    VideoCapture cap;
    Odroid *currentOdroid;
    Odroid *odroid1 = new Odroid();
    Odroid *odroid2 = new Odroid();
    Odroid *odroid3 = new Odroid();
    Odroid *odroid4 = new Odroid();
    odroid1->cap.open(ODROID1);
    odroid2->cap.open(ODROID2);
    odroid3->cap.open(ODROID3);
    odroid4->cap.open(ODROID4);
    odroid1->leftNeighbour=odroid1;
    odroid1->rightNeighbour=odroid2;
    odroid1->leftOverlap=0;
    odroid2->rightNeighbour=odroid3;
    odroid2->leftNeighbour=odroid1;
    odroid3->leftNeighbour=odroid2;
    odroid3->rightNeighbour=odroid4;
    odroid4->leftNeighbour=odroid3;
    //odroid4->rightNeighbour=odroid4;
    odroid4->rightOverlap = 0;

    Mat img;



    cap = odroid1->cap;
    currentOdroid = odroid1;

    string Pos = "";
    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    string posPoint = "";
    string posRect ="";
    int currentCamera= 1;
    while (true)
    {

    	if(true){


    		switch(currentCamera)
    		{
    		case 1:
    			globalX = odroid1->currentX;
    			if (odroid1->currentX > (odroid1->EndXCoordinate - odroid1->rightOverlap)){
        			currentCamera = 2;
        			cap = odroid2->cap;
        			currentOdroid = odroid2;
        			cout << "chagned to cam2" << endl;
    			}
    			break;
    		case 2:
    			globalX = odroid1->length + odroid2->currentX - odroid2->leftOverlap;
    			if(odroid2->currentX < (odroid2->StartXCoordinate + odroid2->leftOverlap)){
        			currentCamera = 1;
        			cap = odroid1->cap;
        			currentOdroid = odroid1;
        			cout << "chagned to cam1" << endl;
    			}
    			if(odroid2->currentX > (odroid2->EndXCoordinate - odroid2->rightOverlap)){
        			currentCamera = 3;
        			cap = odroid3->cap;
        			currentOdroid = odroid3;
        			cout << "chagned to cam3" << endl;
    			}

    			break;
    		case 3:
    			globalX = odroid1->length + odroid2->length  +  odroid3->currentX
    						- odroid3->leftOverlap;
    			if(odroid3->currentX < (odroid3->StartXCoordinate + odroid3->leftOverlap)){
        			currentCamera = 2;
        			cap = odroid2->cap;
        			currentOdroid = odroid2;
        			cout << "chagned to cam 2" << endl;
    			}
    			if(odroid3->currentX > (odroid3->EndXCoordinate - odroid3->rightOverlap)){
        			currentCamera = 4;
        			cap = odroid4->cap;
        			currentOdroid = odroid4;
        			cout << "chagned to cam 4" << endl;
    			}
    			break;
    		case 4:
    			globalX = odroid1->length + odroid2->length  +  odroid3->length
    					+ odroid4->currentX - odroid4->leftOverlap ;
    			if (odroid4->currentX < (odroid4->StartXCoordinate - odroid4->leftOverlap)){
        			currentCamera = 3;
        			cap = odroid3->cap;
        			currentOdroid = odroid3;
        			cout << "chagned to cam 3" << endl;
    			}

    			break;
    		default:
    			break;
    		}

    	}
        cap >> img;


        if (!img.data)
            continue;

        vector<Rect> found, found_filtered;

        hog.detectMultiScale(img, found, 0, Size(8,8), Size(32,32), 1.05, 2);


        size_t i, j;

        //for (i=0; i<1; i++)
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

			currentOdroid->currentX = r.x;

			string x = to_string(r.x);
			string y = to_string(r.y);
			if(abs(globalX- globalXToPrint)>5){
				globalXToPrint=globalX;
			}
			posRect = "Pos: x:" + to_string(globalXToPrint); //+ " y: " + y;

			rectangle(img, r.tl(), r.br(), cv::Scalar(0,255,0), 2);
        }



        CvFont font;
        double hScale=0.5;
        double vScale=0.5;
        int    lineWidth=1;
        cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);

        IplImage* img1 = new IplImage(img);

        char* p = new char[posRect.length()+1];

        memcpy(p, posRect.c_str(), posRect.length()+1);

        cvPutText(img1, p, cvPoint(10,230), &font, cvScalar(240,0,0));


        imshow("video capture", img);
        if (waitKey(1) >= 0)
            break;

    }


    return 0;
}

