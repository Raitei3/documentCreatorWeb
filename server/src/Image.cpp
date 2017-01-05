#include "../headers/Image.hpp"
#include "../headers/binarization.hpp"

Image::Image(const std::string &path){
  m_img = cv::imread(path, CV_LOAD_IMAGE_COLOR);
  if(! m_img.data )                              // Check for invalid input
  {
    std::cout <<  "Could not open or find the image" << std::endl ;
  }
}

cv::Mat Image::getMat() const
{
  return m_img;
}

void Image::setMat(const cv::Mat &mat)
{
  m_img = mat;
}

cv::Rect Image::getBoundingBoxAtIndex(int index,int line) const
{
  std::vector<ConnectedComponent> ListTmpCC = m_listLine[line].getListCC();
  return ListTmpCC[index].getBoundingBox();
}

int Image::getBaselineAtIndex(int index,int line)
{
  std::vector<ConnectedComponent> ListTmpCC = m_listLine[line].getListCC();
  return ListTmpCC[index].getBaseline();
}

void Image::setBoundingBoxAtIndex(int index,int line, int up, int down, int left, int right)
{
  m_listLine[line].setBoundingBoxAtIndex(index, up, down, left, right);
}

void Image::setBaselineAtIndex(int index,int line, int value)
{
  m_listLine[line].setBaselineAtIndex(index,value);
}

ConnectedComponent Image::getConnectedComponnentAt(int index, int line) const{
  return m_listLine[line].getConnectedComponentAtIndex(index);
}

cv::Mat Image::binarizeImage() const
{
  cv::Mat m_img_bin (m_img.rows, m_img.cols, CV_8U);
  Binarization::NiblackSauvolaWolfJolion(m_img, m_img_bin, Binarization::WOLFJOLION);
  return m_img_bin;
}

