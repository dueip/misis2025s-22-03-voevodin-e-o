#include "gldm.hpp"
#include <numeric>
using namespace misis;



GLDM::GLDM(const std::filesystem::path& img)
{
    readImage(img);
}

bool GLDM::readImage(const std::filesystem::path& img)
{
    try
    {
        image = cv::imread(img.string());
        CheckReturn(isImageLoaded(), false);
        computeGLDM(2, 3);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void misis::GLDM::computeGLDM(Real delta, Real alpha) {
    int Ng = 256;
    int maxDependence = 8;
    cv::Mat P = cv::Mat::zeros(Ng, maxDependence + 1, CV_64F);

    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            int centerVal = image.at<uchar>(y, x);
            int count = 0;

            for (int dy = -delta; dy <= delta; ++dy) {
                for (int dx = -delta; dx <= delta; ++dx) {
                    if (dx == 0 && dy == 0) continue;

                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && ny >= 0 && nx < image.cols && ny < image.rows) {
                        int neighborVal = image.at<uchar>(ny, nx);
                        if (abs(centerVal - neighborVal) <= alpha) {
                            count++;
                        }
                    }
                }
            }

            if (count <= maxDependence)
                P.at<double>(centerVal, count)++;
        }
    }

    P.copyTo(image);
    wasGlDMComputed = true;
}


bool misis::GLDM::importImageFromMat(const cv::Mat& mat)
{
    mat.copyTo(image);
    return isImageLoaded();
}

bool [[nodiscard]] GLDM::isImageLoaded() const
{
    return !image.empty(); //&& wasGlDMComputed;
}

Real [[nodiscard]] GLDM::getDependenceNonUniformityFeatureValue() const
{
    CheckReturn(isImageLoaded(), 0.0f);
    CheckReturn(wasGlDMComputed, 0.0f);
    
    double Nz = sum(image)[0];
    CheckReturn(Nz < std::numeric_limits<double>::epsilon() || Nz > std::numeric_limits<double>::epsilon(), 0.0f);

    double result = 0.0;
    for (int j = 0; j < image.cols; ++j) {
        double colSum = 0.0;
        for (int i = 0; i < image.rows; ++i) {
            colSum += image.at<double>(i, j);
        }
        result += colSum * colSum;
    }
    return result / Nz;

}

Real [[nodiscard]] GLDM::getLowGrayLevelEmphasisFeatureValue() const
{
    CheckReturn(isImageLoaded(), 0.0f);
    CheckReturn(wasGlDMComputed, 0.0f);

    double Nz = sum(image)[0];
    CheckReturn(Nz < std::numeric_limits<double>::epsilon() || Nz > std::numeric_limits<double>::epsilon(), 0.0f);

    double result = 0.0;
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            result += image.at<double>(i, j) / ((i + 1.0) * (i + 1.0));
        }
    }
    return result / Nz;
}
