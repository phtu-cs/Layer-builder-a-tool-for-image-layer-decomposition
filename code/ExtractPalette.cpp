#include"ExtractPalette.h"
#include<Eigen/Dense>
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace Eigen;
using namespace std;
const float MIN_DISTANCE = 0.001;
const float THETA_A = 80;
const int NUM_BINS = 4;
int Palette::FindInterval(float num, const VectorXf& bins){

	for (int i = 0; i < bins.size() - 1; i++){
		if (num >= bins(i) && num <= bins(i + 1)){
			return i;
			break;
		}
	}
}

Palette::Palette(){
	
}
Palette::~Palette(){

}
void Palette::HistCounts(const Image& rgbI, const Image& labI, Cluster& assigned_cluster){
	

	assigned_cluster.resize(pow(NumBins, 3));
	VectorXf bins = VectorXf::LinSpaced(NumBins + 1, 0, 1);


	float temp_darkest = FLT_MAX; int num_darkest_row = 0; int num_darkest_col = 0;
	for (int row = 0; row < rgbI.rows(); row++){
		for (int col = 0; col < rgbI.cols(); col++){

			//std::cout << I(row, col)(0);
			int rbin = FindInterval(rgbI(row, col)(0) / 255.0, bins);
			int gbin = FindInterval(rgbI(row, col)(1) / 255.0, bins);
			int bbin = FindInterval(rgbI(row, col)(2) / 255.0, bins);

			if (temp_darkest > rgbI(row, col).sum()){
				temp_darkest = rgbI(row, col).sum();
				num_darkest_row = row;
				num_darkest_col = col;
			}
			//if (rbin*pow(NumBins, 2) + gbin*NumBins + bbin>4000){
			//	cout << rbin*pow(NumBins, 2) + gbin*NumBins + bbin << endl;
			//}
			assigned_cluster[rbin*pow(NumBins, 2) + gbin*NumBins + bbin].weight++;
			assigned_cluster[rbin*pow(NumBins, 2) + gbin*NumBins + bbin].color += rgbI(row, col) / 255.0;
			assigned_cluster[rbin*pow(NumBins, 2) + gbin*NumBins + bbin].color_lab += labI(row, col);
		//	cout << labI(row, col) << endl;
		}
	}
	for (int i = 0; i < assigned_cluster.size(); i++){
		if (assigned_cluster[i].weight != 0){
			assigned_cluster[i].color /= assigned_cluster[i].weight;
			
		//	cout << assigned_cluster[i].color_lab << endl;
			assigned_cluster[i].color_lab /= assigned_cluster[i].weight;
			
		}
	}
	DarkestColor = rgbI(num_darkest_row, num_darkest_col);
	DarkestColorLab = labI(num_darkest_row, num_darkest_col);
}
void Palette::InitKmeans(const Cluster& all_clusters, int K){
	//color belongs to Lab space!

	//copy the clusters
	Cluster temp_clusters = all_clusters;
	int ik = 0;
	while (ik < K){
		//find largest N_i
		int max_n = 0; int max_npos = 0;
		for (int i = 0; i < temp_clusters.size(); i++){
			if (max_n < temp_clusters[i].weight){
				max_n = temp_clusters[i].weight;
				max_npos = i;
			}
		}
		Vector3f temp_c = temp_clusters[max_npos].color;
		Vector3f temp_clab = temp_clusters[max_npos].color_lab;
		for (int i = 0; i < temp_clusters.size(); i++){
			//use lab space to calculate the distance
			float temp_d = Distance(temp_clusters[i].color_lab, temp_clab);
			float temp_factor = 1 - exp(-pow(temp_d, 2) / pow(THETA_A, 2));
			temp_clusters[i].weight = temp_clusters[i].weight*temp_factor;
		}

		//add the init color
		PalettePack.push_back(temp_c);
		PalettePackLab.push_back(temp_clab);
		
		ik++;
	}
	//fix darkest color
	PalettePack.push_back(DarkestColor);
	PalettePackLab.push_back(DarkestColorLab);

}

float Palette::Distance(const Vector3f& c1, const Vector3f& c2){
	Vector3f diff = c1.array() - c2.array();
	return sqrt(diff.array().square().sum());
}

