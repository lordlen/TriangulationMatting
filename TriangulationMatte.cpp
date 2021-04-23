#include "TriangulationMatte.h"

bool TriangulationMatte::initTriangulation(std::string b0, std::string b1, std::string c0, std::string c1)
{
    // initialize the backgrounds and the composites
    B0 = cv::imread(b0);
    B1 = cv::imread(b1);
    C0 = cv::imread(c0);
    C1 = cv::imread(c1);

    if (B0.empty() || B1.empty() || C0.empty() || C1.empty()) {
        return false;
    }
    else {
        return true;
    }
}

// init needs to be called before this
bool TriangulationMatte::triangulation()
{
    // TODO: check the dimensions of the image are the same

    int rows = B0.rows;
    int cols = B0.cols;

    F = cv::Mat::zeros(rows, cols, CV_32FC3);
    Alpha = cv::Mat::zeros(rows, cols, CV_32FC3);

    // need to  loop through each pixel and fill in the correct value in the output
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // make the delta matrix
            // get the C matrix and B matrix
            cv::Mat c0;
            cv::Mat c1;
            cv::Mat b0;
            cv::Mat b1;

            cv::Mat(C0.at<cv::Vec3b>(i, j)).convertTo(c0, CV_32FC1);
            cv::Mat(C1.at<cv::Vec3b>(i, j)).convertTo(c1, CV_32FC1);
            cv::Mat(B0.at<cv::Vec3b>(i, j)).convertTo(b0, CV_32FC1);
            cv::Mat(B1.at<cv::Vec3b>(i, j)).convertTo(b1, CV_32FC1);

            cv::Mat c;
            cv::Mat b;

            cv::vconcat(c0, c1, c);
            cv::vconcat(b0, b1, b);

            cv::Mat D = c - b;
            
            // construct the A matrix
            float dataA[] = {
                1,0,0,-b.at<float>(0,0),
                0,1,0,-b.at<float>(1,0),
                0,0,1,-b.at<float>(2,0),
                1,0,0,-b.at<float>(3,0),
                0,1,0,-b.at<float>(4,0),
                0,0,1,-b.at<float>(5,0)
            };

            cv::Mat A = cv::Mat(6, 4, CV_32FC1, dataA);

            cv::Mat pinvA;

            // get the pinv of A
            cv::invert(A, pinvA, cv::DECOMP_SVD);

            // solve for x
            cv::Mat x = pinvA * D;
            cv::Vec f = cv::Vec3f(x.at<float>(0,0), x.at<float>(1, 0), x.at<float>(2, 0));
            F.at<cv::Vec3f>(i, j) = f;
            float alpha = 1 - x.at<float>(3, 0);
            Alpha.at<cv::Vec3f>(i, j) = cv::Vec3f(alpha, alpha, alpha);
        }
    }

    return true;
}

bool TriangulationMatte::initComposite(std::string b2)
{
    B2 = cv::imread(b2);
    return true;
}

bool TriangulationMatte::createComposite()
{
    // TODO:check if the dimension of b2 matches dimensions of alphaF
    
    cv::Mat b2;
    B2.convertTo(b2, CV_32FC3);

    Out = F + (b2.mul(Alpha));
    Out.convertTo(Out, CV_8UC3);
    return true;
}

bool TriangulationMatte::writeImage(std::string filepath)
{
    cv::imwrite(filepath, Out);
    return false;
}
