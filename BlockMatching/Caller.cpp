#include "Caller.h"
#include "Utility.h"
#include "Device.cuh"
#include "BlockMatching.h"
#include "guidedfilter.h"

using namespace std;
using namespace cv;

int cameraTest()
{
	int id, key;
	Mat img;
	VideoCapture cam;
	while (true)
	{
		cin >> id;
		cam.open(id);
		cout << cam.get(CV_CAP_PROP_FRAME_WIDTH) << '*' << cam.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
		cout << cam.set(CV_CAP_PROP_FRAME_WIDTH, 320) << endl;
		cout << cam.set(CV_CAP_PROP_FRAME_HEIGHT, 240) << endl;
		cout << cam.get(CV_CAP_PROP_FRAME_WIDTH) << '*' << cam.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
		if (!cam.isOpened())
		{
			cout << "Can Not Open\n";
			return -1;
		}


		while (true)
		{
			cam >> img;
			imshow("Camera " + to_string(id), img);
			key = waitKey(5);
			if (key == 'q')
			{
				break;
			}
			else if (key == 'i')
				cout << img.rows << '*' << img.cols << endl;
		}
	}
}

//void singleFrame()
//{
//	Mat left, right, g1, g2, disp, disp2;
//	left = imread("./../Images/Art/view1_.png");
//	right = imread("./../Images/Art/view5_.png");
//
//	cvtColor(left, g1, CV_BGR2GRAY);
//	cvtColor(right, g2, CV_BGR2GRAY);
//
//	Device device;
//	clock_t start = clock();
//	device.blockMatching_gpu(g1, g2, disp, 5, 64);
//	clock_t end = clock();
//	cout << "GPU : " << (double)(end - start) / CLOCKS_PER_SEC << endl;
//
//	imshow("disp", disp);
//	waitKey(0);
//}
//
//void remapTest()
//{
//	Mat mapX1, mapY1, mapX2, mapY2;
//	// open file
//	Mat left_org = imread("./../Chess/Set2/Left_0.jpg");
//	Mat right_org = imread("./../Chess/Set2/Right_0.jpg");
//	Mat left, right, left_rec, right_rec;
//	Mat camMat1, camMat2, distCoe1, distCoe2, R, T;
//	Size targetSize = Size(320, 200);
//	int rows = targetSize.height, cols = targetSize.width;
//	int total = rows * cols;
//
//	// convert to gray map
//	cvtColor(left_org, left, CV_BGR2GRAY);
//	cvtColor(right_org, right, CV_BGR2GRAY);
//
//	// resize to small size
//	resize(left, left, targetSize);
//	resize(right, right, targetSize);
//
//	// load camera parameter
//	LoadDataBatch("./../Calib_Data_OpenCV.yml", camMat1, camMat2, distCoe1, distCoe2, R, T);
//
//	// rectify the camera
//	Rectify(camMat1, camMat2, distCoe1, distCoe2, R, T, targetSize, mapX1, mapY1, mapX2, mapY2);
//
//	// return value
//	uchar *cpu_result, *gpu_result;
//	cpu_result = new uchar[total];
//	gpu_result = new uchar[total];
//
//	// execute on cpu and gpu
//	Device d;
//	remap_cpu(left, right, mapX1, mapY1, mapX2, mapY2, total, cpu_result);
//	d.remap_gpu(left, right, mapX1, mapY1, mapX2, mapY2, rows, cols, total, gpu_result);
//
//	//for (size_t i = 0; i < total; i++)
//	//{
//	//	if (cpu_result[i] != gpu_result[i])
//	//	{
//	//		cout << i << endl;
//	//	}
//	//}
//
//	imshow("cpu", Mat(rows, cols, CV_8UC1, cpu_result));
//	imshow("gpu", Mat(rows, cols, CV_8UC1, gpu_result));
//
//	waitKey(0);
//}
//
//void cvtColorTest()
//{
//	Mat left_org = imread("./../Chess/Set2/Left_0.jpg");
//	Mat right_org = imread("./../Chess/Set2/Right_0.jpg");
//	Mat left, right;
//	Size targetSize = Size(320, 200);
//	int rows = targetSize.height, cols = targetSize.width, total = rows * cols;
//	uchar *left_gray = new uchar[total];
//	uchar *right_gray = new uchar[total];
//	int count = 0;
//	resize(left_org, left, targetSize);
//	resize(right_org, right, targetSize);
//
//	clock_t	 start, end;
//	start = clock();
//	while (count ++ < 1000)
//		cvtColor_impl((uchar3 *)left.data, left_gray, rows, cols);
//	// cvtColor_impl((uchar3 *)right_org.data, right_gray, rows, cols);
//	end = clock();
//	cout << "self : " << (double)(end - start) / CLOCKS_PER_SEC << endl;
//
//	count = 0;
//	Mat left_ref, right_ref;
//	start = clock();
//	while (count++ < 1000)
//		cvtColor(left, left_ref, CV_BGR2GRAY);
//	// cvtColor(right, right_ref, CV_BGR2GRAY);
//	end = clock();
//	cout << "opencv : " << (double)(end - start) / CLOCKS_PER_SEC << endl;
//
//	// cvtColor_gpu((uchar3 *)left.data, left_gray, rows, cols);
//
//	imshow("ref", left_ref);
//	imshow("cal", Mat(rows, cols, CV_8UC1, left_gray));
//
//	waitKey(0);
//}

