#pragma once
#include <opencv2\opencv.hpp>
#include <RealSense\SenseManager.h>
#include <RealSense\SampleReader.h>

#pragma comment(lib, "libpxc.lib")

class RealSenseCVWrapper
{
public:
	//	constructor
	//	@param
	//		width : width of picture size
	//		height: height of picture size
	RealSenseCVWrapper(int width, int height);
	~RealSenseCVWrapper();

	//	query RealSense stream
	//	@return
	//		true = stream is succcessfully queried
	bool queryStream();

	//	get camera buffers
	void getColorBuffer();
	void getDepthBuffer();
	void getMappedDepthBuffer();
	void getMappedColorBuffer();
	void getXYZBuffer();
	void getColorBuffer(cv::Mat &color);
	void getDepthBuffer(cv::Mat &depth);
	void getMappedDepthBuffer(cv::Mat &mappedDepth);
	void getMappedColorBuffer(cv::Mat &mappedColor);
	void getXYZBuffer(cv::Mat &xyz);
	
	//	get calibration status
	void getCalibrationStatus();

	//	OpenCV image buffer
	cv::Size bufferSize;		//	buffer size
	cv::Mat colorBuffer;		//	RGB camera buffer (BGR, 8UC3)
	cv::Mat depthBuffer;		//	depth camera buffer (Gray, 32FC1)
	cv::Mat colorBufferMapped;	//	RGB camera buffer mapped to depth camera (BGR, 8UC3)
	cv::Mat depthBufferMapped;	//	depth camera buffer mapped to RGB camera (Gray, 32FC1)
	std::vector<cv::Point3f> xyzBuffer;			//	XYZ point cloud buffer from depth camera (XYZ, 32FC3)

	//	RGB camera calibration data writtern by OpenCV camera model
	cv::Mat cameraMatrix;		//	inclueds fx,fy,cx,cy
	cv::Mat distCoeffs;			//	k1,k2,p1,p2,k3
	cv::Mat transform;			//	4x4 coordinate transformation from RGB camera origin to the world (=depth) system origin

protected:
	Intel::RealSense::SenseManager *rsm;
	Intel::RealSense::Sample *sample;

};

