#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <filesystem>

cv::Mat gen_image(uint8_t object_color, uint8_t bg_color) {
    const int size = 127;
    cv::Mat image(size, size, CV_8UC1, cv::Scalar(bg_color));
    cv::circle(image, cv::Point(size / 2, size / 2), 40, cv::Scalar(object_color), -1);
    return image;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <output_orig> <output_result>\n";
        return 1;
    }


    const uint8_t params[3] = { 0, 127, 255 };
    std::vector<cv::Mat> images;
    for (int circle = 0; circle < 3; ++circle) {
        for (int bg = 0; bg < 3; ++bg) {
            if (circle != bg) {
                images.push_back(gen_image(params[circle], params[bg]));
            }
        }
    }

    cv::Mat grid(127 * 3, 127 * 2, CV_8UC1);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            cv::Rect roi(j * 127, i * 127, 127, 127);
            images[i * 2 + j].copyTo(grid(roi));
        }
    }

    cv::rotate(grid, grid, cv::ROTATE_90_CLOCKWISE);
    cv::Mat M1 = (cv::Mat_<int>(2, 2) << 1, 0, 0, -1);
    cv::Mat M2 = (cv::Mat_<int>(2, 2) << 0, 1, -1, 0);

    cv::Mat I1_float, I2_float;
    cv::filter2D(grid, I1_float, CV_32F, M1);
    cv::filter2D(grid, I2_float, CV_32F, M2);

    cv::Mat I3_float = I1_float.mul(I1_float) + I2_float.mul(I2_float);

    cv::Mat V1_gray, V2_gray, V3_gray;
    cv::normalize(I1_float, V1_gray, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::normalize(I2_float, V2_gray, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::normalize(I3_float, V3_gray, 0, 255, cv::NORM_MINMAX, CV_8U);

    cv::Mat V4;
    std::vector<cv::Mat> channels = { V1_gray, V2_gray, V3_gray };
    cv::merge(channels, V4);

    cv::Mat V1_bgr, V2_bgr, V3_bgr;
    cv::cvtColor(V1_gray, V1_bgr, cv::COLOR_GRAY2BGR);
    cv::cvtColor(V2_gray, V2_bgr, cv::COLOR_GRAY2BGR);
    cv::cvtColor(V3_gray, V3_bgr, cv::COLOR_GRAY2BGR);


    cv::Mat top_row, bottom_row, result_img;
    cv::hconcat(V1_bgr, V2_bgr, top_row);
    cv::hconcat(V3_bgr, V4, bottom_row);
    cv::vconcat(top_row, bottom_row, result_img);

    cv::imwrite(argv[1], grid);
    cv::imwrite(argv[2], result_img);

    return 0;
}