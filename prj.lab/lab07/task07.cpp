#include <opencv2/opencv.hpp>

#include <semcv/semcv.hpp>

cv::Mat detectContours(cv::Mat& img) {
    cv::Mat gray, edge, res;

    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    int morphSize = 7;
    cv::Mat morphElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(morphSize, morphSize));
    cv::morphologyEx(gray, gray, cv::MORPH_OPEN, morphElement);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

    cv::Mat gradX, gradY, absGradX, absGradY;
    cv::Sobel(gray, gradX, CV_32F, 1, 0, 3);
    cv::Sobel(gray, gradY, CV_32F, 0, 1, 3);
    cv::convertScaleAbs(gradX, absGradX);
    cv::convertScaleAbs(gradY, absGradY);
    cv::addWeighted(absGradX, 0.5, absGradY, 0.5, 0, edge);

    cv::Mat closeElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(edge, edge, cv::MORPH_CLOSE, closeElement, cv::Point(-1, -1), 2);
    cv::threshold(edge, edge, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

    res = img.clone();
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edge, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours) {
        if (contour.size() < 5) continue;
        cv::RotatedRect ell = cv::fitEllipse(contour);
        cv::ellipse(res, ell, cv::Scalar(0, 255, 0), 2);
        cv::circle(res, ell.center, 2, cv::Scalar(0, 0, 255), 2, cv::FILLED);
    }

    return res;
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
    auto annotatedImg = detectContours(img);
    cv::imshow("Contours with Ellipses", annotatedImg);
    cv::waitKey(0);

    return 0;
}
