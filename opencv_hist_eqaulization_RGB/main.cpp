#include "hist_func.h"

void hist_eq_Color(Mat &input, Mat &equalized, G(*trans_func)[3], float **CDF);

int main() {

	Mat input = imread("input.jpg", CV_LOAD_IMAGE_COLOR);
	Mat equalized_RGB = input.clone();

	// PDF or transfer function txt files
	FILE *f_equalized_PDF_RGB, *f_PDF_RGB;
	FILE *f_trans_func_eq_RGB;

	fopen_s(&f_PDF_RGB, "PDF_RGB.txt", "w+");
	fopen_s(&f_equalized_PDF_RGB, "equalized_PDF_RGB.txt", "w+");
	fopen_s(&f_trans_func_eq_RGB, "trans_func_eq_RGB.txt", "w+");

	float **PDF_RGB = cal_PDF_RGB(input);	// PDF of Input image(RGB) : [L][3]
	float **CDF_RGB = cal_CDF_RGB(input);	// CDF of Input image(RGB) : [L][3]

	G trans_func_eq_RGB[L][3] = { 0 };		// transfer function
	
	// histogram equalization on RGB image
	hist_eq_Color(input, equalized_RGB, trans_func_eq_RGB, CDF_RGB);

	// equalized PDF (RGB)
	float **equalized_PDF = cal_CDF_RGB(equalized_RGB);

	for (int i = 0; i < L; i++) {
		for (int k = 0; k < 3; k++) {
			// write PDF
			fprintf(f_PDF_RGB, "%d %f\t", i, PDF_RGB[i][k]);
			fprintf(f_equalized_PDF_RGB, "%d %f\t", i, equalized_PDF[i][k]);

			// write transfer functions
			fprintf(f_trans_func_eq_RGB, "%d %d\t", i, trans_func_eq_RGB[i][k]);
		}
		fprintf(f_PDF_RGB, "\n");
		fprintf(f_equalized_PDF_RGB, "\n");
		fprintf(f_trans_func_eq_RGB, "\n");
	}

	// memory release
	free(PDF_RGB);
	free(CDF_RGB);
	fclose(f_PDF_RGB);
	fclose(f_equalized_PDF_RGB);
	fclose(f_trans_func_eq_RGB);

	////////////////////// Show each image ///////////////////////

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Equalized_RGB", WINDOW_AUTOSIZE);
	imshow("Equalized_RGB", equalized_RGB);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram equalization on 3 channel image
void hist_eq_Color(Mat &input, Mat &equalized, G(*trans_func)[3], float **CDF) {

	for (int i = 0; i < L; i++) {
		for (int k = 0; k < 3; k++) {
			trans_func[i][k] = (G)(L - 1)*CDF[i][k];
		}
	}

	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			for (int k = 0; k < 3; k++) {
				equalized.at<Vec3b>(i, j)[k] = trans_func[input.at<Vec3b>(i, j)[k]][k];
			}
		}
	}


	////////////////////////////////////////////////
	//											  //
	// How to access multi channel matrix element //
	//											  //
	// if matrix A is CV_8UC3 type,				  //
	// A(i, j, k) -> A.at<Vec3b>(i, j)[k]		  //
	//											  //
	////////////////////////////////////////////////

}