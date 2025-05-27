#include <iostream>
#include <array>
#include <opencv2/opencv.hpp>
#include <semcv/semcv.hpp>


namespace misis
{
    namespace GeneratedImageResolution
    {
        constexpr int rows = 30;
        constexpr int cols = 768;
    }
    namespace Correction
    {
        constexpr int corrected_size = 5;
        constexpr std::array<double, corrected_size> correction_coefs = { 1.8, 2.0, 2.2, 2.4, 2.6 };

        cv::Mat gamma_correct(cv::Mat img, double gamma)
        {
            cv::Mat corrected = cv::Mat(img.size(), img.type());
            
            for (int i = 0; i < img.rows; i++) {
                for (int j = 0; j < img.cols; j++) {
                    const double img_val = img.at<uchar>(i, j);
                    corrected.at<uchar>(i, j) = static_cast<uchar>(255.0 * pow(img_val / 255, gamma));
                }
            }

            return corrected;
        }
    }
}

cv::Mat generate_gray_image(const int rows, const int cols)
{
    using namespace misis;
    cv::Mat img(rows, cols, CV_8U);
    for (int i = 0; i < img.rows; ++i)
    {
        for (int j = 0; j < img.cols; ++j)
        {
            img.at<uchar>(i, j) = j / 3;
        }
    }
    return img;
}

int main(int argc, char* argv[])
{
    std::string path;
    if (!misis::is_debugger_present())
    {
        if (argc == 1)
        {
            std::cerr << "Savefile was not provided; Terminating...";
            return misis::Errors::InvalidParameter;
        }

        if (argc > 2)
        {
            std::cerr << "We save to multiple files. Terminating...";
            return misis::Errors::TooManyOpenFiles;
        }
        path = argv[1];
    }
    else
    {
        path = "./TEST_IMAGES/image.png";
    }

    cv::Mat img = generate_gray_image(misis::GeneratedImageResolution::rows, misis::GeneratedImageResolution::cols);
  
    std::vector<cv::Mat> corrected_mats;
    corrected_mats.reserve(misis::Correction::corrected_size);
    

    for (int i = 0; i < misis::Correction::corrected_size; ++i)
    {
        corrected_mats.push_back(misis::Correction::gamma_correct(img, misis::Correction::correction_coefs[i]));
    }

    for (int i = 0; i < misis::Correction::corrected_size; ++i)
    {
        cv::vconcat(img, corrected_mats[i], img);
    }

    try
    {
        cv::imwrite(path, img);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what();
    }
}