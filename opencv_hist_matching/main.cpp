#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);

int main() {

	Mat input = imread("mars.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output = imread("mars2.png", CV_LOAD_IMAGE_COLOR);
	Mat input_gray,output_gray,final_img;

	cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale
	cvtColor(output, output_gray, CV_RGB2GRAY);
	cvtColor(input, final_img, CV_RGB2GRAY);

	Mat matching1 = input_gray.clone();
	Mat matching2 = output_gray.clone();

	// PDF or transfer function txt files
	
	FILE *f_trans_func_eq1;
	FILE *f_trans_func_eq2;
	FILE *f_eq2;

	fopen_s(&f_trans_func_eq1, "trans_func_mat1.txt", "w+");
	fopen_s(&f_trans_func_eq2, "trans_func_mat2.txt", "w+");
	fopen_s(&f_eq2, "f_eq2.txt", "w+");

	float *PDF1 = cal_PDF(input_gray);	// PDF of Input image(Grayscale) : [L]
	float *CDF1 = cal_CDF(input_gray);	// CDF of Input image(Grayscale) : [L]
	float *PDF2 = cal_PDF(output_gray);	// PDF of Input image(Grayscale) : [L]
	float *CDF2 = cal_CDF(output_gray);

	G trans_func_eq1[L] = { 0 };			// transfer function
	G trans_func_eq2[L] = { 0 };
	G eq2[L] = { 0 };
	hist_eq(input_gray, matching1, trans_func_eq1, CDF1);
	hist_eq(output_gray, matching2, trans_func_eq2, CDF2);	

	float *equalized_PDF_gray1 = cal_PDF(matching1);	
	float *equalized_PDF_gray2 = cal_PDF(matching2);	// equalized PDF (grayscale)

	int i = 0;
	for (int k = 0; k < L; k++) {
		while (i < trans_func_eq2[k + 1]) {
			if (eq2[i] == 0) {
				eq2[i] = k;
			}
			i += 1;
		}
	}
	for (int i = 0; i < L; i++) {
		fprintf(f_trans_func_eq1, "%d\t%d\n", i, trans_func_eq1[i]);
		fprintf(f_trans_func_eq2, "%d\t%d\n", trans_func_eq2[i], i);
		fprintf(f_eq2, "%d\t%d\n", i, eq2[i]);
	}

	for (int i = 0; i < matching1.rows; i++)
		for (int j = 0; j < matching1.cols; j++)
			final_img.at<G>(i, j) = eq2[trans_func_eq1[matching1.at<G>(i, j)]];

	

	// memory release
	free(PDF1);
	free(CDF1);
	fclose(f_trans_func_eq1);
	fclose(f_trans_func_eq2);
	fclose(f_eq2);

	////////////////////// Show each image ///////////////////////

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Equalized", WINDOW_AUTOSIZE);
	imshow("Equalized", final_img);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram matching
void hist_eq(Mat &input, Mat &matching, G *trans_func, float *CDF) {

	// compute transfer function
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);
}