cv::vector<ConnectedComponent> Image::extractComposentConnectImage(cv::Mat img){

  if(img.channels()>1){
    cv::cvtColor(img,img,CV_RGB2GRAY);
  }

  bitwise_not(img,img);
  cv::vector<ConnectedComponent> tmpCC;
  cv::vector<cv::vector<cv::Point>> contours;
  cv::findContours(img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
  tmpCC.reserve(contours.size());
  for(size_t i=0; i<contours.size(); i++){
    ConnectedComponent cc = ConnectedComponent(contours[i]);
    tmpCC.push_back(cc);
  }
  return tmpCC;
}

int Image::getCharacterHeight(const cv::Mat &img){
    /* Returns the median height of a character in a given binary image */
if(img.empty())
  return false;
else if(img.channels()>1){
  cv::cvtColor(img,img,CV_RGB2GRAY);
}
    // Smooth the image to remove unwanted noise from extracted CCs
cv::medianBlur(img,img,7);
bitwise_not(img,img);

    // CCs extraction
cv::vector<cv::vector<cv::Point> > contours;
cv::findContours(img, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_KCOS);
    // We sort the array with respect to their height
sort(contours.begin(), contours.end(), [](const cv::vector<cv::Point>& c1, const cv::vector<cv::Point>& c2){
  return cv::boundingRect(c1).height < cv::boundingRect(c2).height;
});


    // Merely return the median's height
int median = (int)contours.size()/2;
return cv::boundingRect(contours[median]).height; 
}

void Image::ComputeMask()
{  
  /* Create the mask for had each line and add each ConnectedComponent to a line */
  cv::Mat binarize; 
  binarizeImage().copyTo(binarize);
  int character_height = getCharacterHeight(binarize.clone());

  cv::Mat tmp;

  // horizontal blur
  cv::blur(binarize,tmp,cv::Size(5*character_height,0.5*character_height));
  // binarization + median filtering
  cv::threshold(tmp,tmp,190,255,1);
  if(tmp.empty())
    return;
  else if(tmp.channels()>1){
    cv::cvtColor(tmp,tmp,CV_RGB2GRAY);
  }
  cv::medianBlur(tmp,tmp,9);

  cv::vector<cv::vector<cv::Point>> contours_mask;
  // CCs extraction of mask 
  cv::findContours(tmp, contours_mask, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_KCOS);
  // CCs extraction of image
  cv::vector<ConnectedComponent> tmpCC = extractComposentConnectImage(binarize.clone());
  //We sort the CCs with respect to their position in the image
  sort(tmpCC.begin(), tmpCC.end(), [](const ConnectedComponent c1,const ConnectedComponent c2){
    return cv::boundingRect(c1.getListPoint()).x < cv::boundingRect(c2.getListPoint()).x;
  });

  //We sort the array with respect to their y 
  sort(contours_mask.begin(), contours_mask.end(), [](const cv::vector<cv::Point>& c1, const cv::vector<cv::Point>& c2){
    return cv::boundingRect(c1).y < cv::boundingRect(c2).y;
  });

  size_t nbCCInLine = 0;

  if(! contours_mask.empty())
  {
    for(size_t i=0 ; i<contours_mask.size(); ++i)
    {
      Line line = Line();
      cv::Rect rMask = cv::boundingRect(contours_mask[i]);
      for(size_t k=0; k < tmpCC.size(); ++k)
      {
        cv::Rect r = cv::boundingRect(tmpCC[k].getListPoint());
        if(tmpCC[k].getInline() == false)
        {
          // Verify if CC[k] is in Line[i]
          if(inMiddle(rMask,r) || rMask.contains(r.tl()) || rMask.contains(r.br()))
          {
            line.addCC(tmpCC[k]);
            tmpCC[k].setInline(true);
            ++nbCCInLine;
          }
        }  
      }  
      m_listLine.push_back(line);
      if(m_listLine[i].getListCC().size() != 0)
      {
        m_listLine[i].computeBaseLine();
      }     
    }
    // No Line founded and Connected Component exist
  } else if(tmpCC.size() == 1){
    Line line = Line();
    line.addCC(tmpCC[0]);
    ++nbCCInLine;
    m_listLine.push_back(line);
    m_listLine[0].computeBaseLine();
  }

  // All connected component not added to a line of mask, trash line
  if(nbCCInLine != tmpCC.size())
  {
    Line line = Line();
    for(size_t i = 0; i < tmpCC.size() ; i++){
      if(tmpCC[i].getInline() == false)
        line.addCC(tmpCC[i]);
    }
    m_listLine.push_back(line);
  }

  contours_mask.clear();
  tmpCC.clear();

  tmp.release();
  binarize.release();
}


std::string Image::jsonBoundingRect(){
  std::string json;
  int id = 0;
  for(size_t line=0 ; line < m_listLine.size(); ++line)
  {
    std::vector<ConnectedComponent> ListTmpCC = m_listLine[line].getListCC();
    for(size_t i=0; i < ListTmpCC.size(); ++i)
    {
      cv::Rect rect = cv::boundingRect(ListTmpCC[i].getListPoint());
      json += ("\""+ std::to_string(id) +"\":{");
      json += ("\"idCC\":" + std::to_string(i) + ",");
      json += ("\"idLine\":" + std::to_string(line) + ",");
      json += ("\"x\":" + std::to_string(rect.x) + ",");
      json += ("\"y\":" + std::to_string(rect.y) + ",");
      json += ("\"width\":" + std::to_string(rect.width) + ",");
      json += ("\"height\":" + std::to_string(rect.height));
      json += ("}");
      json += (",");
      id++;
    }
    ListTmpCC.clear();
  }
  
  json = json.substr(0, json.size()-1);
  return json;
}

std::string Image::jsonBaseline(){
  std::string json;
  for(size_t line = 0 ; line < m_listLine.size(); ++line)
  {
    if(m_listLine[line].getListCC().size() != 0)
    {
      std::vector<ConnectedComponent> ListTmpCC = m_listLine[line].getListCC();
      cv::Rect cc_begin = cv::boundingRect(ListTmpCC[0].getListPoint());
      cv::Rect cc_end = cv::boundingRect(ListTmpCC[ListTmpCC.size()-1].getListPoint());
      json += ("\"" + std::to_string(line) +"\":{");
      json += ("\"idLine\":" + std::to_string(line) + ",");
      json += ("\"x_begin\":" + std::to_string(cc_begin.x) + ",");
      json += ("\"x_end\":" + std::to_string(cc_end.x + cc_end.width) + ",");
      json += ("\"y_baseline\":" + std::to_string(m_listLine[line].getBaseline()));
      json += ("}");
      json += (",");
      ListTmpCC.clear();
    }
  }
  json = json.substr(0, json.size()-1);
  return json;
}


std::string Image::extractDataFromComponent(int index, int lineId) const
{

  // Generate binarized image
  cv::Mat imgBinarized; 
  imgBinarized = binarizeImage().clone();

  std::string data;

  // Get coordonate of the component
  cv::Rect bb = getBoundingBoxAtIndex(index, lineId);

  // Apply a mask to erase the background and resize to the component size
  cv::Mat mask = imgBinarized(bb);
  cv::threshold(mask, mask, 20, 1, cv::THRESH_BINARY_INV); 
  cv::Mat letter; 
  letter = cv::Mat::ones(mask.rows,mask.cols,m_img.type());
  letter.setTo(cv::Scalar(255,255,255));
  m_img(bb).copyTo(letter, mask);

  for (int i=0; i<letter.rows; ++i) {
    // get the row i
    cv::Vec3b *p = letter.ptr<cv::Vec3b>(i);
    for (int j=0; j<letter.cols; ++j) {
      // get the col j
      const cv::Vec3b pix = p[j];
      unsigned char opacity;
      // if color is white make it transparent
      if(pix[0] == 255 && pix[1] == 255 && pix[2] == 255)
        opacity = 0;
      else
        opacity = 255;

      // transforme RGB to ARGB
      unsigned int v = (opacity<<24)|(pix[0]<<16)|(pix[1]<<8)|(pix[2]);
      data += std::to_string(v);
      data += ",";
    }
  }
  mask.release();
  letter.release();
  imgBinarized.release();
  return data.substr(0, data.size()-1);
} 

bool Image::inMiddle(const cv::Rect bb1, const cv::Rect bb2) const
{
  int bb2_x = bb2.x + (bb2.width/2);
  int bb2_y = bb2.y + (bb2.height/2);
  int bb1_x_max = bb1.x + bb1.width + 3;
  int bb1_y_max = bb1.y + bb1.height + 3;
  if((bb1.x) <= bb2_x && bb2_x <= bb1_x_max && (bb1.y) <= bb2_y &&  bb2_y <= bb1_y_max)
  {
    return true;
  }
  return false;
}

void Image::setBaselineForLine(int id, int value)
{
  m_listLine[id].setBaseline(value);
}

int Image::isValidIdLine(int line) const{
  if (line >=0 && size_t(line) < m_listLine.size())
    return line;
  return -1;
}

int Image::isValidIdCC(int line, int cc) const{
  if(isValidIdLine(line) != -1) {
    const size_t sz = m_listLine[line].getListCC().size();
    if (cc>=0 && size_t(cc)<sz)
      return cc;
  }
  return -1;
}

std::vector<Line> Image::getListLine(){
  return m_listLine;
}

void Image::removeConnectedComponentAt(int index, int line){
  m_listLine[line].removeConnectedComponentAt(index);
}
