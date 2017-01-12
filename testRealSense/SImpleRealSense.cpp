#include <opencv2\opencv.hpp>
//#include <pxcsensemanager.h>
#include <RealSense\SenseManager.h>
#include <RealSense\SampleReader.h>

#pragma comment(lib, "libpxc.lib")

using namespace Intel::RealSense;
using namespace cv;
using namespace std;

void getColorStream(Intel::RealSense::Capture::Sample *sample, cv::Mat &color)
{
	//	Acquire access to image data
	Image *img_c = sample->color;
	Image::ImageData data_c;
	img_c->AcquireAccess(Image::ACCESS_READ_WRITE, Image::PIXEL_FORMAT_BGR, &data_c);
	//	create OpenCV Mat from Image::ImageInfo
	Image::ImageInfo cinfo = img_c->QueryInfo();
	Mat temp_c(cinfo.height, cinfo.width, CV_8UC3);
	//	copy data
	temp_c.data = data_c.planes[0];
	temp_c.copyTo(color);
	//	release access
	img_c->ReleaseAccess(&data_c);
}

void getDepthStream(Intel::RealSense::Capture::Sample *sample, cv::Mat &depth)
{
	Image *img_d = sample->depth;
	Image::ImageData data_d;
	img_d->AcquireAccess(Image::ACCESS_READ_WRITE, Image::PIXEL_FORMAT_DEPTH_F32, &data_d);
	Image::ImageInfo dinfo = img_d->QueryInfo();
	Mat temp_d(dinfo.height, dinfo.width, CV_32FC1);
	temp_d.data = data_d.planes[0];
	temp_d.copyTo(depth);
	img_d->ReleaseAccess(&data_d);
}

void getDepthFromCamStream(Intel::RealSense::SenseManager *psm , cv::Mat &mapped_dapth)
{
	//	create projection stream to acquire mapped depth image
	Projection *projection = psm->QueryCaptureManager()->QueryDevice()->CreateProjection();
	Image *depth_mapped = projection->CreateDepthImageMappedToColor(psm->QuerySample()->depth, psm->QuerySample()->color);
	//	acquire access to depth data
	Image::ImageData ddata_mapped;
	depth_mapped->AcquireAccess(Image::ACCESS_READ, Image::PIXEL_FORMAT_DEPTH_F32, &ddata_mapped);
	//	copy to cv::Mat
	uint32_t *d = reinterpret_cast<uint32_t*>(ddata_mapped.planes[0]);
	Image::ImageInfo dinfo = depth_mapped->QueryInfo();
	Mat frame_d_mapped(dinfo.height, dinfo.width, CV_32FC1);
	frame_d_mapped.data = ddata_mapped.planes[0];
	frame_d_mapped.copyTo(mapped_dapth);
	//	release access
	depth_mapped->ReleaseAccess(&ddata_mapped);
}

void getStreams(Intel::RealSense::Capture::Sample *sample, cv::Mat &color, cv::Mat &depth)
{
	getColorStream(sample, color);
	getDepthStream(sample, depth);
}

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
	//Session *session = psm->QuerySession();
	
	//	OpenCV frame buffer
	//Mat frame_c(480, 640, CV_8UC3);		//	color frame
	//Mat frame_d(480, 640, CV_16SC1);		//	depth frame

	//	loop
	while (1) {
		int64 t = cv::getTickCount();
		double f = cv::getTickFrequency();

		if (psm->AcquireFrame(true) < Status::STATUS_NO_ERROR) break;

		//	RealSense frame buffer
		Capture::Sample *sample = psm->QuerySample();
		Mat frame_c, frame_d;
		getStreams(sample, frame_c, frame_d);
		Mat frame_d32;
		frame_d.convertTo(frame_d32, CV_32F);
		frame_d32 /= 1000.0;
		imshow("Color", frame_c);
		imshow("Depth", frame_d32);

		//	depth map to color image
		getDepthFromCamStream(psm, frame_d);
		Mat frame_d32_mapped;
		normalize(frame_d, frame_d32_mapped, 1, 0, CV_MINMAX);
		imshow("Mapped Depth", frame_d32_mapped);
		
		//	reconstruct RGBD image
		

 		if (waitKey(10) == 27) break;

		psm->ReleaseFrame();
		
		double fps = f / (cv::getTickCount() - t);
		static int count = 0;
		static double fps_sum = 0.0;
		if (count >= 33) {
			cout << "fps = " << fps_sum / count << "\r";
			count = 0;
			fps_sum = 0.0;
		}
		else {
			fps_sum += fps;
			count++;
		}
	}
	psm->Release();
	
	return 0;
}