
#ifndef WUESWU_RESULTAVERAGE_H
#define WUESWU_RESULTAVERAGE_H

#include <deque>
#include <numeric>
#include "Config.h"

/**
 * Can be used to average result of some data points for given type T.
 * @tparam T data type
 * @tparam Count Number of data points to average on.
 */
template<typename T>
class ResultAverage {
public:
  ResultAverage(const Config &configuration)
      : configuration(configuration) {}

  /**
   * Struct containing information about computed average - value and number
   * of samples used to perform estimate.
   */
  struct AverageResult {
    T value;
    size_t num;
  };

  AverageResult average(T new_value) {
    if (values.size() == configuration.estimator.average_window)
      values.pop_front();
    values.push_back(new_value);

    if (configuration.estimator.filtered_average)
      return filtered_average(values);
    else
      return arithm_average(values);
  }

private:
  AverageResult arithm_average(const std::deque<T> &values) const {
    T sum = std::accumulate(values.begin(), values.end(), T{});
    size_t num = values.size();
    return AverageResult{sum / num, num};
  }

  AverageResult filtered_average(const std::deque<T> &values) const {
    if (configuration.estimator.filtered_average_coeff >= 0.5)
      throw std::exception{};
    size_t num = values.size();
    std::vector<T> values_vec(num);
    std::copy(values.begin(), values.end(), values_vec.begin());
    std::sort(values_vec.begin(), values_vec.end());
    size_t threshold = static_cast<size_t>(num * configuration.estimator.filtered_average_coeff);
    auto begin = values_vec.begin() + threshold;
    auto end = values_vec.end() - threshold;
    T sum = std::accumulate(begin, end, T{});
    return AverageResult{sum / std::distance(begin, end), num};
  }

  std::deque<T> values;

  const Config configuration;
};

#endif //WUESWU_RESULTAVERAGE_H
