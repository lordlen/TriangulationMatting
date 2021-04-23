#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
class TriangulationMatte
{
private:
	cv::Mat B0, B1;
	cv::Mat C0, C1;
	cv::Mat Alpha;
	cv::Mat F;
	cv::Mat B2;
	cv::Mat Out;
public:
	// initializes the matrices by filenames
	bool initTriangulation(std::string b0, std::string b1, std::string c0, std::string c1);
	bool triangulation();
	bool initComposite(std::string b2);
	bool createComposite();
	bool writeImage(std::string filepath);
};

