
#include <cv.h>
#include <highgui.h>
#include <string.h>

using namespace cv;
using namespace std;

float CompareImage(string image1 , string image2, string savePath)
{
  Mat gray1,gray2;
cv::Mat img1 = cv::imread(image1);
cv::Mat img2 = cv::imread(image2);
if (img2.size()!=img1.size()){
  cerr<< "Taille d'image diférente \n";
  return -1;
}
Size size=img1.size();
float nbPixels = size.height*size.width;

Mat result,gray;
cv::compare(img1 , img2  , result , cv::CMP_EQ );
cv::imwrite(savePath,result);
cvtColor(result, gray, CV_BGR2GRAY);
float similarPixels  = countNonZero(gray);
float res = (similarPixels/nbPixels)*100;
std::cout << "corresponding to "<<res<<"%" << std::endl;
}


int main(int argc,char* argv[])
{
  if (!((argc == 3)||(argc==4))) {
    std::cout << "/* ./CompareImage Image1 Image2 */" << std::endl;
  }
  string s1 = argv[1];
  string s2 = argv[2];
  if (argc == 4) {
    CompareImage(s1,s2,argv[3]);
  }
  else
  CompareImage(s1,s2,"images/resultCompareImage.png");
return 1;
}
