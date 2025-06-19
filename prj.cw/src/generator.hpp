#include <opencv2/opencv.hpp>
#include <string>


class GLDMFeatureImageGenerator {
public:
    GLDMFeatureImageGenerator(int width = 1024, int height = 512)
        : width(width), height(height) {}

    cv::Mat generateLowGrayLevelImage() {
        cv::Mat img(height, width, CV_64F);
        cv::randu(img, cv::Scalar(0), cv::Scalar(50));
        return img;
    }

    cv::Mat generateHighDependenceUniformityImage() {
        cv::Mat img(height, width, CV_64F, cv::Scalar(0));
        int blockSize = 16;
        for (int y = 0; y < height; y += blockSize) {
            for (int x = 0; x < width; x += blockSize) {
                uchar val = (rand() % 4) * 50; 
                cv::Rect roi(x, y, blockSize, blockSize);
                img(roi) = val;
            }
        }
        return img;
    }

    cv::Mat generateHighDependenceNonUniformityImage() {
        cv::Mat img(height, width, CV_64F);
        for (int y = 0; y < height; y += 16) {
            for (int x = 0; x < width; x += 16) {
                cv::Mat block(16, 16, CV_8UC1);
                cv::randu(block, cv::Scalar(0), cv::Scalar(255));
                block.copyTo(img(cv::Rect(x, y, 16, 16)));
            }
        }
        return img;
    }

    cv::Mat generateGradientImage() {
        cv::Mat img(height, width, CV_8UC1);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                img.at<uchar>(y, x) = static_cast<uchar>((255.0 * x) / width);
            }
        }
        return img;
    }

    cv::Mat generateSpotImage() {
        cv::Mat img(height, width, CV_8UC1, cv::Scalar(0));
        cv::Point center(width / 2, height / 2);
        int radius = min(width, height) / 4;
        circle(img, center, radius, cv::Scalar(200), -1);  
        return img;
    }


    cv::Mat generateRandomNoiseImage() {
        cv::Mat img(height, width, CV_64F);
        cv::randu(img, cv::Scalar(0), cv::Scalar(255));
        return img;
    }

    void saveImage(const cv::Mat& img, const std::string& filename) {
        cv::imwrite(filename, img);
    }

private:
    int width;
    int height;
};