Vector3f Palette::Centroid(const vector<ClusterElem>& points){

	Vector3f color_sum(0, 0, 0);
	int weight_sum = 0;
	for (int i = 0; i < points.size(); i++){

	//	cout << points[i].color << endl;
		color_sum += points[i].color*points[i].weight;
		weight_sum += points[i].weight;
	}
	color_sum /= weight_sum;

	return color_sum;
}

Vector3f Palette::CentroidLab(const vector<ClusterElem>& points){

	Vector3f color_sum(0, 0, 0);
	int weight_sum = 0;
	for (int i = 0; i < points.size(); i++){

		//	cout << points[i].color << endl;
		color_sum += points[i].color_lab*points[i].weight;
		weight_sum += points[i].weight;
	}
	color_sum /= weight_sum;

	return color_sum;
}

void Palette::WeightedKmeans(const Cluster& clu){

	//size of Pallette
	int pal_size = PalettePack.size();
	float distance_sum = 0;
	do{
		distance_sum = 0;
		ColorPoints next_c, next_clab;
		Clusters clusters(pal_size);
		//assign points to different clusters
		for (int ic = 0; ic < clu.size(); ic++){
			Vector3f c = clu[ic].color_lab;
			float min_distance = FLT_MAX;
			int temp_num_min = 0;
			for (int ip = 0; ip < pal_size; ip++){
				float temp_distance = Distance(c, PalettePackLab[ip]);
				
				if (min_distance > temp_distance){
					min_distance = temp_distance;
					temp_num_min = ip;
				}
				
			}
			clusters[temp_num_min].push_back(clu[ic]);
		}

		//calculate centroids of each cluster
		for (int icl = 0; icl < clusters.size() ; icl++){
			next_c.push_back(Centroid(clusters[icl]));
			next_clab.push_back(CentroidLab(clusters[icl]));
			distance_sum += Distance(next_clab[icl], PalettePackLab[icl]);
		}
		//fix darkest color
		next_c.push_back(DarkestColor);
		next_clab.push_back(DarkestColorLab);

		PalettePack = next_c;
		PalettePackLab = next_clab;
	} while (distance_sum > MIN_DISTANCE);
}
void Palette::ExtractPalettePack(const Image& rgbI,const Image& labI, int num_color){
	Cluster temp_clu;
	SetNumBins(NUM_BINS);
	HistCounts(rgbI, labI, temp_clu);
	InitKmeans(temp_clu, num_color);
	WeightedKmeans(temp_clu);
	///* Manually selected color */
	//PalettePack[0][0] = 0.0627;
	//PalettePack[0][1] = 0.1059;
	//PalettePack[0][2] = 0.2118;

	//PalettePack[1][0] = 0.9569;
	//PalettePack[1][1] = 0.9765;
	//PalettePack[1][2] = 0.9804;

	//PalettePack[2][0] = 0.7843;
	//PalettePack[2][1] = 0.8471;
	//PalettePack[2][2] = 0.8980;

	//PalettePack[3][0] = 0.4431;
	//PalettePack[3][1] = 0.8039;
	//PalettePack[3][2] = 0.9412;

	//PalettePack[4][0] = 0.3882;
	//PalettePack[4][1] = 0.4078;
	//PalettePack[4][2] = 0.4000;

	PaletteShow(num_color, rgbI.rows(), rgbI.cols());
}
void Palette::SetNumBins(int Num){
	NumBins = Num;
}
void Palette::PaletteShow(int num_color, int rows ,int cols){

	
	std::vector<cv::Mat> temp_mat;
	for (int i = 0; i < num_color; i++){
		for (int j = 0; j < 3; j++){
			temp_mat.push_back(cv::Mat(round(rows/10.0), cols, CV_32FC1, PalettePack[i][j]));

	//		cout << PalettePack[i][j] << ", ";
		//	cout << cv::Mat(100, 100, CV_32FC1,PalettePack[i][j]) << endl;
		}
		cv::Mat show;
//		cout << endl;
		cv::merge(temp_mat, show);
		show_pal.push_back(show);

//		cv::cvtColor(show, show, cv::COLOR_Lab2BGR);
		stringstream temp_s;
		temp_s << i;

		//cv::imshow(string("window") +temp_s.str(), show);
//		cv::imwrite("color" + temp_s.str() + ".jpg", show*255);
		//cv::waitKey(0);
		temp_mat.clear();
	}

}