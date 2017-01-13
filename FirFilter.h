#ifndef WUESWU_FIRFILETR_H
#define WUESWU_FIRFILETR_H

#include <cassert>
#include <algorithm>
#include <array>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

/**
 * Class implementing windowed FIR filter.
 * Create date:
 *    16/12/10
 * Last modification date:
 *    16/12/25
 *
 * @authors Anna Musiał, Wojciech Gumuła
 *
 * @tparam M Window size is determined as 2 * M +1. Thus, M shall be a positive integer.
 */
template<int M>
struct FirFilter {
  using data_t = std::array<double, 2 * M + 1>;

  /**
   * Construction custom hamming or rectangular window.
   * Can produce low or high pass filter for given signal and filtering frequency.
   *
   * Create date:
   *    16/12/10
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   *
   * @param is_low_pass determines if filter is low or high pass
   * @param fs signal sampling rate
   * @param fc fitering frequency
   * @param is_hamming Implements hamming window if true, rectangular otherwise
   */
  FirFilter(bool is_low_pass, double fs, double fc, bool is_hamming = false) {
    static_assert(M > 0, "M must be greater than 0");
    double fcn = fc / (fs * 2);
    double wcn = 2 * M_PI * fcn;

    data_t samples;
    if (is_low_pass) {

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


  /**
   * Filters input signal with given filter.
   *
   * Create date:
   *    16/12/10
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   *
   * @param raw_data input data container
   * @param filtered_data container for output data
   */
  void filter(const std::vector<double> &raw_data, std::vector<double> &filtered_data) {
    size_t K = raw_data.size();
    assert(K >= N);
    int P = M - 1;
    ssize_t L = K + P;

    std::vector<double> signal_prim(static_cast<size_t>(K + M - 1));
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