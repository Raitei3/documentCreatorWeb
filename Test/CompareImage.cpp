
#include <cv.h>
#include <highgui.h>
#include <string.h>
#include <math.h>

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
//Mat cmp(img1.rows,img1.cols,img1.type());
/*for (int i = 0; i < img1.size().height;i++) {
  for(int j = 0; j < img1.size().width;j++){
    uchar* pixval = CV_MAT_ELEM(img1,uchar,i,j);
    cmp[i][j]=abs(img1[i][j]-img2[i][j]);
  }
}*/





cv::compare(img1 , img2  , result , cv::CMP_EQ );
//cv::imwrite(savePath,result);
cvtColor(result, gray, CV_BGR2GRAY);
cv::imwrite(savePath+"-binaire.png",gray);

/*unsigned char *input = (unsigned char*)(img1.data);
unsigned char *input2 = (unsigned char*)(img2.data);
unsigned char *input3 = (unsigned char*)(img1.data);

int r1,g1,b1,b2,r2,g2;
for(int i = 0;i < img1.rows ;i++){
        for(int j = 0;j < img1.cols ;j++){
            b1 = input[img1.step * i + j ] ;
            g1= input[img1.step * i + j + 1];
            r1 = input[img1.step * i + j + 2];
            b2 = input2[img1.step * i + j ] ;
            g2= input2[img1.step * i + j + 1];
            r2 = input2[img1.step * i + j + 2];
            input3[img1.step * i + j ] = abs(b1-b2);
            input3[img1.step * i + j + 1 ] = abs(g1-g2);
            input3[img1.step * i + j + 2 ] = abs(r1-r2);
        }
    }*/
    for (int i = 0; i < img1.size().height;i++) {
      for(int j = 0; j < img1.size().width;j++){
        img1.at<Vec3b>(i,j)=Vec3b(255,255,255)-((img1.at<Vec3b>(i,j)-img2.at<Vec3b>(i,j))*10);
      }
    }


cv::imwrite(savePath+"-soustract.png",img1);
//cv::imwrite(savePath+"-binaire.png",gray);

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
