#include <iostream>
#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <random>

#define USE_FACE
#include "NtKinect.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>

using namespace std;

void doJob(unsigned int deg[8]) {
	NtKinect kinect;
	int j = 0;
	int i = 0;
	bool flag = false;

	int num = 15;
	double f_yaw[15] = { 0 }, f_pitch[15] = { 0 };
	double filter_gain = 0.9;
	double ave_yaw = 0, ave_pitch = 0;
	double mod_yaw = 0, pre_yaw = 0;
	double mod_pitch = 0, pre_pitch = 0;

	while (1) {
		kinect.setRGB();
		kinect.setSkeleton();
		for (auto person : kinect.skeleton) {
			for (auto joint : person) {
				if (joint.TrackingState == TrackingState_NotTracked) continue;
				ColorSpacePoint cp;
				kinect.coordinateMapper->MapCameraPointToColorSpace(joint.Position, &cp);
			}
		}
		kinect.setFace();
		for (vector<PointF> vf : kinect.facePoint) {
			for (PointF p : vf) {
				cv::rectangle(kinect.rgbImage, cv::Rect((int)p.X - 3, (int)p.Y - 3, 6, 6), cv::Scalar(0, 0, 255), 2);
			}
		}
		if (kinect.faceDirection.size() > 0) {
			//頭部方向
			cv::Vec3f dir = kinect.faceDirection[0];
			cv::putText(kinect.rgbImage, "pitch : " + to_string(dir[0]), cv::Point(200 * 0 + 50, 30), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 230, 0), 1);
			cv::putText(kinect.rgbImage, "yaw : " + to_string(dir[1]), cv::Point(200 * 0 + 50, 60), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 230, 0), 1);
			cv::putText(kinect.rgbImage, "roll : " + to_string(dir[2]), cv::Point(200 * 0 + 50, 90), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 230, 0), 1);

			//ローパスフィルタのみ
			//double filter_gain = 0.9;
			//mod_yaw = (double)(pre_yaw * filter_gain + dir[1] * (1 - filter_gain));
			//pre_yaw = mod_yaw;

			//平均値フィルタ
			for (int i = num - 1; i > 0; i--) {
				f_yaw[i] = f_yaw[i - 1];
				f_pitch[i] = f_pitch[i - 1];
			}
			f_yaw[0] = dir[1];
			f_pitch[0] = dir[0];
			for (i = 0; i < num; i++) {
				ave_yaw += f_yaw[i];
				ave_pitch += f_pitch[i];
			}
			ave_yaw = (double)ave_yaw / num;
			ave_pitch = (double)ave_pitch / num;
			//ローパスフィルタ
			mod_yaw = (double)(pre_yaw * filter_gain + ave_yaw * (1 - filter_gain));
			pre_yaw = mod_yaw;
			mod_pitch = (double)(pre_pitch * filter_gain + ave_pitch * (1 - filter_gain));
			pre_pitch = mod_pitch;

			// Start UDP
			WSAData wsaData;
			SOCKET sock;
			struct sockaddr_in addr;
			WSAStartup(MAKEWORD(2, 0), &wsaData);
			sock = socket(AF_INET, SOCK_DGRAM, 0);
			addr.sin_family = AF_INET;
			addr.sin_port = htons(52525);
			//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			InetPton(addr.sin_family, _T("127.0.0.1"), &addr.sin_addr.S_un.S_addr);
			//string str = to_string(dir[1]) + "," + to_string(dir[0]);
			string str = to_string(mod_yaw) + "," + to_string(mod_pitch) + "," + "e";
			const char* cstr = str.c_str();
			sendto(sock, cstr, 23, 0, (struct sockaddr*) & addr, sizeof(addr));
			closesocket(sock);
			WSACleanup();
			// End UDP


			//if (j <= 10 && flag) {
			//	j++;
			//	std::string str1, str2;
			//	str1 = std::to_string(deg[i]);
			//	str2 = std::to_string(j);
			//	std::string str = str1 + "_" + str2 + ".png";
			//	//imwrite("C:\\Users\\sens\\Documents\\EH_program\\Test\\KinectFaceTracker(Experiment0)\\Image\\Takezawa_img_" + str, kinect.rgbImage);
			//	//imwrite("C:\\Users\\sens\\Documents\\EH_program\\Test\\KinectFaceTracker(Experiment)\\Image\\img_" + str, kinect.rgbImage);
			//}
			//if (j > 10 && flag) {
			//	flag = false;
			//	cout << std::to_string(deg[i]) + "�‹ Finish!!\n";
			//	i++;
			//	if (i >= 8) {
			//		break;
			//	}
			//}
		}

		//画像のリサイズ
		cv::Mat resize_img;
		resize(kinect.rgbImage, resize_img, cv::Size(), 0.5, 0.5);
		cv::imshow("rgb", resize_img);
		auto key = cv::waitKey(1);
		if (key == 'q') break;
		else if (key == 'g') {
			cout << std::to_string(deg[i]) + "�‹ Catch\n";
			flag = true;
			j = 0;
		}
	}

	while (1) {
		kinect.setRGB();
		kinect.setSkeleton();
		for (auto person : kinect.skeleton) {
			for (auto joint : person) {
				if (joint.TrackingState == TrackingState_NotTracked) continue;
				ColorSpacePoint cp;
				kinect.coordinateMapper->MapCameraPointToColorSpace(joint.Position, &cp);
			}
		}
		kinect.setFace();
		for (vector<PointF> vf : kinect.facePoint) {
			for (PointF p : vf) {
				cv::rectangle(kinect.rgbImage, cv::Rect((int)p.X - 3, (int)p.Y - 3, 6, 6), cv::Scalar(0, 0, 255), 2);
			}
		}
		if (kinect.faceDirection.size() > 0) {
			cv::Vec3f dir = kinect.faceDirection[0];
			if (j <= 10 && flag) {
				j++;
				std::string str1, str2;
				str1 = std::to_string(15);
				str2 = std::to_string(j);
				std::string str = str1 + "_" + str2 + ".png";
				//imwrite("C:\\Users\\sens\\Documents\\EH_program\\Test\\KinectFaceTracker(Experiment0)\\Image\\Ryu2_img_" + str, kinect.rgbImage);
				//imwrite("C:\\Users\\sens\\Documents\\EH_program\\Test\\KinectFaceTracker(Experiment)\\Image\\img_" + str, kinect.rgbImage);
			}
			if (j > 10 && flag) {
				flag = false;
				break;
			}
		}
		cv::imshow("rgb", kinect.rgbImage);
		auto key = cv::waitKey(1);
		if (key == 'q') break;
		else if (key == 'g') {
			cout << std::to_string(deg[i]) + "�‹ Catch\n";
			flag = true;
			j = 0;
		}
	}


	cv::destroyAllWindows();
}

int main(int argc, char** argv) {
	unsigned  int deg[8];
	std::vector<int> v = { 0,1,2,3,4,5,6,7 };
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	std::shuffle(v.begin(), v.end(), engine);
	//for (int i = 0; i < 8; i++) {

	//	if (v[i] < 4) {
	//		deg[i] = v[i] * 15;
	//	}
	//	else {
	//		deg[i] = 180 - (7 - v[i]) * 15;
	//	}
	//	cout << std::to_string(i) + "”Ô–Ú " + std::to_string(deg[i]) + "\n";
	//}


	try {
		doJob(deg);
	}
	catch (exception & ex) {
		cout << ex.what() << endl;
		string s;
		cin >> s;
	}
	return 0;
}