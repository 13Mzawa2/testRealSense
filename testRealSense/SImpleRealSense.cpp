#include <opencv2\opencv.hpp>
#include <pxcsensemanager.h>

#pragma comment(lib, "libpxc_d.lib")

using namespace cv;
using namespace std;

int main(void)
{
	PXCSenseManager *psm = PXCSenseManager::CreateInstance();
	if (!psm) {
		cout << "Unable to create PXCSenseManager" << endl;
		return 1;
	}
	//	RealSense settings
	psm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480);
	psm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480);
	psm->Init();
	//	OpenCV frame buffer
	Mat frame_c(480, 640, CV_8UC3);		//	color frame
	Mat frame_d(480, 640, CV_16SC1);		//	depth frame
	//	loop
	while (1) {
		if (psm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) break;

		//	RealSense frame buffer
		PXCCapture::Sample *sample = psm->QuerySample();
		PXCImage *img_c = sample->color;
		PXCImage *img_d = sample->depth;
		PXCImage::ImageData data_c, data_d;
		img_c->AcquireAccess(PXCImage::ACCESS_READ_WRITE, PXCImage::PIXEL_FORMAT_BGR, &data_c);
		img_d->AcquireAccess(PXCImage::ACCESS_READ_WRITE, &data_d);
		//	Copy to OpenCV
		frame_c.data = data_c.planes[0];
		frame_d.data = data_d.planes[0];
		Mat frame_d32;
		frame_d.convertTo(frame_d32, CV_32F);
		normalize(frame_d32, frame_d32, 0, 1, NORM_MINMAX);
		imshow("Color", frame_c);
		imshow("Depth", frame_d32);

		if (waitKey(10) == 27) break;

		psm->ReleaseFrame();
	}
	psm->Release();
	
	return 0;
}