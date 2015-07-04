#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>

#include <sys/stat.h> // ディレクトリ・ファイル名の確認
#include <sys/types.h> // ディレクトリ作成

using namespace std;
using namespace cv;

static void help()
{
  cout <<   "./facedetect --input=\"directory\" --output=\"directory\"\n\n"
            "During execution:\n\tHit any key to quit.\n"
            "\tUsing OpenCV version " << CV_VERSION << "\n" << endl;
}

void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip );

string cascadeName = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";

int main( int argc, const char** argv )
{
  CvCapture* capture = 0;
  Mat frame, frameCopy, image;
  string inputDir = "--input=";
  size_t inputDirLen = inputDir.length();
  bool isInputDir = false;
  string outputDir = "--output=";
  size_t outputDirLen = outputDir.length();
  bool isOutputDir = false;

  string inputName;
  bool tryflip = false;

  help();

  CascadeClassifier cascade,nestedCascade;
  double scale = 1;

  // 引数処理
  for( int i = 1; i < argc; i++ )
    {
	  cout << "Processing " << i << " " <<  argv[i] << endl;
	  if( inputDir.compare( 0, inputDirLen, argv[i], inputDirLen) == 0)
		{
		  inputDir.assign( argv[i] + inputDirLen);
		  cout << "input directory = " << inputDir << endl;
		  isInputDir = true;
		}
	  else if( outputDir.compare( 0, outputDirLen, argv[i], outputDirLen) == 0)
		{
		  outputDir.assign( argv[i] + outputDirLen);
		  cout << "output directory = " << outputDir << endl;
		  isOutputDir = true;
		}
	  else if( argv[i][0] == '-' )
        {
		  cerr << "WARNING: Unknown option %s" << argv[i] << endl;
        }
	  else
		inputName.assign( argv[i] );
    }

  // 入力画像ディレクトリの確認
  if(! isInputDir)
	{
	  cerr << "No input directory... " << endl;
	  help();
	  return -1;
	}
  else
	{
	  struct stat st;
	  if(stat(inputDir.c_str(), &st) != 0)
		{
		  cerr << "No such a directory " << inputDir << endl;
		  return -1;
		}
	}

  // 出力画像ディレクトリの確認
  if(! isOutputDir)
	{
	  cerr << "No output directory" << endl;
	  cout << "will make a directory \"output\"" << endl;
	  mkdir("output", 0755); // "outputディレクトリを作成"
	  outputDir.assign("output" + outputDirLen);
	}
  else
	{
	  struct stat st;
	  if(stat(outputDir.c_str(), &st) != 0)
		{
		  cerr << "No such a directory " << outputDir << endl;
		  cout << "will make a directory "  << outputDir << endl;
		  mkdir(outputDir.c_str(), 0755);
		}
	}

  if( !cascade.load( cascadeName ) )
    {
	  cerr << "ERROR: Could not load classifier cascade" << endl;
	  cout << "Please Check a cascade file is exist at " << 
		"\"" << cascadeName << "\"" << endl;
	  help();
	  return -1;
    }

  // TODO:inputディレクトリのファイル一覧を取得する

  string filename = "../src/test.jpg";
  Mat inputImage = imread(filename, 1);
  Mat grayImage, smallImg( cvRound (inputImage.rows/scale), cvRound(inputImage.cols/scale), CV_8UC1 );
  cvtColor(inputImage, grayImage,  CV_RGB2GRAY);
  vector<Rect> faces;
  resize( grayImage, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
  equalizeHist( smallImg, smallImg );
  cascade.detectMultiScale( smallImg, faces,
							1.1, 2, 0
							//|CV_HAAR_FIND_BIGGEST_OBJECT
							//|CV_HAAR_DO_ROUGH_SEARCH
							|CV_HAAR_SCALE_IMAGE
							,
							Size(30, 30) );
      // 矩形を描く
    for (vector<Rect>::iterator iter = faces.begin(); iter != faces.end(); iter ++) {
	  rectangle(inputImage, *iter, Scalar(255, 0, 0), 1);
    }
 
    // 表示
    imshow("out", inputImage);
    waitKey(0);

	return 0;

}

void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip )
{
    int i = 0;
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =  { CV_RGB(0,0,255),
        CV_RGB(0,128,255),
        CV_RGB(0,255,255),
        CV_RGB(0,255,0),
        CV_RGB(255,128,0),
        CV_RGB(255,255,0),
        CV_RGB(255,0,0),
        CV_RGB(255,0,255)} ;
    Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );

    cvtColor( img, gray, CV_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    t = (double)cvGetTickCount();
    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
        //|CV_HAAR_FIND_BIGGEST_OBJECT
        //|CV_HAAR_DO_ROUGH_SEARCH
        |CV_HAAR_SCALE_IMAGE
        ,
        Size(30, 30) );
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
                                 1.1, 2, 0
                                 //|CV_HAAR_FIND_BIGGEST_OBJECT
                                 //|CV_HAAR_DO_ROUGH_SEARCH
                                 |CV_HAAR_SCALE_IMAGE
                                 ,
                                 Size(30, 30) );
        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++ )
        {
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }
    t = (double)cvGetTickCount() - t;
    printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );
    for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )
    {
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r->width/r->height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
        else
            rectangle( img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),
                       cvPoint(cvRound((r->x + r->width-1)*scale), cvRound((r->y + r->height-1)*scale)),
                       color, 3, 8, 0);
        if( nestedCascade.empty() )
            continue;
        smallImgROI = smallImg(*r);
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
            //|CV_HAAR_FIND_BIGGEST_OBJECT
            //|CV_HAAR_DO_ROUGH_SEARCH
            //|CV_HAAR_DO_CANNY_PRUNING
            |CV_HAAR_SCALE_IMAGE
            ,
            Size(30, 30) );
        for( vector<Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++ )
        {
            center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
            center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
            radius = cvRound((nr->width + nr->height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
    }
    cv::imshow( "result", img );
}
