#pragma once
#include <pylon/PylonIncludes.h>
#include <chrono>

class ImageSaver
{
public:
	void save(Pylon::CGrabResultPtr ptrGrabResult) {
		Pylon::CPylonImage img;
		img.AttachGrabResultBuffer(ptrGrabResult);
		img.Save(Pylon::ImageFileFormat_Tiff, formatName(imageCount));
	}

private:

	static const char* getPrefix(const std::chrono::seconds& initTime)
	{
		std::ostringstream stream;
		stream << initTime.count() << "-";
		return stream.str().c_str();
	}

	Pylon::String_t formatName(size_t count)
	{
		char buffer[100];
		std::snprintf(buffer, sizeof(buffer), "%s-%i.bmp", prefix, count);
		return buffer;
	}

	const char* prefix = getPrefix(
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			)
	);

	size_t imageCount = 0;

};
