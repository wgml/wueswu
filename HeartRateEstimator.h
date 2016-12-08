
#ifndef WUESWU_HEARTRATEESTIMATOR_H
#define WUESWU_HEARTRATEESTIMATOR_H

#include <deque>
#include <mutex>
#include "AcquisitionContext.h"

class HeartRateEstimator;
class ContextProvider {
public:
  void subscribe(HeartRateEstimator *estimator) {
    this->estimator = estimator;
  }
  virtual void run() = 0;

  virtual ~ContextProvider() = default;

protected:
  HeartRateEstimator *estimator = nullptr;
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
    std::lock_guard<std::mutex> lock(data_mutex);
    while (data.size() >= WINDOW_SIZE)
      data.pop_front();
    data.push_back(context);
  }

private:
  double estimate();
  void get_raw_data(double *);
  void filter_data(double*, double*);
  void fft_data(double*, double*, double*, const double);
  double determine_result(double*, double*);

  const size_t WINDOW_SIZE = 512;
  volatile bool work = false;

  std::deque<AcquisitionContext> data;
  std::mutex data_mutex;
  ContextProvider &provider;
};


#endif //WUESWU_HEARTRATEESTIMATOR_H
