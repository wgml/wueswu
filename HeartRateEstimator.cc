#include <iostream>
#include "HeartRateEstimator.h"
#include <cassert>
#include <thread>
#include "kissfft/kiss_fft.h"
#include "kissfft/tools/kiss_fftr.h"

void HeartRateEstimator::init() {
  provider.subscribe(this);
}

void HeartRateEstimator::run() {
  work = true;
  while (work) {
    std::this_thread::sleep_for(std::chrono::seconds{1});
    std::cerr << "Determined heart rate is " << estimate()
              << ". There are " << data.size() << " samples in context." << std::endl;
  }
}

double HeartRateEstimator::estimate() {
  double *raw_data = nullptr;
  {
    std::lock_guard<std::mutex> lock(data_mutex);
    if (data.size() < WINDOW_SIZE)
    {
      std::cerr << " Not enough data samples collected yet." << std::endl;
      return -1;
    }
    raw_data = new double[WINDOW_SIZE];
    get_raw_data(raw_data);
  }

  double *filtered_data = new double[WINDOW_SIZE];
  double *ffted_data = new double[WINDOW_SIZE];
  double *ffted_freq = new double[WINDOW_SIZE];

  filter_data(raw_data, filtered_data);
  fft_data(filtered_data, ffted_data, ffted_freq, 33); //todo
  double result = determine_result(ffted_data, ffted_freq);
  delete[] filtered_data;
  delete[] raw_data;
  delete[] ffted_data;
  delete[] ffted_freq;

  return result * 60;
}

void HeartRateEstimator::get_raw_data(double * data_ptr)
{
  assert(data.size() == WINDOW_SIZE);
  for (size_t i = 0; i < WINDOW_SIZE; i++)
    data_ptr[i] = data.at(i).pixelSum;
}

void HeartRateEstimator::filter_data(double * raw_data, double *filtered_data)
{
  for (size_t i = 0; i < WINDOW_SIZE; i++)
    filtered_data[i] = raw_data[i];
}

void HeartRateEstimator::fft_data(double *raw_data, double *fft_data_abs, double *fft_data_freq, const double avg_freq)
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

double HeartRateEstimator::determine_result(double *fft_abs, double *fft_freq)
{
  const double freq_lo = 0.5, freq_hi = 4;

  size_t idx;
  double max_freq = -1;
  double max_freq_val = -1;;

  for (idx = 0; idx < WINDOW_SIZE; idx++)
    if (fft_freq[idx] >= freq_lo)
    {
      max_freq_val = fft_abs[idx];
      max_freq = fft_freq[idx];
      break;
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
