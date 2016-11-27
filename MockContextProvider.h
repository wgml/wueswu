
#ifndef WUESWU_MOCKCONTEXTPROVIDER_H
#define WUESWU_MOCKCONTEXTPROVIDER_H

#include <vector>
#include "HeartRateEstimator.h"

class MockContextProvider : public ContextProvider {
public:
  MockContextProvider(std::string filename, bool runInfinitely)
  : runInfinitely(runInfinitely)
  {
    loadFromFile(filename);
  }

  void loadFromFile(std::string filename);
  void run() override;

private:
  std::vector<AcquisitionContext> buffer;
  unsigned long us_delay;
  bool runInfinitely;
};

#endif //WUESWU_MOCKCONTEXTPROVIDER_H