void streamDepth(Size targetSize, int SADWindowSize, int numDisp)
{
	VideoCapture camL, camR;
	Mat left, right;
	camL.open(0);
	if (!camL.isOpened())
	{
		cout << "Left Camera Could Not Open!\n";
		return;
	}
	camR.open(1);
	if (!camR.isOpened())
	{
		cout << "Right Camera Could Not Open!\n";
		return;
	}

	cout << "Check the Camera's Relative Position\n";
	int sw;
	while (true)
	{
		camL >> left;
		camR >> right;
		imshow("Temp_Left", left);
		imshow("Temp_Right", right);
		sw = waitKey(1);
		if (sw == ' ')
		{
			break;
		}
		else if (sw == 'p')
		{
			camL.release();
			camR.release();
			camL.open(1);
			camR.open(0);
		}
	}
	destroyAllWindows();
	
	Mat x1, y1, x2, y2;
	// calib camera
	getCalibResult(targetSize, x1, y1, x2, y2);

	// init the gpu
	Device device(targetSize, numDisp, SADWindowSize, x1, y1, x2, y2);

	// start processing
	while (true)
	{
		// read input from camera
		camL >> left;
		camR >> right;
		device.pipeline(left, right);
		waitKey(1);
	}
}

void photoDepth(Size targetSize, int SADWindowSize, int numDisp)
{
	VideoCapture camL, camR;
	camL.open(0);
	if (!camL.isOpened())
	{
		cout << "Left Camera Could Not Open!\n";
		return;
	}
	camR.open(1);
	if (!camR.isOpened())
	{
		cout << "Right Camera Could Not Open!\n";
		return;
	}



	Mat x1, y1, x2, y2;
	// calib camera
	getCalibResult(targetSize, x1, y1, x2, y2);

	// init the gpu
	Device device(targetSize, numDisp, SADWindowSize, x1, y1, x2, y2);

	// start processing
	Mat left, right;
	int key;
	while (true)
	{
		// read input from camera
		camL >> left;
		camR >> right;
		key = waitKey(1);
		if (key == ' ')
		{
			device.pipeline(left, right);
		}
		imshow("Left", left);
		imshow("Right", right);
	}
}

int Calib()
{
	return StereoCalib();
}

void OpenCVBM()
{
	Mat left, right, gl, gr, disp;
	left = imread("./../Images/Art/view1.png");
	right = imread("./../Images/Art/view5.png");
	imshow("left", left);
	imshow("right", right);

	StereoBM bm;
	bm.state->SADWindowSize = 11;
	bm.state->numberOfDisparities = 64;
	bm.state->minDisparity = 0;

	cvtColor(left, gl, CV_BGR2GRAY);
	cvtColor(right, gr, CV_BGR2GRAY);

	bm(gl, gr, disp);
	imshow("disp", disp);
	waitKey(0);
}

