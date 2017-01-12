#include "RealSenseCVWrapper.h"

using namespace Intel::RealSense;
using namespace cv;
using namespace std;


RealSenseCVWrapper::RealSenseCVWrapper(int w = 640, int h = 480)
{
	rsm = SenseManager::CreateInstance();
	if (!rsm) {
		cout << "Unable to create PXCSenseManager" << endl;
	}
	//	RealSense settings
	rsm->EnableStream(Capture::STREAM_TYPE_COLOR, w, h);
	rsm->EnableStream(Capture::STREAM_TYPE_DEPTH, w, h);
	rsm->Init();
	bufferSize = Size(w, h);
}


RealSenseCVWrapper::~RealSenseCVWrapper()
{
	rsm->Release();
}

bool RealSenseCVWrapper::queryStream()
{
	if (rsm->AcquireFrame(true) < Status::STATUS_NO_ERROR) {
		return false;
	}
	sample = rsm->QuerySample();
}

void RealSenseCVWrapper::getColorBuffer()
{
	//	Acquire access to image data
	Image *img_c = sample->color;
	Image::ImageData data_c;
	img_c->AcquireAccess(Image::ACCESS_READ_WRITE, Image::PIXEL_FORMAT_BGR, &data_c);
	//	create OpenCV Mat from Image::ImageInfo
	Image::ImageInfo cinfo = img_c->QueryInfo();
	colorBuffer = Mat(cinfo.height, cinfo.width, CV_8UC3);
	//	copy data
	colorBuffer.data = data_c.planes[0];
	//	release access
	img_c->ReleaseAccess(&data_c);
}

void RealSenseCVWrapper::getDepthBuffer()
{
	Image *img_d = sample->depth;
	Image::ImageData data_d;
	img_d->AcquireAccess(Image::ACCESS_READ_WRITE, Image::PIXEL_FORMAT_DEPTH_F32, &data_d);
	Image::ImageInfo dinfo = img_d->QueryInfo();
	depthBuffer = Mat(dinfo.height, dinfo.width, CV_32FC1);
	depthBuffer.data = data_d.planes[0];
	img_d->ReleaseAccess(&data_d);
}

void RealSenseCVWrapper::getMappedDepthBuffer()
{
	//	create projection stream to acquire mapped depth image
	Projection *projection = rsm->QueryCaptureManager()->QueryDevice()->CreateProjection();
	Image *depth_mapped = projection->CreateDepthImageMappedToColor(sample->depth, sample->color);
	//	acquire access to depth data
	Image::ImageData ddata_mapped;
	depth_mapped->AcquireAccess(Image::ACCESS_READ, Image::PIXEL_FORMAT_DEPTH_F32, &ddata_mapped);
	//	copy to cv::Mat
	Image::ImageInfo dinfo = depth_mapped->QueryInfo();
	depthBufferMapped = Mat(dinfo.height, dinfo.width, CV_32FC1);
	depthBufferMapped.data = ddata_mapped.planes[0];
	//	release access
	depth_mapped->ReleaseAccess(&ddata_mapped);
}

void RealSenseCVWrapper::getMappedColorBuffer()
{
	//	create projection stream to acquire mapped depth image
	Projection *projection = rsm->QueryCaptureManager()->QueryDevice()->CreateProjection();
	Image *color_mapped = projection->CreateColorImageMappedToDepth(sample->depth, sample->color);
	//	acquire access to depth data
	Image::ImageData cdata_mapped;
	color_mapped->AcquireAccess(Image::ACCESS_READ, Image::PIXEL_FORMAT_BGR, &cdata_mapped);
	//	copy to cv::Mat
	Image::ImageInfo cinfo = color_mapped->QueryInfo();
	colorBufferMapped = Mat(cinfo.height, cinfo.width, CV_8UC3);
	colorBufferMapped.data = cdata_mapped.planes[0];
	//	release access
	color_mapped->ReleaseAccess(&cdata_mapped);
}

void RealSenseCVWrapper::getXYZBuffer()
{
	Projection *projection = rsm->QueryCaptureManager()->QueryDevice()->CreateProjection();
	
	//Image *depth = projection->QueryVertices();
}

void RealSenseCVWrapper::getColorBuffer(cv::Mat & color)
{
	getColorBuffer();
	color = colorBuffer.clone();
}

void RealSenseCVWrapper::getDepthBuffer(cv::Mat & depth)
{
	getDepthBuffer();
	depth = depthBuffer.clone();
}

void RealSenseCVWrapper::getMappedDepthBuffer(cv::Mat & mappedDepth)
{
	getMappedDepthBuffer();
	mappedDepth = depthBufferMapped.clone();
}

void RealSenseCVWrapper::getMappedColorBuffer(cv::Mat & mappedColor)
{
	getMappedColorBuffer();
	mappedColor = colorBufferMapped.clone();
}

void RealSenseCVWrapper::getCalibrationStatus()
{
	//	aquire calibration data of RGB camera stream
	Projection * projection = rsm->QueryCaptureManager()->QueryDevice()->CreateProjection();
	Calibration::StreamCalibration *calib;
	Calibration::StreamTransform *trans;
	projection->QueryCalibration()
		->QueryStreamProjectionParameters(StreamType::STREAM_TYPE_COLOR, calib, trans);

	//	copy RGB camera calibration data to OpenCV
	cameraMatrix = Mat::eye(3, 3, CV_32FC1);
	cameraMatrix.at<float>(0, 0) = calib->focalLength.x;
	cameraMatrix.at<float>(1, 1) = calib->focalLength.y;
	cameraMatrix.at<float>(0, 2) = calib->principalPoint.x;
	cameraMatrix.at<float>(1, 2) = calib->principalPoint.y;

	distCoeffs = Mat::zeros(5, 1, CV_32FC1);
	distCoeffs.at<float>(0) = calib->radialDistortion[0];
	distCoeffs.at<float>(1) = calib->radialDistortion[1];
	distCoeffs.at<float>(2) = calib->tangentialDistortion[0];
	distCoeffs.at<float>(3) = calib->tangentialDistortion[1];
	distCoeffs.at<float>(4) = calib->radialDistortion[2];

	transform = Mat::eye(4, 4, CV_32FC1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			transform.at<float>(i, j) = trans->rotation[i][j];		//	todo: rotationÇÃçsÇ∆óÒÇÃèáÇämîF
		}
		transform.at<float>(i, 3) = trans->translation[i];
	}
}
