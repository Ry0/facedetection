#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <boost/filesystem.hpp> // ファイル一覧を取得
#include <boost/foreach.hpp>
#include <sys/stat.h> // ディレクトリ・ファイル名の確認
#include <sys/types.h> // ディレクトリ作成

using namespace std;
using namespace cv;
using namespace boost;

namespace fs = boost::filesystem;

static void help()
{
  cout <<   "./facedetect --input=\"directory\" --output=\"directory\"\n\n"
            "During execution:\n\tHit any key to quit.\n"
            "\tUsing OpenCV version " << CV_VERSION << "\n" << endl;
}

void detectAndSave(CascadeClassifier &cascade, string &srcfilename, string &outputpath);
string ExtractFileName(const string &path, bool without_extension);
bool isInImage(cv::Mat &img, cv::Rect &rect);
cv::Rect fixRectSize(cv::Mat &img, cv::Rect &rect);

string cascadeName = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";

class Argument
{
public:
  Argument(string name)
	:name_(name), prefix_name_(name), prefix_name_len_(name.length()), flag_(false){}
  ~Argument(){}
  int prefix_compare(char* argv){
	int result = prefix_name_.compare(0, prefix_name_len_, argv, prefix_name_len_);
	if(result == 0) { flag_ = true; }
	return result
  }
  void rm_prefix(){
	name_.assign(prefix_name_.c_str() + prefix_name_len_);
  }
private:
  string prefix_name_;
  stirng prefix_name_len_;
  stirng name_;
  bool flag_;
};


class ArgumentParser
{
public:
  ArgumentParser(std::vector<string> options):
	inputDir_(options[0]), outputDir_(options[1]){}
  ~ArgumentParser(){}
  perse(int argc, const char** argv){
	for( int i = 1; i < argc; i++ ){
	  if( inputDir_.prefix_compare( argv[i] ) == 0){
		inputDir_.rm_prefix();
	  }
	  else if( outputDir_.prefix_compare( argv[i] ) == 0){
		outputDir_.rm_prefix();
	  }else{
		cerr << "WARNING: Unknown option %s" << argv[i] << endl;
	  }
    }
  }
private:
  Argument inputDir_;
  Argument outputDir_;
};

int main( int argc, const char** argv )
{
  Mat frame, frameCopy, image;
  string inputDir = "--input=";
  size_t inputDirLen = inputDir.length();
  bool isInputDir = false;
  string outputDir = "--output=";
  size_t outputDirLen = outputDir.length();
  bool isOutputDir = false;
  CascadeClassifier cascade;
  string inputName;

  help();

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
	  return -1;
    }

  //inputディレクトリのファイル一覧を取得する
  std::vector<std::string> file_list;
  const fs::path path(inputDir);
  BOOST_FOREACH(const fs::path& p, std::make_pair(fs::directory_iterator(path),
												  fs::directory_iterator())) {
	if (!fs::is_directory(p))
	  file_list.push_back(p.string());
	std::cout << p.filename() << std::endl;
  }
  
  for(vector<string>::iterator iter = file_list.begin(); iter != file_list.end(); iter++){
	detectAndSave(cascade, *iter, outputDir);	
  }


  return 0;

}

// 顔検出と保存
void detectAndSave(CascadeClassifier &cascade, string &srcfilename, string &outputpath)
{
  Mat inputImage = imread(srcfilename, 1);
  Mat grayImage;
  cvtColor(inputImage, grayImage,  CV_RGB2GRAY);
  Mat eqalizedImage = inputImage;
  // equalizeHist( grayImage, eqalizedImage );
  vector<Rect> faces;

  cascade.detectMultiScale( eqalizedImage, faces,
							1.1, 1, 0, Size(1, 1) );
  // 切り出して保存
  int num = 1;
  for (vector<Rect>::iterator iter = faces.begin(); iter != faces.end(); iter ++) {
	string saveImgname = ExtractFileName(srcfilename, true);
	string saveImgpath = outputpath + "/" + saveImgname + "_" + to_string(num) + ".jpg";
	cout << saveImgpath << endl;
	num++;
	Rect newRect = Rect(iter->x - (iter->width)*0.3,
						iter->y - (iter->height)*0.3,
						(iter->width)*1.6,
						(iter->height)*1.6);
   
	cout << *iter << endl;
	cout << newRect << endl;
	Rect fixedRect = fixRectSize(inputImage, newRect);
	cout << fixedRect << endl;cout << endl;
	imwrite(saveImgpath, inputImage(fixedRect));
  }
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

// Rectの範囲が画像内かどうかチェック
bool isInImage(cv::Mat &img, cv::Rect &rect)
{
  if(rect.x < 0) return false;
  if(rect.y < 0) return false;
  if(rect.x + rect.width > img.cols) return false;
  if(rect.y + rect.height > img.rows) return false;
  return true;
}

// Rectの範囲が画像内になければ修正する
cv::Rect fixRectSize(cv::Mat &img, cv::Rect &rect)
{
  if(isInImage(img, rect)) return rect;
  cv::Rect fixedRect = rect;
  if(rect.x < 0) fixedRect.x = 0;
  if(rect.y < 0) fixedRect.y = 0;
  if(fixedRect.x + rect.width > img.cols) fixedRect.width = img.cols - fixedRect.x;
  if(fixedRect.y + rect.height > img.rows) fixedRect.height = img.rows - fixedRect.y;
  return fixedRect;
}
