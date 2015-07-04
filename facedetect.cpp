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


string ExtractFileName(const string &path, bool without_extension);

string cascadeName = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";

int main( int argc, const char** argv )
{
  Mat frame, frameCopy, image;
  string inputDir = "--input=";
  size_t inputDirLen = inputDir.length();
  bool isInputDir = false;
  string outputDir = "--output=";
  size_t outputDirLen = outputDir.length();
  bool isOutputDir = false;

  string inputName;

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
  Mat grayImage;
  cvtColor(inputImage, grayImage,  CV_RGB2GRAY);
  Mat eqalizedImage = inputImage;
  equalizeHist( grayImage, eqalizedImage );
  vector<Rect> faces;
  cascade.detectMultiScale( eqalizedImage, faces,
							1.1, 2, 0
							//|CV_HAAR_FIND_BIGGEST_OBJECT
							//|CV_HAAR_DO_ROUGH_SEARCH
							|CV_HAAR_SCALE_IMAGE
							,
							Size(30, 30) );
  // 切り出して保存
  int num = 1;
  for (vector<Rect>::iterator iter = faces.begin(); iter != faces.end(); iter ++) {
	string saveImgname = ExtractFileName(filename, true);
	string saveImgpath = outputDir + "/" + saveImgname + "_" + to_string(num) + ".jpg";
	cout << saveImgpath << endl;
	num++;
	Rect newRect = Rect(iter->x - (iter->width)*0.5,
						iter->y - (iter->height)*0.5,
						(iter->width)*1.5,
						(iter->height)*1.5);
	imwrite(saveImgpath, inputImage(newRect));
  }
  num = 0;

  return 0;

}



// パスからファイル名を抽出（拡張子を除くフラグ付き）
// http://www.slis.tsukuba.ac.jp/~fujisawa.makoto.fu/cgi-bin/wiki/index.php?string%A4%CB%A4%E8%A4%EB%CA%B8%BB%FA%CE%F3%BD%E8%CD%FD#w175146e
string ExtractFileName(const string &path, bool without_extension = true)
{
    string fn;
    string::size_type fpos;
    if((fpos = path.find_last_of("/")) != string::npos){
        fn = path.substr(fpos+1);
    }
    else if((fpos = path.find_last_of("\\")) != string::npos){
		fn = path.substr(fpos+1);
	}
	else{
		fn = path;
	}
 
	if(without_extension && (fpos = fn.find_last_of(".")) != string::npos){
		fn = fn.substr(0, fpos);
	}
 
	return fn;
}
