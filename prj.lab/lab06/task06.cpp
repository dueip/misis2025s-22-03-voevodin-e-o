#include <opencv2/opencv.hpp>
#include <fstream>
#include <random>
#include <semcv/semcv.hpp>

cv::Mat detectBlobs(cv::Mat& img) {
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::imshow("Grayscale Image", gray);

    cv::Size originalSize = img.size();

    int morphSize = 7;
    cv::Mat morphElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(morphSize, morphSize));
    cv::morphologyEx(gray, gray, cv::MORPH_OPEN, morphElement);
    cv::GaussianBlur(gray, gray, cv::Size(7, 7), 0);

    const int numLevels = 5;
    const double scaleFactor = 1.5;
    const double sigma1 = 0.8, sigma2 = 0.3;
    const int ksize1 = 9, ksize2 = 7;

    std::vector<cv::Mat> pyramid = { gray.clone() };
    for (int i = 1; i < numLevels; ++i) {
        cv::Mat blurred, resized;
        cv::GaussianBlur(pyramid[i - 1], blurred, cv::Size(), (sigma1 + sigma2) / 2.0);
        cv::resize(blurred, resized, cv::Size(), 1.0 / scaleFactor, 1.0 / scaleFactor, cv::INTER_NEAREST);
        cv::imshow("Blurred & Resized Level " + std::to_string(i), resized);
        cv::waitKey(0);
        pyramid.push_back(resized);
    }

    std::vector<cv::Mat> dogPyramid;
    for (size_t i = 0; i < pyramid.size(); ++i) {
        cv::Mat blurred1, blurred2, dogImage;
        cv::GaussianBlur(pyramid[i], blurred1, cv::Size(ksize1, ksize1), sigma1);
        cv::GaussianBlur(pyramid[i], blurred2, cv::Size(ksize2, ksize2), sigma2);
        cv::subtract(blurred2, blurred1, dogImage);
        cv::normalize(dogImage, dogImage, 0, 255, cv::NORM_MINMAX);

        int dogMorphSize = 9;
        cv::Mat dogElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dogMorphSize, dogMorphSize));
        cv::morphologyEx(dogImage, dogImage, cv::MORPH_CLOSE, dogElement);

        cv::imshow("DoG Image Level " + std::to_string(i), dogImage);
        cv::waitKey(0);

        dogPyramid.push_back(dogImage);
    }

    cv::Mat dogFinal = dogPyramid.back();
    cv::resize(dogFinal, dogFinal, originalSize, 0, 0, cv::INTER_LINEAR);
    cv::imshow("Final DoG Image Resized", dogFinal);
    cv::waitKey(0);

    return img.clone();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>\n";
        return 1;
    }

    cv::Mat img = cv::imread(argv[1]);
    if (img.empty()) {
        std::cerr << "Failed to load image: " << argv[1] << "\n";
        return 1;
    }

    cv::imshow("Original Input Image", img);
    cv::Mat annotatedImg = detectBlobs(img);
    cv::imshow("Detected Structures (Output)", annotatedImg);
    cv::waitKey(0);

    return 0;
}
