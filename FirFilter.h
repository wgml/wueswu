#ifndef WUESWU_FIRFILETR_H
#define WUESWU_FIRFILETR_H

#include <cassert>
#include <array>

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

template<int M>
struct FirFilter {
  using data_t = std::array<double, 2 * M + 1>;

  FirFilter(int filter_type, double fs, double fc, bool is_hamming = false) {
    static_assert(M > 0, "M must be greater than 0");
    double fcn = fc / (fs * 2);
    double wcn = 2 * M_PI * fcn;

    data_t samples;
    if (filter_type == 1) {

      for (int i = -M, j = 0; i <= M; ++i, ++j) {
        if (i < 0)
          samples[j] = sin(wcn * i) / (M_PI * i);
        else if (i == 0)
          samples[j] = wcn / M_PI;
        else
          samples[j] = sin(wcn * i) / (M_PI * i);
      }
    } else {
      for (int i = -M, j = 0; i <= M; ++i, ++j) {
        if (i < 0)
          samples[j] = -sin(wcn * i) / (M_PI * i);
        else if (i == 0)
          samples[j] = 1 - wcn / M_PI;
        else
          samples[j] = -sin(wcn * i) / (M_PI * i);
      }
    }

    for (int k = 0; k <= N - 1; k++) {
      if (is_hamming) {
        auto hamming = 0.54 - 0.46 * cos(2 * M_PI * k / (N - 1));
        filter_coeffs[k] = hamming * samples[k];
      } else {
        filter_coeffs[k] = samples[k];
      }
    }
  }

  ~FirFilter() = default;

  template<size_t K>
  void filter(const std::array<double, K> &raw_data, std::array<double, K> &filtered_data) {
    assert(K >= N);
    int P = M - 1;
    ssize_t L = K + P;

    std::array<double, K + M - 1> signal_prim{};
    for (auto i = 0; i < P; ++i)
      signal_prim[i] = raw_data[0];

    std::copy(raw_data.begin(), raw_data.end(), signal_prim.begin() + P);

    for (auto k = M; k <= L; ++k) {
      double v = 0;
      for (auto m = 0; m < M; ++m) {
        v += filter_coeffs[m] * signal_prim[k - m - 1];
      }
      filtered_data[k - M] = v;
    }
  }

private:
  data_t filter_coeffs;
  const int N = 2 * M + 1;
};

#endif