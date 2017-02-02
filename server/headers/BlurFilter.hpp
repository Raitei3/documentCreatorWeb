#ifndef BLURFILTER_HPP
#define BLURFILTER_HPP

#include <QImage>
#include <opencv2/core/core.hpp>

enum class Function {LINEAR=0, LOG, PARABOLA, SINUS, ELLIPSE, HYPERBOLA};
enum class Mode {COMPLETE=0, AREA};
enum class Method {GAUSSIAN=0, MEDIAN, NORMAL};
enum class Area {UP=0, DOWN, CENTERED};

/*
  Apply blur to whole image
 */
extern  QImage blurFilter(const QImage &imgOriginal, Method method, int intensity);

/*
  Apply blur to specific image area
*/
extern  QImage blurFilter(const QImage &imgOriginal, Method method, int intensity, Function function, Area area=Area::UP, float coeff=1, int vertical=0, int horizontal=0, int radius=0); //Vertical : Position vertical of the area, Horizontal : Position Horizontal of the area

extern  QImage makePattern(const QImage &imgOriginal, Function function, Area area, float coeff1, int vertical, int horizontal, int radius = 10);
extern  QImage applyPattern(const QImage &imgOriginal, const QImage &pattern, Method method, int intensity);

/*
  Apply blur to whole image
*/
extern  cv::Mat blurFilter(const cv::Mat &imgOriginal, Method method, int intensity);

/*
  Apply blur to specific image area
*/
extern  cv::Mat blurFilter(const cv::Mat &imgOriginal, Method method, int intensity, Function function, Area area=Area::UP, float coeff=1, int vertical=0, int horizontal=0, int radius=0);

extern  cv::Mat makePattern(const cv::Mat &originalMat, Function function, Area area, float coeff1, int vertical, int horizontal, int radius = 10);
extern  cv::Mat applyPattern(const cv::Mat &originalMat, const cv::Mat &pattern, Method method, int intensity);

float getRadiusFourier(const QImage &original);
int searchFitFourier(const QImage &original, float radiusExample);


class  BlurFilter : public QObject
{
  Q_OBJECT

public :

  explicit BlurFilter(const QImage &original, Method method, int intensity, Mode mode=Mode::COMPLETE, Function function=Function::LINEAR, Area area=Area::UP, float coeff=1, int vertical=0, int horizontal=0, int radius=0, const QImage &pattern = QImage(), QObject *parent =0) :
    QObject(parent), _intensity(intensity), _method(method), _mode(mode), _function(function), _area(area), _coeff(coeff), _vertical(vertical), _horizontal(horizontal), _radius(radius), _original(original), _pattern(pattern)
{}

  static void calcSolutions(float a, float b, float discr, float &y1, float &y2);
  static void calculFunction(Function fct, int rows, int x, int y, int vertical, int horizontal, float coeff, int &yFunction, int &y2Function);
  static bool upperThan(Function fct, int rows, int x, int y, int vertical, int horizontal, float coeff);
  static bool isNearFunction(int x, int y, int rows, Function fct, int horizontal, int vertical, float coeff, int radius);

public slots :

  virtual QImage apply();

signals:

  void imageReady(const QImage &);

protected :
  const int _intensity;
  const Method _method;
  const Mode _mode;
  const Function _function;
  const Area _area;
  const float _coeff;
  const int _vertical;
  const int _horizontal;
  const int _radius;
  QImage _original;
  QImage _pattern;

};

#endif //BLURFILTER_HPP
