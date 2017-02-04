#include "BackgroundReconstruction.hpp"
//#include "ui_Backgroundreconstruction.h"

#include <opencv2/imgproc/imgproc.hpp>//cvtColor

#include "textinpainting.hpp"
#include "convertor.h"

static const int IMG_WIDTH = 500;
static const int IMG_HEIGHT = 680;

BackgroundReconstruction::BackgroundReconstruction(QObject *parent) :
    QObject(parent){}
    //ui(new Ui::BackgroundReconstruction){
    //ui->setupUi(this);


BackgroundReconstruction::~BackgroundReconstruction(){
    //delete ui;
}

void BackgroundReconstruction::setOriginalImage(const QImage &img){

    QImage small = img.scaled(IMG_WIDTH,IMG_HEIGHT,Qt::KeepAspectRatio, Qt::FastTransformation);
    _smallOriginalImg = Convertor::getCvMat(small);

    //ui->originalLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    //ui->originalLabel->setText(tr("original image"));
    //ui->originalLabel->setPixmap(QPixmap::fromImage(small));

  _originalImg = Convertor::getCvMat(img);
}

void BackgroundReconstruction::setBinarizedImage(const QImage &img){

    QImage small = img.scaled(IMG_WIDTH,IMG_HEIGHT,Qt::KeepAspectRatio, Qt::FastTransformation);

    cv::cvtColor(Convertor::getCvMat(small), _smallBinarizedImg, CV_RGB2GRAY);
    cv::cvtColor(Convertor::getCvMat(img), _binarizedImg, CV_RGB2GRAY);

    process(true);
}

void BackgroundReconstruction::process(bool preview){

    cv::Mat background;
    if(preview){ // We work only with the thumbnail
        background = TextInpainting::getBackground(_smallOriginalImg, _smallBinarizedImg, getMaxTextArea(), getMaxTextWidth(), getMaxTextHeight());
        _background = Convertor::getQImage(background);

      //  ui->background->setFrameStyle(QFrame::Panel | QFrame::Sunken);
      //  ui->background->setText(tr("estimated background"));
      //  ui->background->setPixmap(QPixmap::fromImage(_background.scaled(IMG_WIDTH,IMG_HEIGHT,Qt::KeepAspectRatio, Qt::FastTransformation)));

    } else { // With the whole image
        background = TextInpainting::getBackground(_originalImg, _binarizedImg, getMaxTextArea(), getMaxTextWidth(), getMaxTextHeight());
        _background = Convertor::getQImage(background);
    }


}

void BackgroundReconstruction::on_slider_valueChanged(int value){
    max_text_area = value;
    process(true);
}
