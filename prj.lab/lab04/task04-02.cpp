#include <semcv/semcv.hpp>
#include <fstream>
#include <random>
#include <vector>
#include <filesystem>

#include <semcv/semcv.hpp>
#include <fstream>
#include <vector>
#include <filesystem>

struct DetectedEllipse {
    double center_x;
    double center_y;
    double width;
    double height;
    double angle;
    int row;
    int col;
};

void drawAndSave(cv::Mat& img, const cv::RotatedRect& ellipse,
    std::vector<DetectedEllipse>& detections,
    int cellSize) {
    cv::ellipse(img, ellipse, cv::Scalar(0, 0, 255), 2);

    int col = static_cast<int>(ellipse.center.x) / cellSize;
    int row = static_cast<int>(ellipse.center.y) / cellSize;


    double local_x = ellipse.center.x - col * cellSize;
    double local_y = ellipse.center.y - row * cellSize;

    detections.push_back({
        local_x,       
        local_y,       
        ellipse.size.width,
        ellipse.size.height,
        ellipse.angle,
        row,
        col
        });
}

std::pair<cv::Mat, std::vector<DetectedEllipse>> detectObjects(cv::Mat& img)
{
    const int cellSize = 256;
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);
    cv::Mat binary;
    cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

    cv::Mat morph;
    int morph_size = 7;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(morph_size, morph_size));

    cv::morphologyEx(binary, morph, cv::MORPH_OPEN, element, cv::Point(-1, -1), 1);
    cv::morphologyEx(morph, morph, cv::MORPH_CLOSE, element, cv::Point(-1, -1), 2);
    cv::morphologyEx(morph, morph, cv::MORPH_OPEN, element, cv::Point(-1, -1), 2);
    cv::morphologyEx(morph, morph, cv::MORPH_CLOSE, element, cv::Point(-1, -1), 2);
    cv::dilate(morph, morph, element);
    cv::dilate(morph, morph, element);

    cv::Mat result = img.clone();
    std::vector<DetectedEllipse> detections;

    cv::Mat labels, stats, centrs;
    int num = cv::connectedComponentsWithStats(morph, labels, stats, centrs);

    for (int i = 1; i < num; ++i) {
        int left = stats.at<int>(i, cv::CC_STAT_LEFT);
        int top = stats.at<int>(i, cv::CC_STAT_TOP);
        int width = stats.at<int>(i, cv::CC_STAT_WIDTH);
        int height = stats.at<int>(i, cv::CC_STAT_HEIGHT);

        if (width < 5 || height < 5) continue;

        cv::Rect boundingBox(left, top, width, height);
        cv::Mat component = morph(boundingBox);

        std::vector<cv::Point> points;
        cv::findNonZero(component, points);

        if (points.size() >= 5) {
            cv::RotatedRect ell = cv::fitEllipse(points);
            if (ell.size.area() > cellSize * cellSize) continue;

            cv::Point2f scaledCenter(ell.center.x + left, ell.center.y + top);
            cv::Size2f scaledSize(ell.size.width, ell.size.height);
            cv::RotatedRect scaledEllipse(scaledCenter, scaledSize, ell.angle);

            drawAndSave(result, scaledEllipse, detections, cellSize);
        }
    }

    return { result, detections };
}

void saveDetectionResults(const std::string& filePath, const std::vector<DetectedEllipse>& detections) {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open output file: " + filePath);
    }

    outFile << detections.size() << "\n";

    for (const auto& det : detections) {
        outFile << det.center_x << "\n";
        outFile << det.center_y << "\n";
        outFile << det.width << "\n";
        outFile << det.height << "\n";
        outFile << det.angle << "\n";
        outFile << det.row << "\n";
        outFile << det.col << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input_image> <output_image> [output_detections]\n";
        return 1;
    }

    std::string inputImgPath = argv[1];
    std::string outputImgPath = argv[2];
    std::string detectedInfo;

    if (argc > 3) {
        detectedInfo = argv[3];
    }
    else {
        std::filesystem::path p(outputImgPath);
        detectedInfo = p.stem().string() + "_detections.txt";
    }

    try {
        // Загрузка изображения
        cv::Mat inputImg = cv::imread(inputImgPath);
        if (inputImg.empty()) {
            throw std::runtime_error("Could not load image: " + inputImgPath);
        }

        auto [detectedImg, detections] = detectObjects(inputImg);

        cv::imwrite(outputImgPath, detectedImg);
        saveDetectionResults(detectedInfo, detections);

        std::cout << "Successfully processed image. Results saved to:\n";
        std::cout << "  Image: " << outputImgPath << "\n";
        std::cout << "  Detections: " << detectedInfo << "\n";
        std::cout << "Detected objects: " << detections.size() << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}