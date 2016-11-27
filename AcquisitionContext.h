
#ifndef WUESWU_ACQUISITIONCONTEXT_H
#define WUESWU_ACQUISITIONCONTEXT_H

#include <chrono>

struct AcquisitionContext {
  std::chrono::microseconds timestamp;
  unsigned long pixelSum;
  unsigned long pixelCount;
};
#endif //WUESWU_ACQUISITIONCONTEXT_H
