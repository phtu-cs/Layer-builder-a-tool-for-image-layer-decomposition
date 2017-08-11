#ifndef _EXTRACT_PALETTE_
#define _EXTRACT_PALETTE_


#include<vector>
#include<Eigen/Dense>
#include<opencv2/opencv.hpp>

using std::vector;
using namespace Eigen;
typedef Eigen::Matrix<Vector3f, Dynamic, Dynamic> Image;

class Palette{

public:	
	struct ClusterElem{

		ClusterElem(){ color = Vector3f::Zero();
					   color_lab = Vector3f::Zero();
					   weight = 0; }
		Vector3f color;
		Vector3f color_lab;
		int weight;
	};

	typedef Eigen::Vector3f Pixel;
	typedef vector<int> NumColorPoints; 
	typedef vector<Pixel> ColorPoints;
	typedef vector<vector<ClusterElem>> Clusters;
	typedef vector<ClusterElem> Cluster;

	Palette();
	~Palette();

	void HistCounts(const Image& rgbI, const Image& labI, Cluster& assigned_cluster);
	void InitKmeans(const Cluster& all_clusters, int K);
	void WeightedKmeans(const Cluster& clu);
	void ExtractPalettePack(const Image& rgbI, const Image& labI, int NumColor);
	void SetNumBins(int Num);
	void PaletteShow(int num_color, int rows, int cols);


	int FindInterval(float num, const VectorXf& bins);
	float Distance(const Vector3f& c1, const Vector3f& c2);
	Vector3f Centroid(const vector<ClusterElem>& points);
	Vector3f CentroidLab(const vector<ClusterElem>& points);



	vector<cv::Mat> show_pal;
	vector<Pixel> PalettePack;
	vector<Pixel> PalettePackLab;
	Pixel DarkestColor;
	Pixel DarkestColorLab;
	int NumBins;
};




#endif