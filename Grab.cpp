#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

#include "ImageSaver.h"
#include "ConfigDump.h"

using GenApi_3_0_Basler_pylon_v5_0::CEnumerationPtr;
using GenApi_3_0_Basler_pylon_v5_0::IsAvailable;

static size_t imageCount = 0;

void grabSucceeded(Pylon::CGrabResultPtr ptrGrabResult)
{
	std::cout << "SizeX: " << ptrGrabResult->GetWidth() << std::endl;
	std::cout << "SizeY: " << ptrGrabResult->GetHeight() << std::endl;
	const uint8_t* pImageBuffer = static_cast<uint8_t *>(ptrGrabResult->GetBuffer());
	std::cout << "Gray value of first pixel: " << static_cast<uint32_t>(pImageBuffer[0]) << std::endl << std::endl;
}

void grabFailed(Pylon::CGrabResultPtr ptrGrabResult)
{
	std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
}

void waitExit()
{
	std::cerr << std::endl << "Press Enter to exit." << std::endl;

	while (std::cin.get() != '\n')
	{
	}
}

void setPixelFormat(Pylon::CInstantCamera& camera)
{
	try
	{
		CEnumerationPtr pixelFormat(camera.GetNodeMap().GetNode("PixelFormat"));
		if (IsAvailable(pixelFormat->GetEntryByName("RGB8packed")))
		{
			pixelFormat->FromString("RGB8packed");
		}
	}
	catch (...)
	{
		std::cerr << "sumting went wong" << std::endl;
	}
}

int main(int argc, char* argv[])
{
	int exitCode = 0;
	ImageSaver imageSaver;

	Pylon::PylonInitialize();

	try
	{
		Pylon::CInstantCamera camera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
		std::cout << "Using device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		camera.Open();

		// debug 
		ConfigDump::dump(camera);
		setPixelFormat(camera);

		const size_t imagesToGrab = 1000;
		camera.MaxNumBuffer = 1000;
		camera.StartGrabbing(imagesToGrab);

		Pylon::CGrabResultPtr ptrGrabResult;

		while (camera.IsGrabbing())
		{
			camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

			if (ptrGrabResult->GrabSucceeded())
			{
				grabSucceeded(ptrGrabResult);
				imageSaver.save(ptrGrabResult);

#ifdef PYLON_WIN_BUILD
				// Display the grabbed image.
				Pylon::DisplayImage(1, ptrGrabResult);
#endif
			}
			else
			{
				grabFailed(ptrGrabResult);
			}
		}
	}
	catch (const GenericException& e)
	{
		std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
		exitCode = 1;
	}

	waitExit();
	Pylon::PylonTerminate();
	return exitCode;
}
