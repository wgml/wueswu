
#ifndef WUESWU_RESULTAVERAGE_H
#define WUESWU_RESULTAVERAGE_H

#include <deque>
template<typename T, size_t Count = 10>
class ResultAverage {
public:
  struct AverageResult {
    T value;
    size_t num;
  };

  AverageResult average(T new_value) {
    if (values.size() == Count)
      values.pop_front();
    values.push_back(new_value);

    T sum = 0;
    for (auto &e : values)
      sum += e;
    auto num = values.size();
    return AverageResult{sum / num, num};
  }
private:
  std::deque<T> values;
};

#endif //WUESWU_RESULTAVERAGE_H
