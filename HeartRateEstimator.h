
#ifndef WUESWU_HEARTRATEESTIMATOR_H
#define WUESWU_HEARTRATEESTIMATOR_H

#include <deque>
#include "AcquisitionContext.h"

class HeartRateEstimator;
class ContextProvider {
public:
  void subscribe(HeartRateEstimator *estimator) {
    this->estimator = estimator;
  }
  virtual void run() = 0;

protected:
  HeartRateEstimator *estimator;
};

class HeartRateEstimator {
public:
  HeartRateEstimator(ContextProvider &provider)
      : provider(provider)
  {}

  void init();

  void run();

  void stop() {
    std::cerr << "Estimator received shutdown signal." << std::endl;
    work = false;
  }

  void notify(AcquisitionContext context) {
    data.push_back(context);
  }

private:
  double estimate();
  volatile bool work = false;
  std::deque<AcquisitionContext> data;
  ContextProvider &provider;
};


#endif //WUESWU_HEARTRATEESTIMATOR_H
