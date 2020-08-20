#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <fstream>
#include <sys/time.h>

#include "yolo_v2_class.hpp"

using namespace std;
using namespace cv;

#define GPU


//画出检测框和相关信息
void DrawBoxes(Mat &frame, vector<string> classes, int classId, float conf, int left, int top, int right, int bottom)
{
	//画检测框
	rectangle(frame, Point(left, top), Point(right, bottom), Scalar(255, 178, 50), 3);

	//该检测框对应的类别和置信度
	string label = format("%.2f", conf);
	if (!classes.empty())
	{
		CV_Assert(classId < (int)classes.size());
		label = classes[classId] + ":" + label;
	}

	//将标签显示在检测框顶部
	int baseLine;
	Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
	top = max(top, labelSize.height);
	rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
	putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 1);
}


//画出检测结果
void Drawer(Mat &frame, vector<bbox_t> outs, vector<string> classes)
{
	//获取所有最佳检测框信息
	for (int i = 0; i < outs.size(); i++)
	{
		DrawBoxes(frame, classes, outs[i].obj_id, outs[i].prob, outs[i].x, outs[i].y,
			outs[i].x + outs[i].w, outs[i].y + outs[i].h);
	}
}


int main(void)
{
	struct timeval tv1,tv2;
	long long T;
	string classesFile = "./yolo/coco.names";
    string modelConfig = "./yolo/yolov4.cfg";
    string modelWeights = "./yolo/yolov4.weights";

    //加载类别名
    vector<string> classes;
	ifstream ifs(classesFile.c_str());
	string line;
	while (getline(ifs, line)) classes.push_back(line);
	//加载网络模型，0是指定第一块GPU
    Detector detector(modelConfig, modelWeights, 0);

	string mode = "video";
	//图像
	if (mode == "image")
	{
		Mat frame = imread("./data/test.jpg");

		//开始计时
		gettimeofday(&tv1, NULL);
		//Mat图像转为yolo输入格式
		shared_ptr<image_t> detImg = detector.mat_to_image_resize(frame);
		//前向预测
		vector<bbox_t> outs = detector.detect_resized(*detImg, frame.cols, frame.rows, 0.25);
		//画图
		Drawer(frame, outs, classes);
		//结束计时
		gettimeofday(&tv2, NULL);
		//计算用时
		T = (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000;
		cout << T << "ms" <<endl;
		
		imwrite("./data/result.jpg", frame);
	}
	//视频
	else if (mode == "video")
	{
		VideoCapture cap("./data/test.avi");
		Size size(1920, 1080);
		VideoWriter writer("./data/result.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, size);
		int i=0;
		while (1) 
		{
			i++;
			if(i==256)
				break;
			Mat frame;
			cap >> frame;
			if (frame.empty()) 
				break;

			//开始计时
			gettimeofday(&tv1, NULL);
			//Mat图像转为yolo输入格式
			shared_ptr<image_t> detImg = detector.mat_to_image_resize(frame);
			//前向预测
			vector<bbox_t> outs = detector.detect_resized(*detImg, frame.cols, frame.rows, 0.25);
			//画图
			Drawer(frame, outs, classes);
			//结束计时
			gettimeofday(&tv2, NULL);
			//计算用时
			T = (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000;
			cout << T << "ms" <<endl;

			writer << frame;
		}
		cap.release();
	}
    return 0;
}
