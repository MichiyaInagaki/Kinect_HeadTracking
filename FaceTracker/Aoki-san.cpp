#include <iostream>
#include <sstream>
​
#define USE_FACE
#include "NtKinect.h"
​
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>
​
using namespace std;
​
string faceProp[] = {
	"happy", "engaed", "glass", "leftEyeClosed",
	"rightEyeClosed", "mouseOpen", "mouseMoved", "lookingAway"
};
string dstate[] = { "unknown", "no", "maybe", "yes" };
​
void doJob() {
	NtKinect kinect;
	while (1) {
		kinect.setRGB();
		kinect.setSkeleton();
		for (auto person : kinect.skeleton) {
			for (auto joint : person) {
				if (joint.TrackingState == TrackingState_NotTracked) continue;
				ColorSpacePoint cp;
				kinect.coordinateMapper->MapCameraPointToColorSpace(joint.Position, &cp);
				cv::rectangle(kinect.rgbImage, cv::Rect((int)cp.X - 5, (int)cp.Y - 5, 10, 10), cv::Scalar(0, 0, 255), 2);
			}
		}
		kinect.setFace();
		for (cv::Rect r : kinect.faceRect) {
			cv::rectangle(kinect.rgbImage, r, cv::Scalar(255, 255, 0), 2);
		}
		for (vector<PointF> vf : kinect.facePoint) {
			for (PointF p : vf) {
				cv::rectangle(kinect.rgbImage, cv::Rect((int)p.X - 3, (int)p.Y - 3, 6, 6), cv::Scalar(0, 255, 255), 2);
			}
		}
​
		/*
		for (int p = 0; p < kinect.faceDirection.size(); p++) {
			cv::Vec3f dir = kinect.faceDirection[p];
			cv::putText(kinect.rgbImage, "pitch : " + to_string(dir[0]), cv::Point(200 * p + 50, 30), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 230, 0), 1, CV_AA);
			cv::putText(kinect.rgbImage, "yaw : " + to_string(dir[1]), cv::Point(200 * p + 50, 60), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 230, 0), 1, CV_AA);
			cv::putText(kinect.rgbImage, "roll : " + to_string(dir[2]), cv::Point(200 * p + 50, 90), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 230, 0), 1, CV_AA);
​
			// Start UDP
			WSAData wsaData;
​
			SOCKET sock;
			struct sockaddr_in addr;
​
			WSAStartup(MAKEWORD(2, 0), &wsaData);
​
			sock = socket(AF_INET, SOCK_DGRAM, 0);
​
			addr.sin_family = AF_INET;
			addr.sin_port = htons(52525);
			//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			InetPton(addr.sin_family, _T("127.0.0.1"), &addr.sin_addr.S_un.S_addr);
​
			string str = "f," + to_string(dir[0]) + "," + to_string(dir[1]);
			const char* cstr = str.c_str();
​
			sendto(sock, cstr, 23, 0, (struct sockaddr *)&addr, sizeof(addr));
​
			closesocket(sock);
​
			WSACleanup();
			// End UDP
		}
		*/
​
		if (kinect.faceDirection.size() > 0) {
			cv::Vec3f dir = kinect.faceDirection[0];
			cv::putText(kinect.rgbImage, "pitch : " + to_string(dir[0]), cv::Point(200 * 0 + 50, 30), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 230, 0), 1, CV_AA);
			cv::putText(kinect.rgbImage, "yaw : " + to_string(dir[1]), cv::Point(200 * 0 + 50, 60), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 230, 0), 1, CV_AA);
			cv::putText(kinect.rgbImage, "roll : " + to_string(dir[2]), cv::Point(200 * 0 + 50, 90), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 230, 0), 1, CV_AA);
​
			// Start UDP
			WSAData wsaData;
​
			SOCKET sock;
			struct sockaddr_in addr;
​
			WSAStartup(MAKEWORD(2, 0), &wsaData);
​
			sock = socket(AF_INET, SOCK_DGRAM, 0);
​
			addr.sin_family = AF_INET;
			addr.sin_port = htons(52525);
			//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			InetPton(addr.sin_family, _T("127.0.0.1"), &addr.sin_addr.S_un.S_addr);
​
			string str = "f," + to_string(dir[0]) + "," + to_string(dir[1]);
			const char* cstr = str.c_str();
​
			sendto(sock, cstr, 23, 0, (struct sockaddr *)&addr, sizeof(addr));
​
			closesocket(sock);
​
			WSACleanup();
			// End UDP
		}
​
		/*
		for (int p = 0; p < kinect.faceProperty.size(); p++) {
			for (int k = 0; k < FaceProperty_Count; k++) {
				int v = kinect.faceProperty[p][k];
				cv::putText(kinect.rgbImage, faceProp[k] + " : " + dstate[v], cv::Point(200 * p + 50, 30 * k + 120), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(232, 232, 0), 1, CV_AA);
			}
		}
		*/
​
		cv::imshow("rgb", kinect.rgbImage);
		auto key = cv::waitKey(1);
		if (key == 'q') break;
	}
	cv::destroyAllWindows();
}
​
int main(int argc, char** argv) {
	try {
		doJob();
	}
	catch (exception &ex) {
		cout << ex.what() << endl;
		string s;
		cin >> s;
	}
	return 0;
}