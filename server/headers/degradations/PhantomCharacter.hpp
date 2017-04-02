#ifndef PHANTOMCHARACTER_HPP
#define PHANTOMCHARACTER_HPP

#include <opencv2/core/core.hpp>

/*
  @param frequency : must be 0, 1, or 2.

  O is 15% probability of occurrence of phantom character.
  1 is 40%
  2 is 70%
*/
extern cv::Mat phantomCharacter(const cv::Mat &imgOriginal, int frequency);


#endif //PHANTOMCHARACTER_HPP
