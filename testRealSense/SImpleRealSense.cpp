#include <opencv2\opencv.hpp>
//#include <pxcsensemanager.h>
#include <RealSense\SenseManager.h>
#include <RealSense\SampleReader.h>

#pragma comment(lib, "libpxc.lib")

using namespace Intel::RealSense;
using namespace cv;
using namespace std;

int main(void)
{
	SenseManager *psm = SenseManager::CreateInstance();
	if (!psm) {
		cout << "Unable to create PXCSenseManager" << endl;
		return 1;
	}
	//	RealSense settings
	psm->EnableStream(Capture::STREAM_TYPE_COLOR, 640, 480);
	psm->EnableStream(Capture::STREAM_TYPE_DEPTH, 640, 480);
	psm->Init();
	//	OpenCV frame buffer
	Mat frame_c(480, 640, CV_8UC3);		//	color frame
	Mat frame_d(480, 640, CV_16SC1);		//	depth frame
	//	loop
	while (1) {
		if (psm->AcquireFrame(true) < Status::STATUS_NO_ERROR) break;

		//	RealSense frame buffer
		Capture::Sample *sample = psm->QuerySample();
		Image *img_c = sample->color;
		Image *img_d = sample->depth;
		Image::ImageData data_c, data_d;
		img_c->AcquireAccess(Image::ACCESS_READ_WRITE, Image::PIXEL_FORMAT_BGR, &data_c);
		img_d->AcquireAccess(Image::ACCESS_READ_WRITE, &data_d);
		//	Copy to OpenCV
		frame_c.data = data_c.planes[0];
		frame_d.data = data_d.planes[0];
		Mat frame_d32;
		frame_d.convertTo(frame_d32, CV_32F);		//	‰æ‘f’l‚ª‚»‚Ì“_‚Ì[“x[mm]
		frame_d32 /= 1000.0;
		imshow("Color", frame_c);
		imshow("Depth", frame_d32);

 		if (waitKey(10) == 27) break;

		psm->ReleaseFrame();
	}
	psm->Release();
	
	return 0;
}