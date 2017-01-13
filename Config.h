#pragma once
#ifndef WUESWU_CONFIG_H
#define WUESWU_CONFIG_H

#include <iostream>
#include <string>
#include <tclap/CmdLine.h>

using TCLAP::ValueArg;
using TCLAP::SwitchArg;
using TCLAP::ArgException;
using TCLAP::CmdLine;

struct Config {
  struct {
    bool mock = false;
    std::string mock_filename = "";
    bool mock_infinitely = true;

    unsigned int width = 800;
    unsigned int height = 600;
    unsigned int exposure_time = 10000;
    double fps = 60;
    unsigned int images_to_grab = 1000000;
    // todo gain?
  } camera;

  struct {
    double fps = 10;
    double min_freq = 0.66;
    double max_freq = 4;
    double window_length = 30;
    bool filtered_average = true;
    double filtered_average_coeff = 0.1;
    unsigned int average_window = 30;
  } estimator;

  friend std::ostream &operator<<(std::ostream &o, const Config &cfg) {
    o << "camera" << std::endl;
    if (cfg.camera.mock) {
      o << " mock_filename=" << std::boolalpha << cfg.camera.mock_filename << std::endl;
      o << " mock_infinitely=" << cfg.camera.mock_infinitely << std::endl;
    } else {
      o << " width=" << cfg.camera.width << std::endl;
      o << " height=" << cfg.camera.height << std::endl;
      o << " exposure_time=" << cfg.camera.exposure_time << std::endl;
      o << " fps=" << cfg.camera.fps << std::endl;
      o << " images_to_grab=" << cfg.camera.images_to_grab << std::endl;
    }
    o << "estimator" << std::endl;
    o << " fps=" << cfg.estimator.fps << std::endl;
    o << " min_freq=" << cfg.estimator.min_freq << std::endl;
    o << " max_freq=" << cfg.estimator.max_freq << std::endl;
    o << " window_length=" << cfg.estimator.window_length << std::endl;
    o << " filtered_average=" << std::boolalpha << cfg.estimator.filtered_average << std::endl;
    if (cfg.estimator.filtered_average)
      o << " filtered_average_coeff=" << cfg.estimator.filtered_average_coeff << std::endl;
    o << " average_window=" << cfg.estimator.average_window << std::endl;
    return o;
  }
};

const Config parse_config(int argc, char *argv[]);

#endif