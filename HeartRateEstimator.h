
#ifndef WUESWU_HEARTRATEESTIMATOR_H
#define WUESWU_HEARTRATEESTIMATOR_H

#include <deque>
#include <mutex>
#include <memory>
#include <cassert>
#include "FirFilter.h"
#include "AcquisitionContext.h"
#include "ResultAverage.h"

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
  HeartRateEstimator(std::shared_ptr<ContextProvider> provider, double min_freq = 0.66, double max_freq = 2.66)
    : provider(provider)
    , min_freq(min_freq)
    , max_freq(max_freq)
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
  static const size_t WINDOW_SIZE = 1024;

  using data_t = std::array<double, WINDOW_SIZE>;

  double estimate();

  double get_raw_data(data_t&);
  void normalize_data(const data_t&, data_t&);
  void kalman_data(const data_t&, data_t&);
  void filter_data(const data_t&, data_t&, double);
  void fft_data(const data_t&, data_t&, data_t&, const double);
  double determine_result(const data_t&, const data_t&);

  bool is_valid(double estimate) {
    return estimate >= min_freq * 60 && estimate <= max_freq * 60;
  }

  volatile bool work = false;

  std::deque<AcquisitionContext> data;
  std::mutex data_mutex;

  ResultAverage<double, 30> average;

  std::shared_ptr<ContextProvider> provider;

  double min_freq;
  double max_freq;

};


#endif //WUESWU_HEARTRATEESTIMATOR_H
