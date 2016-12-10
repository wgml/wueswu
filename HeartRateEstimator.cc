#include <iostream>
#include "HeartRateEstimator.h"
#include <cassert>
#include <thread>
#include <array>
#include <chrono>
#include <algorithm>

#include "kissfft/kiss_fft.h"
#include "kissfft/tools/kiss_fftr.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

void HeartRateEstimator::init() {
  provider->subscribe(this);
}

void HeartRateEstimator::run(double tps) {
  work = true;
  while (work) {
    auto start_time = high_resolution_clock::now();
    auto hr_estimate = estimate();
    auto end_time = high_resolution_clock::now();
    long execution_time = duration_cast<microseconds>(end_time - start_time).count();
    std::cout << "Determined heart rate is " << hr_estimate
      << ". There are " << data.size() << " samples in context. It took me "
      << execution_time << "us to execute." << std::endl;

    if (is_valid(hr_estimate)) {
      auto hr_averaged = average.average(hr_estimate);
      std::cout << "Average out of " << hr_averaged.num
                << " samples is " << hr_averaged.value << std::endl;
    } else {
      std::cout << "Estimated value is considered invalid." << std::endl;
    }
    auto sleep_time = std::max<long>(0L, static_cast<long>(1e6 / tps) - execution_time);
    std::this_thread::sleep_for(microseconds{ sleep_time });
  }
}

double HeartRateEstimator::estimate() {
  data_t raw_data;
  double frequency;
  {
    std::lock_guard<std::mutex> lock(data_mutex);
    if (data.size() < WINDOW_SIZE)
    {
      std::cerr << " Not enough data samples collected yet." << std::endl;
      return -1;
    }
    frequency = get_raw_data(raw_data);
    assert(frequency > 0);
  }
  std::cerr << " Determined signal freq is " << frequency << std::endl;
  data_t filtered_data;
  data_t ffted_data;
  data_t ffted_freq;

  filter_data(raw_data, filtered_data);
  fft_data(filtered_data, ffted_data, ffted_freq, frequency);
  double result = determine_result(ffted_data, ffted_freq);

  return result * 60;
}

double HeartRateEstimator::get_raw_data(data_t &raw_data)
{
  assert(data.size() == WINDOW_SIZE);
  unsigned long timestamp_diffs = 0;
  unsigned long prev_timestamp = 0;
  
  for (size_t i = 0; i < WINDOW_SIZE; i++) {
    raw_data[i] = data.at(i).pixelSum;
    unsigned long timestamp = static_cast<unsigned long>(data.at(i).timestamp.count());
    if (prev_timestamp > 0)
        timestamp_diffs += (timestamp - prev_timestamp);
    prev_timestamp = timestamp;
  }
  return 1 / ((timestamp_diffs / 1e6) / (WINDOW_SIZE - 1));
}

void HeartRateEstimator::filter_data(const data_t &raw_data, data_t &filtered_data)
{
  filter.filter(raw_data, filtered_data);
}

void HeartRateEstimator::fft_data(const data_t &raw_data, data_t &fft_data_abs, data_t &fft_data_freq, const double avg_freq)
{
  size_t nfft;
  for (nfft = 1; nfft < WINDOW_SIZE; nfft *= 2);

  kiss_fft_scalar *signal = new kiss_fft_scalar[nfft];
  kiss_fft_cpx *fft = new kiss_fft_cpx[nfft];
  for (size_t i = 0; i < WINDOW_SIZE; i++)
  {
    signal[i] = static_cast<float>(raw_data[i]);
  }
  for (size_t i = WINDOW_SIZE; i < nfft; i++)
  {
    signal[i] = 0;
  }

  kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft, 0, nullptr, nullptr);
  kiss_fftr(cfg, signal, fft);

  const size_t L = WINDOW_SIZE;
  for (size_t i = 0; i < WINDOW_SIZE; i++)
  {
    fft_data_abs[i] = 2 * std::sqrt(std::pow(fft[i].r, 2) + std::pow(fft[i].i, 2)) / L; // window_size vs nfft, what with imaginary?
    fft_data_freq[i] = (avg_freq * i) / L;
  }
  free(cfg);
  delete[] signal;
  delete[] fft;
}

double HeartRateEstimator::determine_result(const data_t &fft_abs, const data_t &fft_freq)
{
  const double freq_lo = 0.8, freq_hi = 3;

  size_t idx;
  double max_freq = -1;
  double max_freq_val = -1;;

  for (idx = 0; idx < WINDOW_SIZE; idx++)
  {
    if (fft_freq[idx] >= freq_lo)
    {
      max_freq_val = fft_abs[idx];
      max_freq = fft_freq[idx];
      break;
    }
  }
  for (idx++; idx < WINDOW_SIZE; idx++)
  {
    if (fft_freq[idx] > freq_hi)
      break;

    if (max_freq_val < fft_abs[idx])
    {
      max_freq = fft_freq[idx];
      max_freq_val = fft_abs[idx];
    }
  }
  return max_freq;
}
