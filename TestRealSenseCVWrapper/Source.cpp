#include "../testRealSense/RealSenseCVWrapper.h"

int main(void)
{
	RealSenseCVWrapper rscv(640, 480);
	//rscv.queryFrames();
	rscv.getCalibrationStatus();
	//rscv.useAutoAdjust(false);
	
	std::cout << "calibration data: "
		<< "\n camera matrix = \n" << rscv.cameraMatrix
		<< "\n camera distortion params = \n" << rscv.distCoeffs
		<< "\n transform matrix of RGB cam = \n" << rscv.transform
		<< std::endl;

	while (1) {
		cv::Mat src;
		//	RealSense�̃X�g���[�����J�n
		rscv.queryFrames();
		//rscv.getColorBuffer(src);
		//	�f�v�X�摜�Ƀ}�b�v�����J���[�摜���擾
		rscv.getMappedColorBuffer(src);
		//	RealSense�̃X�g���[�����I��
		rscv.releaseFrames();

		cv::imshow("ts", src);
		int c = cv::waitKey(10);
		if (c == 27) break;
		if (c == 'a') {
			rscv.useAutoAdjust(false);
			rscv.setExposure(-5);
			rscv.setWhiteBalance(6000);
		}
		if (c == 'A') {
			rscv.useAutoAdjust(true);
		}
	}

	return 0;
}