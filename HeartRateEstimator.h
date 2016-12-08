
#ifndef WUESWU_HEARTRATEESTIMATOR_H
#define WUESWU_HEARTRATEESTIMATOR_H

#include <deque>
#include <mutex>
#include "AcquisitionContext.h"

#define _USE_MATH_DEFINES
#include <cassert>

class HeartRateEstimator;
class ContextProvider {
public:
  void subscribe(HeartRateEstimator *estimator) {
    this->estimator = estimator;
  }

  void unsubscribe(HeartRateEstimator *estimator)
  {
    assert(this->estimator == estimator);
    this->estimator = nullptr;
  }

  virtual void run() = 0;

  virtual ~ContextProvider() = default;

protected:
  HeartRateEstimator *estimator = nullptr;
};

class HeartRateEstimator {
public:
  HeartRateEstimator(std::shared_ptr<ContextProvider> provider)
      : provider(provider)
  {}

  void init();

  void run(double = 1.0);

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
  static const size_t WINDOW_SIZE = 256;
  using data_t = std::array<double, WINDOW_SIZE>;

  double estimate();
  double get_raw_data(data_t&);
  void filter_data(data_t&, data_t&);
  void fft_data(data_t&, data_t&, data_t&, const double);
  double determine_result(data_t&, data_t&);

  volatile bool work = false;

  std::deque<AcquisitionContext> data;
  std::mutex data_mutex;
  std::shared_ptr<ContextProvider> provider;
};


#endif //WUESWU_HEARTRATEESTIMATOR_H
