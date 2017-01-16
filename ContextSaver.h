#pragma once

#include <string>
#include <chrono>
#include <fstream>

/**
 * Class provided to offer persisting context into files for future use.
 * Result can be used to mock camera provider.
 * Create date:
 *    16/11/27
 * Last modification date:
 *    16/12/25
 *
 * @authors Anna Musiał, Wojciech Gumuła
 */
class ContextSaver {
public:
  ContextSaver()
      : file(file_name(), std::fstream::out) {
  }

  ~ContextSaver() {
    file.close();
  }

  /**
   * Save given context into existing context file.
   * Create date:
   *    16/11/16
   * Last modification date:
   *    16/12/25
   *
   * @authors Anna Musiał, Wojciech Gumuła
   *
   * @param micros acquisition time
   * @param sum determined sum of green pixels
   * @param cnt determined number of green pixels
   */
  void save_context(std::chrono::microseconds micros, unsigned long sum, unsigned long cnt) {
    file << micros.count() << " " << sum << " " << cnt << std::endl;
  }

private:

  std::fstream file;

  static std::string file_name() {
    auto init_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch());
    std::ostringstream stream;
    stream << init_time.count() << ".txt";
    return stream.str();
  }
};
