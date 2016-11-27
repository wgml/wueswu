#pragma once

#include <pylon/PylonIncludes.h>
#include <chrono>
#include <fstream>

class ContextSaver {
public:
  ContextSaver()
      : file(std::fstream()) {
    file.open(file_name(), std::fstream::out);
  }

  ~ContextSaver() {
    file.close();
  }

  void save_context(Pylon::CGrabResultPtr ptrGrabResult, int img_idx) {
    auto width = ptrGrabResult->GetWidth();
    auto height = ptrGrabResult->GetHeight();

    std::cout << "#" << img_idx << " " << width << "x" << height << std::endl;

    const uint8_t *pImageBuffer = static_cast<uint8_t *>(ptrGrabResult->GetBuffer());

    unsigned long sum = 0;
    unsigned long cnt = 0;
    for (size_t row = 0; row < width; row++) {
      for (size_t col = (row + 1) % 2; col < height; col += 2) {
        cnt++;
        sum += pImageBuffer[row * height + col];
      }
    }

    std::cout << "#" << img_idx << " " << "sum: " << sum << " cnt: " << cnt << " result: "
              << sum / cnt << std::endl;
    save_context(current_micros(), sum, cnt);
  }

  void save_context(std::chrono::microseconds micros, unsigned long sum, unsigned long cnt) {
    file << micros.count() << " " << sum << " " << cnt << std::endl;
  }

private:
  std::fstream file;

  Pylon::String_t file_name() const {
    auto init_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch());
    std::ostringstream stream;
    stream << init_time.count() << ".txt";
    return stream.str().c_str();
  }

  std::chrono::microseconds current_micros() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }

};