int start_gpu(Size targetSize, int windowSize, int numDisp)
{
	//Calib();
	streamDepth(targetSize, windowSize, numDisp);
	return 0;
}

int start_cpu(Size targetSize, int windowSize, int numDisp)
{

	int key;
	VideoCapture camL, camR;
	Mat left, right;
	camL.open(0);
	if (!camL.isOpened())
	{
		cout << "Left Camera Could Not Open!\n";
		return -1;
	}
	camR.open(1);
	if (!camR.isOpened())
	{
		cout << "Right Camera Could Not Open!\n";
		return -1;
	}

	cout << "Check the Camera's Relative Position\n";
	int sw;
	while (true)
	{
		camL >> left;
		camR >> right;
		imshow("Temp_Left", left);
		imshow("Temp_Right", right);
		sw = waitKey(1);
		if (sw == ' ')
		{
			break;
		}
		else if (sw == 'p')
		{
			camL.release();
			camR.release();
			camL.open(1);
			camR.open(0);
		}
	}
	destroyAllWindows();

	Mat x1, y1, x2, y2;
	// calib camera
	getCalibResult(targetSize, x1, y1, x2, y2);

	// init the gpu
	Host host(targetSize, numDisp, windowSize, x1, y1, x2, y2);

	clock_t start, end;
	// start processing
	while (true)
	{
		// read input from camera
		camL >> left;
		camR >> right;
		imshow("Left_Org", left);
		imshow("Right_Org", right);

		key = waitKey(1);
		
		if (key == ' ')
		{
			start = clock();
			host.calculateFrame(left, right);
			end = clock();
			cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
		}
	}
	return 0;
}

int start_opencv(Size targetSize, int windowSize, int numDisp)
{
	//Calib();
	int key;
	VideoCapture camL, camR;
	Mat left, right, g1, g2, left_rec, right_rec, disparity, disp_normed;
	camL.open(0);
	if (!camL.isOpened())
	{
		cout << "Left Camera Could Not Open!\n";
		return -1;
	}
	camR.open(1);
	if (!camR.isOpened())
	{
		cout << "Right Camera Could Not Open!\n";
		return -1;
	}

	cout << "Check the Camera's Relative Position\n";
	int sw;
	while (true)
	{
		camL >> left;
		camR >> right;
		imshow("Temp_Left", left);
		imshow("Temp_Right", right);
		sw = waitKey(1);
		if (sw == ' ')
		{
			break;
		}
		else if (sw == 'p')
		{
			camL.release();
			camR.release();
			camL.open(1);
			camR.open(0);
		}
	}
	destroyAllWindows();

	Mat x1, y1, x2, y2;
	// calib camera
	getCalibResult(targetSize, x1, y1, x2, y2);

	StereoBM sbm;
	sbm.state->SADWindowSize = windowSize * 2 + 1;
	sbm.state->numberOfDisparities = numDisp;
	sbm.state->minDisparity = 0;

	clock_t start, end;
	// start processing
	while (true)
	{
		// read input from camera
		camL >> left;
		camR >> right;
		imshow("Left_Org", left);
		imshow("Right_Org", right);

		key = waitKey(1);

		//if (key == ' ')
		//{
			start = clock();
			resize(left, left, targetSize);
			resize(right, right, targetSize);
			cvtColor(left, g1, CV_BGR2GRAY);
			cvtColor(right, g2, CV_BGR2GRAY);
			remap(g1, left_rec, x1, y1, INTER_LINEAR, BORDER_CONSTANT, 0);
			remap(g2, right_rec, x2, y2, INTER_LINEAR, BORDER_CONSTANT, 0);
			imshow("left_rec", left_rec);
			imshow("right_rec", right_rec);
			sbm(left_rec, right_rec, disparity);
			normalize(disparity, disp_normed, 0, 255, CV_MINMAX, CV_8UC1);
			imshow("Disp", guidedFilter(disp_normed, disp_normed, 2, 0.02 * 0.02 * 255 * 255));
			end = clock();
			cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
		//}
	}
	return 0;
}