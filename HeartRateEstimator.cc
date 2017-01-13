#include <iostream>
#include <thread>
#include <algorithm>
#include <numeric>
#include <vector>

#include "kissfft/kiss_fft.h"
#include "kissfft/tools/kiss_fftr.h"
#include "KalmanFilter.h"
#include "HeartRateEstimator.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

void HeartRateEstimator::init() {
  provider->subscribe(this);
}

void HeartRateEstimator::run() {
  work = true;
  while (work) {
    auto start_time = high_resolution_clock::now();
    auto hr_estimate = estimate();
    auto end_time = high_resolution_clock::now();
    auto execution_time = duration_cast<microseconds>(end_time - start_time).count();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()
              << ": determined heart rate is " << hr_estimate
              << ". There are " << data.size() << " samples in context. It took me "
              << execution_time << "us to execute." << std::endl;

    if (is_valid(hr_estimate)) {
      auto hr_averaged = average.average(hr_estimate);
      std::cout << "Average out of " << hr_averaged.num
                << " valid samples is " << hr_averaged.value << std::endl;
    } else {
      std::cout << "Estimated value is considered invalid." << std::endl;
    }
    auto sleep_time = std::max<long>(0L, static_cast<long>(1e6 / configuration.estimator.fps) -
                                         execution_time);
    std::this_thread::sleep_for(microseconds{sleep_time});
  }
}

double HeartRateEstimator::estimate() {
  size_t window = window_size();
  data_t raw_data((window));
  double frequency;
  {
    std::lock_guard<std::mutex> lock(data_mutex);
    if (data.size() < window) {
      std::cerr << " Not enough data samples collected yet." << std::endl;
      return -1;
    }
    frequency = get_raw_data(raw_data);
    assert(frequency > 0);
  }
  std::cerr << " Determined signal freq is " << frequency << std::endl;
  data_t normalized_data(window);
  data_t kalmaned_data(window);
  data_t filtered_data(window);
  data_t ffted_data(window);
  data_t ffted_freq(window);

  normalize_data(raw_data, normalized_data);
  kalman_data(normalized_data, kalmaned_data);
  filter_data(kalmaned_data, filtered_data, frequency);
  fft_data(filtered_data, ffted_data, ffted_freq, frequency);
  double result = determine_result(ffted_data, ffted_freq);

  return result * 60;
}

double HeartRateEstimator::get_raw_data(data_t &raw_data) {
  size_t window = window_size();

  unsigned long timestamp_diffs = 0;
  unsigned long prev_timestamp = 0;
  unsigned long timestamp_diff_min = std::numeric_limits<unsigned long>::max();
  unsigned long timestamp_diff_max = 0;

  for (size_t i = 0; i < window; i++) {
    raw_data[i] = data.at(i).pixelSum;
    unsigned long timestamp = static_cast<unsigned long>(data.at(i).timestamp.count());
    if (prev_timestamp > 0) {
      unsigned long timestamp_diff = timestamp - prev_timestamp;
      timestamp_diffs += timestamp_diff;

      timestamp_diff_min = std::min(timestamp_diff_min, timestamp_diff);
      timestamp_diff_max = std::max(timestamp_diff_max, timestamp_diff);

      if (timestamp_diff_max >= 1.5 * timestamp_diff_min) {
        std::cout
            << "There was significant difference between lowest and highest interval between image frames. "
                "This might indicate one or more frames were skipped. "
                "Thus estimate might be inaccurate. "
                "(min: " << timestamp_diff_min
            << ", max: " << timestamp_diff_max << ")." << std::endl;

      }
    }
    prev_timestamp = timestamp;
  }
  return 1 / ((timestamp_diffs / 1e6) / (window - 1));
}

void HeartRateEstimator::normalize_data(const data_t &raw_data, data_t &normalized_data) {
  size_t window = window_size();
  auto sum = std::accumulate(raw_data.begin(), raw_data.end(), 0.0);
  auto mean = sum / window;

  std::vector<data_t::value_type> diff(window);
  std::transform(raw_data.begin(), raw_data.end(), diff.begin(),
                 [mean](double x) { return x - mean; });
  auto sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
  auto stdev = std::sqrt(sq_sum / window);

  for (size_t i = 0; i < raw_data.size(); i++)
    normalized_data[i] = (raw_data[i] - mean) / stdev;
}

void HeartRateEstimator::kalman_data(const data_t &normalized_data, data_t &kalmaned_data) {
  kalman_filter::predict(normalized_data, kalmaned_data);
}

void HeartRateEstimator::filter_data(const data_t &raw_data, data_t &filtered_data,
                                     double sampling_rate) {
  data_t tmp_data(window_size());
  FirFilter<10>{true, sampling_rate, configuration.estimator.max_freq}.filter(raw_data, tmp_data);
  FirFilter<10>{false, sampling_rate, configuration.estimator.min_freq}.filter(tmp_data,
                                                                               filtered_data);
}

void
HeartRateEstimator::fft_data(const data_t &raw_data, data_t &fft_data_abs, data_t &fft_data_freq,
                             const double avg_freq) {
  size_t window = window_size();
  size_t nfft;
  for (nfft = 1; nfft < window; nfft *= 2);

  kiss_fft_scalar *signal = new kiss_fft_scalar[nfft];
  kiss_fft_cpx *fft = new kiss_fft_cpx[nfft];
  for (size_t i = 0; i < window; i++) {
    signal[i] = static_cast<float>(raw_data[i]);
  }
  for (size_t i = window; i < nfft; i++) {
    signal[i] = 0;
  }

  kiss_fftr_cfg cfg = kiss_fftr_alloc(static_cast<int>(nfft), 0, nullptr, nullptr);
  kiss_fftr(cfg, signal, fft);

  const size_t L = window;
  for (size_t i = 0; i < window; i++) {
    fft_data_abs[i] = 2 * std::sqrt(std::pow(fft[i].r, 2) + std::pow(fft[i].i, 2)) / L;
    fft_data_freq[i] = (avg_freq * i) / L;
  }
  free(cfg);
  delete[] signal;
  delete[] fft;
}

double HeartRateEstimator::determine_result(const data_t &fft_abs, const data_t &fft_freq) {
  size_t window = window_size();
  size_t idx;
  double max_freq = -1;
  double max_freq_val = -1;;

  for (idx = 0; idx < window; idx++) {
    if (fft_freq[idx] >= configuration.estimator.min_freq * 1.1) {
      max_freq_val = fft_abs[idx];
      max_freq = fft_freq[idx];
      break;
    }
  }

  bool valid = false;
  for (idx++; idx < window && fft_freq[idx] <= configuration.estimator.max_freq; idx++) {
    if (max_freq_val < fft_abs[idx]) {
      max_freq = fft_freq[idx];
      max_freq_val = fft_abs[idx];
      valid = true;
    }
  }
  return valid ? max_freq : -1;
}
