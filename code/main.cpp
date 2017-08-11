#include"LLE.h"
#include<iostream>
#include<Eigen/Dense>
#include<opencv2/opencv.hpp>
#include"EnergyFunction.h"
#include"ExtractPalette.h"
#include"MatConvert.h"
#include"Superpixel.h"
#include<fstream>
using namespace Eigen;
using namespace std;
using namespace cv;

void ConvertFromSlic(const Slic& s, const cv::Mat& img, vector<Superpixel>& spixels){
	for (int i = 0; i < img.cols; i++){
		for (int j = 0; j < img.rows; j++){
			int index = s.clusters[i][j];
			cv::Vec3b temp_color = img.at<cv::Vec3b>(j, i);

			spixels[index].color += Vector3f(temp_color[0], //B
				temp_color[1], //G
				temp_color[2]); //R

			spixels[index].pixel_count++;
			spixels[index].x = s.centers[index][3];
			spixels[index].y = s.centers[index][4];
			spixels[index].pixels.push_back(Pixel(i, j, spixels[index].color));
		}
	}
	//average sum of color 
	for (int i = 0; i < s.centers.size(); i++){

		spixels[i].color /= spixels[i].pixel_count;
		//	cout << spixels[i].color << endl;
	}

}
int main(int argc, char *argv[]){


	Mat img = imread(argv[1]);
	int PALETTE_SIZE = atoi(argv[2]);

	IplImage *image = cvLoadImage(argv[1], 1);

	IplImage *lab_image = cvCloneImage(image);

	cvCvtColor(image, lab_image, CV_BGR2Lab);
	int rows = img.rows;
	int cols = img.cols;
	Mat lab_img(rows, cols, CV_32FC3);
	//
	cvtColor(img, lab_img, COLOR_BGR2Lab);

	//convert Mat to Eigen::Matrix
	Image eig_bgrimg(rows, cols);
	Image eig_labimg(rows, cols);

	Mat2Eig(img, eig_bgrimg);
	Mat2Eig(lab_img, eig_labimg);

	/* Palette extraction */
	Palette p;
	p.ExtractPalettePack(eig_bgrimg, eig_labimg, PALETTE_SIZE);
	cout << endl;
	cout << "Color extraction. Done." << endl;


	/*super-pixel sgementation*/
	int w = image->width, h = image->height;
	int nr_superpixels = 250;
	int nc = 40;
	double step = sqrt((w * h) / (double)nr_superpixels);
	Slic slic;
	slic.generate_superpixels(lab_image, step, nc);

	slic.colour_with_cluster_means(image);
	slic.display_center_grid(image, CvScalar(255, 0, 0));
	slic.display_contours(image, CvScalar(0, 0, 255));

	cout << "Superpixel segementation. Done." << endl;

	/* Convert to superpixels */
	vector<Superpixel> spixels(slic.centers.size());
	ConvertFromSlic(slic, img, spixels);

	/* Compute energy function */
	vector<Mat> layers;
	vector<Mat> layers_perpixel;

	EnergyFunction efunc(1, 0.5, 0.1, 0.1, PALETTE_SIZE);
	efunc.GetMatrixM(spixels);
	efunc.GetMatrixR(spixels, p);
	efunc.GetMatrixU(spixels);
	efunc.GetMatrixB(spixels);
	efunc.GetMatrixQ(img, spixels);
	efunc.GetMatrixLAndX(spixels);
	efunc.ShowMatrixL(layers, slic, rows, cols);
	efunc.ShowMatrixX(layers_perpixel, slic, rows, cols);

	cout << "Layer decomposition. Done." << endl;

	/* Output images */
	stringstream s,ssp;
	ssp << PALETTE_SIZE;
	for (int i = 0; i < layers_perpixel.size(); i++){
		vector<Mat> temp_rgb;
		temp_rgb.push_back(layers_perpixel[i]);
		temp_rgb.push_back(layers_perpixel[i]);
		temp_rgb.push_back(layers_perpixel[i]);
		Mat temp_rgb_result;
		merge(temp_rgb, temp_rgb_result);
		Mat temp_result;
		s << i;
		vconcat(p.show_pal[i], temp_rgb_result, temp_result);
		imwrite("layer_" + s.str() + "_numsp_" + ssp.str() + "_" + argv[1], temp_result * 255);
		temp_rgb.clear();
		s.str("");
	}

	Mat recons_img = Mat::zeros(img.rows, img.cols, CV_32FC3);
	for (int row = 0; row < img.rows; row++){
		for (int col = 0; col < img.cols; col++){

			for (int i = 0; i < PALETTE_SIZE; i++){

				recons_img.at<Vec3f>(row, col) += (layers_perpixel[i].at<float>(row, col) * Vec3f(p.PalettePack[i][0],
					p.PalettePack[i][1], p.PalettePack[i][2]));
			}
		}
	}
	imwrite("recons_" + string(argv[1]), recons_img * 255);
	Mat recol_image = Mat::zeros(img.rows, img.cols, CV_32FC3);
	int num_recolor;


	cout << "Which layer do you want to recolor? (from layer 0 to "
		<< layers_perpixel.size() - 1 << ")" << endl;

	for (int i = 0; i < layers_perpixel.size(); i++){
		s << i;
		cout << "Color of layer " << i << ":" << p.PalettePack[i].transpose() << " (B G R, [0,1]) " << 
			"layer_" + s.str() + "_numsp_" + ssp.str() + "_" + argv[1] << endl;
		s.str("");
	}
	cout << "Please see output images for visualization" << endl;
	cout << "Please input the number of the layer: ";
	cin >> num_recolor;
	cout << "You want to recolor layer" << num_recolor << endl;
	cout << "Please input the color (BGR) of the new layer (e.g. 0.5 0.5 0.5): ";
	cin >> p.PalettePack[num_recolor][0];
	cin >> p.PalettePack[num_recolor][1];
	cin >> p.PalettePack[num_recolor][2];

	for (int row = 0; row < img.rows; row++){
		for (int col = 0; col < img.cols; col++){
			for (int i = 0; i < PALETTE_SIZE; i++){

				recol_image.at<Vec3f>(row, col) += (layers_perpixel[i].at<float>(row, col) * Vec3f(p.PalettePack[i][0],
					p.PalettePack[i][1], p.PalettePack[i][2]));
			}
		}
	}
	imwrite("recol_" + string(argv[1]), recol_image * 255);

	return 0;
}