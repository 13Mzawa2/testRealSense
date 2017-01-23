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
		rscv.getColorBuffer(src);
		//	�f�v�X�摜�Ƀ}�b�v�����J���[�摜���擾
		//rscv.getMappedColorBuffer(src);
		//	���_���W�̌��z�𐄒肵�ĉA�e�\��
		rscv.getXYZBuffer();
		cv::Mat XYZImage(rscv.bufferSize, CV_32FC1);
		for (int i = 1; i < rscv.bufferSize.height-1; i++) {
			for (int j = 1; j < rscv.bufferSize.width-1; j++) {
				//	��4���ւ̌��z�x�N�g�����Z�o
				float intensity = 0.0f;
				cv::Point3f v0, v1, v2, v3, v4;
				v0 = rscv.xyzBuffer[i*rscv.bufferSize.width + j];
				if (v0.z != 0) {
					v1 = rscv.xyzBuffer[(i - 1)*rscv.bufferSize.width + (j - 1)] - v0;
					v2 = rscv.xyzBuffer[(i - 1)*rscv.bufferSize.width + (j + 1)] - v0;
					v3 = rscv.xyzBuffer[(i + 1)*rscv.bufferSize.width + (j + 1)] - v0;
					v4 = rscv.xyzBuffer[(i + 1)*rscv.bufferSize.width + (j - 1)] - v0;
					//	���ꂼ��̌��z����@���x�N�g����4��ގZ�o���ĕ���
					//	v1    v2
					//	   v0
					//	v4    v3
					//	�@�������͎�O������
					cv::Point3f n0, n1, n2, n3, n4;
					n1 = v2.cross(v1);
					n2 = v3.cross(v2);
					n3 = v4.cross(v3);
					n4 = v1.cross(v4);
					n0 = (n1 + n2 + n3 + n4) / cv::norm(n1 + n2 + n3 + n4);
					cv::Point3f light(3.f, 3.f, 0.f);
					intensity = n0.ddot(light/cv::norm(light));
				}
				else {
					intensity = 0.f;
				}

				XYZImage.at<float>(i, j) = intensity;
			}
		}
		//	RealSense�̃X�g���[�����I��
		rscv.releaseFrames();

		cv::imshow("ts", src);
		cv::imshow("xyz", XYZImage);
		int c = cv::waitKey(10);
		if (c == 27) break;
		if (c == 'a') {
			rscv.useAutoAdjust(false);
			rscv.setExposure(-8);
			rscv.setWhiteBalance(6000);
		}
		if (c == 'A') {
			rscv.useAutoAdjust(true);
		}
	}

	return 0;
}