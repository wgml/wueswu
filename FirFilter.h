#ifndef WUESWU_FIRFILETR_H
#define WUESWU_FIRFILETR_H

#include <cassert>
#include <array>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

template<int M>
struct FirFilter
{
  using data_t = std::array<double, 2 * M + 1>;

  FirFilter(int fitler_type, double fs, double fc)
  {
    static_assert(M > 0, "M must be greater than 0");
    double fcn = fc / (fs * 2);
    double wcn = 2 * M_PI*fcn;

    data_t samples;
    if (fitler_type == 1)
    {

      for (int i = -M, j = 0; i <= M; ++i, ++j)
      {
        if (i<0)
          samples[j] = sin(wcn*i) / (M_PI*i);
        else if (i == 0)
          samples[j] = wcn / M_PI;
        else
          samples[j] = sin(wcn*i) / (M_PI*i);
      }
    }
    else
    {
      for (int i = -M, j = 0; i <= M; ++i, ++j)
      {
        if (i < 0)
          samples[j] = -sin(wcn*i) / (M_PI*i);
        else if (i == 0)
          samples[j] = 1 - wcn / M_PI;
        else
          samples[j] = -sin(wcn*i) / (M_PI*i);
      }
    }

    data_t hamming;
    for (int k = 0; k <= N - 1; k++)
    {
      hamming[k] = 0.54 - 0.46 * cos(2 * M_PI * k / (N - 1));
      filter_coefs[k] = hamming[k] * samples[k];
      std::cerr << filter_coefs[k] << std::endl;
    }
  }

  ~FirFilter() = default;

  template<size_t K>
  void filter(const std::array<double, K> &raw_data, std::array<double, K> &filtered_data)
  {
    assert(K >= N);
    size_t P = M - 1;
    size_t L = K + P;

    std::array<double, K + M - 1> signal_prim{};
    for (size_t i = 0; i < P; ++i)
      signal_prim[i] = raw_data[0];

    std::copy(raw_data.begin(), raw_data.end(), signal_prim.begin() + P);

    for (size_t k = M; k <= L; ++k)
    {
      double v = 0;
      for (auto m = 0; m < M; ++m)
      {
        v += filter_coefs[m] * signal_prim[k - m - 1];
      }
      filtered_data[k - M] = v;
    }
  }

private:
  data_t filter_coefs;
  const int N = 2 * M + 1;
};

#endif