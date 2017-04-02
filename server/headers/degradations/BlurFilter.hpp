#ifndef BLURFILTER_HPP
#define BLURFILTER_HPP

#include <opencv2/core/core.hpp>

enum class Function {LINEAR=0, LOG, PARABOLA, SINUS, ELLIPSE, HYPERBOLA};
enum class Mode {COMPLETE=0, AREA};
enum class Method {GAUSSIAN=0, MEDIAN, NORMAL};
enum class Area {UP=0, DOWN, CENTERED};

extern cv::Mat blurFilter(const cv::Mat &original, Method method, int intensity, Mode mode=Mode::COMPLETE, Function function=Function::LINEAR, Area area=Area::UP, float coeff=1, int vertical=0, int horizontal=0, int radius=0); 
extern cv::Mat makePattern(const cv::Mat &original, Function function, Area area, float coeff1, int vertical, int horizontal, int radius = 10);
extern cv::Mat applyPattern(const cv::Mat &original, const cv::Mat &pattern, Method method, int intensity);

extern float getRadiusFourier(const cv::Mat &original);
extern int searchFitFourier(const cv::Mat &original, float radiusExample);


#endif //BLURFILTER_HPP
