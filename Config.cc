#include "Config.h"

const Config parse_config(int argc, char *argv[]) {
  try {
    auto config = Config{};

    CmdLine cmd("Estimate heart rate from video stream.", ' ', "1");
    ValueArg<std::string> mock_filename("m", "mock-filename",
                                        "Mock camera provider with previously saved context from given file.",
                                        false, config.camera.mock_filename, "filename");
    SwitchArg no_mock_infinitely("n", "no-mock-infinitely", "Loop mocked video stream.",
                                 !config.camera.mock_infinitely);

    ValueArg<unsigned int> video_width("x", "video-width", "Width for camera video stream.", false,
                                       config.camera.width, "pixels");
    ValueArg<unsigned int> video_height("y", "video-height", "Height for camera video stream.",
                                        false, config.camera.height, "pixels");
    ValueArg<unsigned int> exposure_time("t", "exposure-time", "Camera exposure time.", false,
                                         config.camera.exposure_time, "milliseconds");
    ValueArg<double> camera_fps("f", "camera-fps", "Camera fps.", false, config.camera.fps,
                                "frames per second");
    ValueArg<unsigned int> images_to_grab("i", "images-to-grab",
                                          "Limit number of images to grab from camera.", false,
                                          config.camera.images_to_grab, "value");

    ValueArg<double> estimator_fps("e", "estimator-fps", "Estimator fps.", false,
                                   config.estimator.fps, "frames per second");
    ValueArg<double> min_freq("l", "minimum-freq", "Lower bound for estimate freq.", false,
                              config.estimator.min_freq, "Hz");
    ValueArg<double> max_freq("u", "maximum-freq", "Upper bound for estimate freq.", false,
                              config.estimator.max_freq, "Hz");
    ValueArg<double> window_length("w", "window-length", "Estimate window length.", false,
                                   config.estimator.window_length, "seconds");
    SwitchArg arithm_average("a", "use-arithm-average",
                             "Use arithmetic instead of truncated average. Second one will remove samples from each end of sorted values set.",
                             !config.estimator.filtered_average);
    ValueArg<double> filtered_average_coeff("c", "filtered-average-coeff",
                                            "Coefficient for truncated average.", false,
                                            config.estimator.filtered_average_coeff, "ratio");
    ValueArg<double> average_window("d", "average-window",
                                    "Number of samples to compute average from.", false,
                                    config.estimator.average_window, "value");

    cmd.add(mock_filename);
    cmd.add(no_mock_infinitely);
    cmd.add(video_width);
    cmd.add(video_height);
    cmd.add(exposure_time);
    cmd.add(camera_fps);
    cmd.add(images_to_grab);
    cmd.add(estimator_fps);
    cmd.add(min_freq);
    cmd.add(max_freq);
    cmd.add(window_length);
    cmd.add(arithm_average);
    cmd.add(filtered_average_coeff);
    cmd.add(average_window);
    cmd.parse(argc, argv);

    if (mock_filename.isSet()) {
      config.camera.mock = true;
      config.camera.mock_filename = mock_filename.getValue();
    }
    if (no_mock_infinitely.isSet())
      config.camera.mock_infinitely = false;

    if (video_width.isSet())
      config.camera.width = video_width.getValue();

    if (video_height.isSet())
      config.camera.height = video_height.getValue();

    if (exposure_time.isSet())
      config.camera.exposure_time = exposure_time.getValue();

    if (camera_fps.isSet())
      config.camera.fps = camera_fps.getValue();

    if (images_to_grab.isSet())
      config.camera.images_to_grab = images_to_grab.getValue();

    if (estimator_fps.isSet())
      config.estimator.fps = estimator_fps.getValue();

    if (min_freq.isSet())
      config.estimator.min_freq = min_freq.getValue();

    if (max_freq.isSet())
      config.estimator.max_freq = max_freq.getValue();

    if (arithm_average.isSet())
      config.estimator.filtered_average = false;

    if (filtered_average_coeff.isSet())
      config.estimator.filtered_average_coeff = filtered_average_coeff.getValue();

    if (average_window.isSet())
      config.estimator.average_window = average_window.getValue();

    return config;
  } catch (ArgException &e) {
    std::cout << "Configuration error " << e.error() << " for argument " << e.argId() << std::endl;
    std::cout << "Falling back to default values." << std::endl;
    return Config{};
  }
}
