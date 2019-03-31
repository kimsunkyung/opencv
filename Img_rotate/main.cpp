#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt);

int main()
{
	Mat input, rotated;

	// Read each image
	input = imread("lena.jpg");

	// Check for invalid input
	if (!input.data) {
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// original image
	namedWindow("image");
	imshow("image", input);

	rotated = myrotate<Vec3b>(input, 45, "bilinear");

	// rotated image
	namedWindow("rotated");
	imshow("rotated", rotated);

	waitKey(0);

	return 0;
}

template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt) {
	int row = input.rows;
	int col = input.cols;

	float radian = angle * CV_PI / 180;

	float sq_row = ceil(row * sin(radian) + col * cos(radian));
	float sq_col = ceil(col * sin(radian) + row * cos(radian));

	Mat output = Mat::zeros(sq_row, sq_col, input.type());

	for (int i = 0; i < sq_row; i++) {
		for (int j = 0; j < sq_col; j++) {
			float x = (j - sq_col / 2) * cos(radian) - (i - sq_row / 2) * sin(radian) + col / 2;
			float y = (j - sq_col / 2) * sin(radian) + (i - sq_row / 2) * cos(radian) + row / 2;

			if ((y >= 0) && (y <= (row - 1)) && (x >= 0) && (x <= (col - 1))) {
				if (!strcmp(opt, "nearest")) {
					float xx = round(x);
					float yy = round(y);
					output.at<Vec3b>(i, j) = input.at<Vec3b>(yy, xx);

				}
				else if (!strcmp(opt, "bilinear")) {
					float x1 = floor(x);
					float x2 = ceil(x);
					float y1 = floor(y);
					float y2 = ceil(y);

					float lam = x - x1;
					float mu = y - y1;

					output.at<Vec3b>(i, j) = lam*(mu*input.at<Vec3b>(y2, x2) + (1 - mu)*input.at<Vec3b>(y1, x2))
						+ (1 - lam)*(mu*input.at<Vec3b>(y2, x1) + (1 - mu)*input.at<Vec3b>(y1, x1));

				}
			}
		}
	}

	return output;
}