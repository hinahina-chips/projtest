#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>

#ifdef _DEBUG
#pragma comment(lib, "opencv_world470d.lib")
#define CAMERA_EXT "d.lib"
#else
#pragma comment(lib, "opencv_world470.lib")
#define CAMERA_EXT ".lib"
#endif

#include "HighSpeedProjector.h"
#include "ProjectorUtility.h"

int main() {
	bool loopFlag = true;
	int projFrame = 0;
	cv::Mat sampleImg = cv::imread("./0000.jpg");

	std::thread thrProj([&] {
		try {
			HighSpeedProjector proj_V3;
			DYNAFLASH_PARAM param = getDefaultDynaParamRGB();
			param.dFrameRate = 500.0;
			param.nMirrorMode = 0;
			printDynaParam(param);

			proj_V3.connect(0);
			proj_V3.setParam(param);
			proj_V3.start();

			while (loopFlag) {
				projFrame += proj_V3.sendImage(sampleImg.data);
			}

			proj_V3.stop();
			proj_V3.disconnect();
		}
		catch (std::exception& e) {
			std::cout << "\033[41m ERROR \033[49m\033[31m thrProj : " << e.what() << "\033[39m" << std::endl;
		}
		});

	clock_t start = clock();
	while (loopFlag) {
		clock_t now = clock();
		const double time_ = static_cast<double>(now - start) / CLOCKS_PER_SEC;
		if (time_ > 1.0) {
			std::cout << "projection : " << projFrame / time_ << " fps" << std::endl;
			projFrame = 0;
			start = now;
		}

		if ((GetAsyncKeyState(VK_ESCAPE) & 0x80000000) != 0) {
			loopFlag = false;
		}
	}

	thrProj.join();

	return 0;
}