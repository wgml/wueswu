
#include "CameraContextProvider.h"

void CameraContextProvider::run() {
  Pylon::PylonInitialize();
  Pylon::CBaslerUsbInstantCamera camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
  init(camera, width, height, exposureTime);

  camera.StartGrabbing(imagesToGrab); //@@@@@@@@

  Pylon::CGrabResultPtr ptrGrabResult;

  for (unsigned int idx = 0; idx < imagesToGrab && camera.IsGrabbing(); idx++) {
    camera.RetrieveResult(10000, ptrGrabResult, Pylon::TimeoutHandling_Return);

    if (ptrGrabResult->GrabSucceeded()) {
      grabSucceeded(ptrGrabResult, idx);
    } else {
      grabFailed(ptrGrabResult, idx);
    }
  }
  estimator->stop();
}

void CameraContextProvider::init(Pylon::CBaslerUsbInstantCamera &camera, int width, int height, int exposureTime) {
  camera.Open();

  int maxWidth = 2040;
  int maxHeight = 1086;
  int offsetX = (((maxWidth - width) / 2) / 2) * 2;
  int offsetY = (((maxHeight - height) / 2) / 2) * 2;
  std::cerr << width << "x" << height << "+" << offsetX << "+" << offsetY << " e=" << exposureTime
            << std::endl;

  camera.Width = width;
  camera.Height = height;
  camera.OffsetX = offsetX;
  camera.OffsetY = offsetY;
  camera.ExposureTime = exposureTime;
  camera.MaxNumBuffer = 100;
  setPixelFormat(camera);
}

void CameraContextProvider::setPixelFormat(Pylon::CBaslerUsbInstantCamera &camera) {
  try {
    CEnumerationPtr pixelFormat(camera.GetNodeMap().GetNode("PixelFormat"));
    if (IsAvailable(pixelFormat->GetEntryByName("BayerBG8"))) {
      pixelFormat->FromString("BayerBG8");
      std::cerr << "set pixel format to BayerBG8" << std::endl;
    } else {
      std::cerr << "BayerBG8 not available" << std::endl;
    }
  }
  catch (...) {
    std::cerr << "sumting went wong" << std::endl;
  }
}

void CameraContextProvider::grabSucceeded(Pylon::CGrabResultPtr resultPtr, unsigned int idx) {
  auto width = resultPtr->GetWidth();
  auto height = resultPtr->GetHeight();

  std::cerr << "#" << idx << " " << width << "x" << height << std::endl;

  const uint8_t* pImageBuffer = static_cast<uint8_t *>(resultPtr->GetBuffer());

  unsigned long sum = 0;
  unsigned long cnt = 0;
  for (size_t row = 0; row < width; row++)
  {
    for (size_t col = (row + 1) % 2; col < height; col += 2)
    {
      cnt++;
      sum += pImageBuffer[row * height + col];
    }
  }

  std::cerr << "#" << idx << " " << "sum: " << sum << " cnt: " << cnt << " result: " << sum / cnt << std::endl;

  auto now = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch());
  AcquisitionContext ctx{now, sum, cnt};
  estimator->notify(ctx);
  contextSaver.save_context(now, sum, cnt);
}

void CameraContextProvider::grabFailed(Pylon::CGrabResultPtr resultPtr, unsigned int idx) {
  std::cerr << "#" << idx << " " << "Error: " << resultPtr->GetErrorCode() << " "
            << resultPtr->GetErrorDescription() << std::endl;

}
