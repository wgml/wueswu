
#ifndef WUESWU_ACQUISITIONCONTEXT_H
#define WUESWU_ACQUISITIONCONTEXT_H

#include <chrono>

/**
 * Struct collection context on which algorithm is executed.
 *
 * Create date:
 *    16/11/27
 * Last modification date:
 *    16/11/27
 *
 * @authors Anna Musiał, Wojciech Gumuła
 */
struct AcquisitionContext {
  std::chrono::microseconds timestamp;
  unsigned long pixelSum;
  unsigned long pixelCount;
};
#endif //WUESWU_ACQUISITIONCONTEXT_H
