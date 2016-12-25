
#ifndef WUESWU_RESULTAVERAGE_H
#define WUESWU_RESULTAVERAGE_H

#include <deque>
#include <numeric>

/**
 * Can be used to average result of some data points for given type T.
 * @tparam T data type
 * @tparam Count Number of data points to average on.
 */
template<typename T, size_t Count = 10>
class ResultAverage {
public:
  /**
   * Struct containing information about computed average - value and number
   * of samples used to perform estimate.
   */
  struct AverageResult {
    T value;
    size_t num;
  };

  AverageResult average(T new_value) {
    if (values.size() == Count)
      values.pop_front();
    values.push_back(new_value);
    T sum = std::accumulate(values.begin(), values.end(), 0.0);
    auto num = values.size();
    return AverageResult{sum / num, num};
  }

private:
  std::deque<T> values;
};

#endif //WUESWU_RESULTAVERAGE_H
