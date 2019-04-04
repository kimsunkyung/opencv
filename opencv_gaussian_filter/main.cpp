#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
#define IM_TYPE	CV_8UC3

using namespace cv;

// Image Type
// "G" for GrayScale Image, "C" for Color Image
#if (IM_TYPE == CV_8UC3)
typedef uchar G;
typedef cv::Vec3b C;
#elif (IM_TYPE == CV_16SC3)
typedef short G;
typedef Vec3s C;
#elif (IM_TYPE == CV_32SC3)
typedef int G;
typedef Vec3i C;
#elif (IM_TYPE == CV_32FC3)
typedef float G;
typedef Vec3f C;
#elif (IM_TYPE == CV_64FC3)
typedef double G;
typedef Vec3d C;
#endif

Mat gaussianfilter_gray(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);
Mat gaussianfilter_rgb(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);
int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;
	Mat output2;

	cvtColor(input, output, COLOR_RGB2GRAY);

	if (!input.data)
	{
		return -1;
	}
	
	namedWindow("Gray", WINDOW_AUTOSIZE);
	imshow("Gray", output);

	namedWindow("Color", WINDOW_AUTOSIZE);
	imshow("Color", input);
	
	output = gaussianfilter_gray(output, 1, 1, 1, "mirroring"); //Boundary process: zero-paddle, mirroring, adjustkernel
	output2 = gaussianfilter_rgb(input, 1, 1, 1, "zero-paddle"); //Boundary process: zero-paddle, mirroring, adjustkernel


	namedWindow("Gaussian Filter_Gray", WINDOW_AUTOSIZE);
	imshow("Gaussian Filter_Gray", output);

	namedWindow("Gaussian Filter_RGB", WINDOW_AUTOSIZE);
	imshow("Gaussian Filter_RGB", output2);


	waitKey(0);

	return 0;
}


Mat gaussianfilter_rgb(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {

	Mat kernel;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom;
	float kernelvalue;

	// Initialiazing Kernel Matrix 
	kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);


	denom = 0.0;
	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))) - (pow(b, 2) / (2 * pow(sigmaT, 2))));
			kernel.at<float>(a + n, b + n) = value1;
			denom += value1;
		}
	}

	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			kernel.at<float>(a + n, b + n) /= denom;
		}
	}

	Mat output = Mat::zeros(row, col, input.type());
	Mat output2 = Mat::zeros(row, col, input.type());


	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (!strcmp(opt, "zero-paddle")) {
				float sum1[3] = { 0.0,0.0,0.0 };
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {

						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							kernelvalue = kernel.at<float>(a + n, b + n);
							for (int k = 0; k < 3; k++) {
								sum1[k] += kernelvalue * (float)(input.at<Vec3b>(i + a, j + b)[k]);
							}
						}
					}
				}
				for (int k = 0; k < 3; k++) {
					output.at<Vec3b>(i, j)[k] = sum1[k];
				}
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1[3] = { 0.0,0.0,0.0 };
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						kernelvalue = kernel.at<float>(a + n, b + n);

						if (i + a > row - 1) {  //mirroring for the border pixels
							tempa = i - a;
						}
						else if (i + a < 0) {
							tempa = -(i + a);
						}
						else {
							tempa = i + a;
						}
						if (j + b > col - 1) {
							tempb = j - b;
						}
						else if (j + b < 0) {
							tempb = -(j + b);
						}
						else {
							tempb = j + b;
						}
						for (int k = 0; k < 3; k++) {
							sum1[k] += kernelvalue * (float)(input.at<Vec3b>(tempa,tempb)[k]);
						}
				
					}
				}
				for (int k = 0; k < 3; k++) {
					output.at<Vec3b>(i, j)[k] = sum1[k];
				}
			}


			else if (!strcmp(opt, "adjustkernel")) {
				float sum1[3] = { 0.0,0.0,0.0 };
				float sum2[3] = { 0.0,0.0,0.0 };
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						kernelvalue = kernel.at<float>(a + n, b + n);
						for (int k = 0; k < 3; k++) {
							sum1[k] += kernelvalue * (float)(input.at<Vec3b>(i + a, j + b)[k]);
							sum2[k] += kernelvalue;
						}
					}
				}
				for (int k = 0; k < 3; k++) {
					output.at<Vec3b>(i, j)[k] = sum1[k];
				}
			}
		}
	}
	return output;
}

Mat gaussianfilter_gray(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {

	Mat kernel;
	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom;
	float kernelvalue;
	//std::cout << input.at<Vec3b>(0, 0) << std::endl;
	// Initialiazing Kernel Matrix 
	kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);


	denom = 0.0;
	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))) - (pow(b, 2) / (2 * pow(sigmaT, 2))));
			kernel.at<float>(a + n, b + n) = value1;
			denom += value1;
		}
	}

	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			kernel.at<float>(a + n, b + n) /= denom;
			//std::cout << kernel << std::endl;
		}
	}

	Mat output = Mat::zeros(row, col, input.type());
	Mat output2 = Mat::zeros(row, col, input.type());


	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (!strcmp(opt, "zero-paddle")) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {

						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							kernelvalue = kernel.at<float>(a + n, b + n);
							sum1 += kernelvalue * (float)(input.at<G>(i + a, j + b));
						}
					}
				}
				output.at<G>(i, j) = (G)sum1;
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						kernelvalue = kernel.at<float>(a + n, b + n);

						if (i + a > row - 1) {  //mirroring for the border pixels
							tempa = i - a;
						}
						else if (i + a < 0) {
							tempa = -(i + a);
						}
						else {
							tempa = i + a;
						}
						if (j + b > col - 1) {
							tempb = j - b;
						}
						else if (j + b < 0) {
							tempb = -(j + b);
						}
						else {
							tempb = j + b;
						}
						sum1 += kernelvalue * (float)(input.at<G>(tempa, tempb));
					}
				}
				output.at<G>(i, j) = (G)sum1;
			}


			else if (!strcmp(opt, "adjustkernel")) {
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						kernelvalue = kernel.at<float>(a + n, b + n);
						sum1 += kernelvalue * (float)(input.at<G>(i + a, j + b));
						sum2 += kernelvalue;
					}
				}
				output.at<G>(i, j) = (G)(sum1 / sum2);
			}
		}
	}
	return output;
}