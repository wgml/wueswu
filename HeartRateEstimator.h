
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

  /**
   * Subscribe estimator to given provider. Estimator is then being notified
   * every time new context is acquired.
   * Create date:
   *    16/11/27
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   * @param estimator Estimator to notify on every new context.
   */
  void subscribe(HeartRateEstimator *estimator) {
    this->estimator = estimator;
  }

  /**
   * Abstract execution method, should be implemented as infinite loop.
   * If present, estimator should be notified of any new context acquired.
   *
   * Create date:
   *    16/11/27
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   */
  virtual void run() = 0;

  virtual ~ContextProvider() = default;

protected:
  HeartRateEstimator *estimator = nullptr;
};

class HeartRateEstimator {
public:
  /**
   * Construct estimate algorithm for given context provider.
   * Estimate window frequencies can be also specified.
   *
   * Create date:
   *    16/11/27
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   *
   * @param provider data provider for algorithm.
   * @param min_freq Optional min frequency for estimate.
   * @param max_freq Optional max frequency for estimate.
   */
  HeartRateEstimator(std::shared_ptr<ContextProvider> provider, double min_freq = 0.66,
                     double max_freq = 2.66)
      : provider(provider), min_freq(min_freq), max_freq(max_freq) {}

  /**
   * Method performing algorithm initialization.
   * It should also subscribe estimator into context provider.
   *
   * Create date:
   *    16/11/27
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   */
  void init();

  /**
   * Main loop for estimator. Tps can be specified.
   * Performs estimate procedure on current context state.
   * Result is send to standard output.
   *
   * Create date:
   *    16/11/27
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   *
   * @param tps Optional tps defining algoritm execution frequency
   */
  void run(double tps = 1.0);

  /**
   * Method used to abort execution and peaceful shutdown procedure.
   *
   * Create date:
   *    16/11/27
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   */
  void stop() {
    std::cerr << "Estimator received shutdown signal." << std::endl;
    work = false;
  }

  /**
   * Notify method used for context acquisition.
   * Data points from provider should be passed via call to this method.
   * Thread-safe.
   *
   * Create date:
   *    16/11/27
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   *
   * @param context Acquired image context.
   */
  void notify(AcquisitionContext context) {
    std::lock_guard<std::mutex> lock(data_mutex);
    while (data.size() >= WINDOW_SIZE)
      data.pop_front();
    data.push_back(context);
  }

private:

  /**
   * Window size parameter. It should be moved into template argument in future.
   */
  static const size_t WINDOW_SIZE = 1980;

  using data_t = std::array<double, WINDOW_SIZE>;

  double estimate();

  double get_raw_data(data_t &);

  void normalize_data(const data_t &, data_t &);

  void kalman_data(const data_t &, data_t &);

  void filter_data(const data_t &, data_t &, double);

  void fft_data(const data_t &, data_t &, data_t &, const double);

  double determine_result(const data_t &, const data_t &);

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
