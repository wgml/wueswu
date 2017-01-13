
#ifndef WUESWU_HEARTRATEESTIMATOR_H
#define WUESWU_HEARTRATEESTIMATOR_H

#include <vector>
#include <deque>
#include <mutex>
#include <memory>
#include <cassert>
#include "FirFilter.h"
#include "AcquisitionContext.h"
#include "ResultAverage.h"
#include "Config.h"

class HeartRateEstimator;

/**
 * Abstract provided class. Implementations should override run method.
 * Create date:
 *    16/11/27
 * Last modification date:
 *    16/12/25
 *
 * @authors Anna Musiał, Wojciech Gumuła
 */

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

/**
 * Estimator class, described in a detailed way in the report.
 * Create date:
 *    16/11/27
 * Last modification date:
 *    16/12/25
 *
 * @authors Anna Musiał, Wojciech Gumuła
 */
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
   * @param config Configuration structure.
   */
  HeartRateEstimator(std::shared_ptr<ContextProvider> provider, const Config &config)
      : provider(provider)
      , average(config)
      , configuration(config)
  {}

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
   */
  void run();

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
    while (data.size() >= window_size())
      data.pop_front();
    data.push_back(context);
  }

private:

  using data_t = std::vector<double>;

  double estimate();

  double get_raw_data(data_t &);

  void normalize_data(const data_t &, data_t &);

  void kalman_data(const data_t &, data_t &);

  void filter_data(const data_t &, data_t &, double);

  void fft_data(const data_t &, data_t &, data_t &, const double);

  double determine_result(const data_t &, const data_t &);

  bool is_valid(double estimate) {
    return estimate >= configuration.estimator.min_freq * 60
           && estimate <= configuration.estimator.max_freq * 60;
  }

  size_t window_size() {
    return static_cast<size_t>(configuration.estimator.window_length * configuration.camera.fps);
  }

  volatile bool work = false;

  std::deque<AcquisitionContext> data;
  std::mutex data_mutex;

  ResultAverage<double> average;

  std::shared_ptr<ContextProvider> provider;
  const Config configuration;
};

#endif //WUESWU_HEARTRATEESTIMATOR_H
