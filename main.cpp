#include <opencv2/opencv.hpp>

#include <cmath>
#include <iostream>

#include "MotionDetection.h"
#include "FaceDetection.h"
#include "PersonArea.h"

#define QUEUE_LEN 7

using namespace cv;
using namespace std;

int main() {
	VideoCapture cam(0);
	VideoWriter res;
	if (!cam.isOpened())
		return -1;
	namedWindow("result", WINDOW_FREERATIO);

	Mat previousFrame; // previous frame for motion detection
	cam >> previousFrame;
	cvtColor(previousFrame, previousFrame, CV_BGR2GRAY);

	vector<Rect> faces = detectFaces(previousFrame);
	if (faces.empty()) {
		cout << "No faces found!" << endl;
		return -1;
	}
	vector<PersonArea*> person_areas;
	for (Rect face : faces) {
		cout << face.x << ' ' << face.y << endl;
		person_areas.push_back(new PersonArea(face, QUEUE_LEN, previousFrame.size()));
	}

	while (true) {
		Mat img;
		cam >> img;
		if (img.empty())
			break;

		Mat motion = detectMotion(img, previousFrame);

		for (auto pa : person_areas) {
			pa->update(motion);
		}

		Mat result;
		cvtColor(motion, result, COLOR_GRAY2BGR);

		for (auto pa : person_areas) {
			pa->drawOn(img);
			pa->drawGraphOn(img);
		}

		if (res.isOpened())
			res << img;
		else
			res = *new VideoWriter("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 50, result.size());
		imshow("result", img);
		int keyCode = waitKey(5);
		if (keyCode == 27)
			break;
	}
}