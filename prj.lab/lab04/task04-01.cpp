#include <opencv2/opencv.hpp>
#include <fstream>
#include <random>
#include <filesystem>
#include <optional>
#include <vector>
#include <stdexcept>

struct EllipseParams {
    int width;
    int height;
    int angle;
    int x;
    int y;
};

struct EllipseData {
    EllipseParams params;
    int row;
    int col;
};

void placeEllipse(cv::Mat& img, const EllipseParams& params, int ellpisColor) {
    cv::Point center(params.x, params.y);
    cv::Size ellipsRange(params.width / 2, params.height / 2);
    cv::RotatedRect rotatedBox(center, ellipsRange, params.angle);
    cv::ellipse(img, rotatedBox,
        cv::Scalar(ellpisColor, ellpisColor, ellpisColor), cv::FILLED);
}

EllipseParams generateParams(int min_elps_width, int max_elps_width, int min_elps_height, int max_elps_height,
    int margin, int img_size, std::mt19937& rng) {
    EllipseParams generatedElleps;
    std::uniform_int_distribution<int> dist_width(min_elps_width, max_elps_width);
    std::uniform_int_distribution<int> dist_height(min_elps_height, max_elps_height);
    std::uniform_int_distribution<int> dist_angle(0, 180);
    std::uniform_int_distribution<int> dist_x(margin, img_size - margin);
    std::uniform_int_distribution<int> dist_y(margin, img_size - margin);

    generatedElleps.width = dist_width(rng);
    generatedElleps.height = dist_height(rng);
    generatedElleps.angle = dist_angle(rng);
    generatedElleps.x = dist_x(rng);
    generatedElleps.y = dist_y(rng);

    while (generatedElleps.x - generatedElleps.width / 2 < margin ||
        generatedElleps.x + generatedElleps.width / 2 > img_size - margin ||
        generatedElleps.y - generatedElleps.height / 2 < margin ||
        generatedElleps.y + generatedElleps.height / 2 > img_size - margin) {
        generatedElleps.x = dist_x(rng);
        generatedElleps.y = dist_y(rng);
    }

    return generatedElleps;
}

void applyNoise(cv::Mat& img, int noise_std) {
    cv::Mat noised(img.size(), CV_8UC1);
    cv::randn(noised, 0, noise_std);
    cv::add(img, noised, img);
}

void applyBlur(cv::Mat& img, int blur_size) {
 
    if (blur_size <= 0) return;
    cv::blur(img, img, cv::Size(blur_size, blur_size));
}

std::pair<cv::Mat, EllipseParams> generateEllips(int bg_color, int elps_color,
    int min_elps_width, int max_elps_width,
    int min_elps_height, int max_elps_height) {

    const int img_size = 256;
    const int margin = 32;

    cv::Mat ellipsImage(img_size, img_size, CV_8UC1, cv::Scalar(bg_color));

    std::random_device dev;
    std::mt19937 randomGenerator(dev());

    EllipseParams params = generateParams(
        min_elps_width, max_elps_width,
        min_elps_height, max_elps_height,
        margin, img_size, randomGenerator);

    placeEllipse(ellipsImage, params, elps_color);
    return { ellipsImage, params };
}

std::pair<cv::Mat, std::vector<EllipseData>> generateCollage(
    int n, int bg_color, int elps_color, int noise_std, int blur_size,
    int min_elps_width, int max_elps_width,
    int min_elps_height, int max_elps_height) {

    if (n <= 0) throw std::invalid_argument("Collage size must be positive");

    std::vector<cv::Mat> rows;
    std::vector<EllipseData> ellipsesData;

    for (int row = 0; row < n; ++row) {
        std::vector<cv::Mat> cols;
        for (int col = 0; col < n; ++col) {
            auto [ellps, params] = generateEllips(
                bg_color, elps_color,
                min_elps_width, max_elps_width,
                min_elps_height, max_elps_height);

            if (ellps.empty()) {
                throw std::runtime_error("Generated empty ellipse at position: " +
                    std::to_string(row) + "," + std::to_string(col));
            }
            cols.push_back(ellps);
            ellipsesData.push_back({ params, row, col });
        }
        cv::Mat rowImage;
        cv::hconcat(cols, rowImage);
        if (rowImage.empty()) {
            throw std::runtime_error("Failed to create row image at row: " + std::to_string(row));
        }
        rows.push_back(rowImage);
    }
    cv::Mat collage;
    cv::vconcat(rows, collage);

    if (collage.empty()) {
        throw std::runtime_error("Failed to create collage");
    }

    // Применяем эффекты только если параметры не нулевые
    if (blur_size > 0) applyBlur(collage, blur_size);
    if (noise_std > 0) applyNoise(collage, noise_std);

    return { collage, ellipsesData };
}

struct Config {
    int n;
    int bg_color;
    int elps_color;
    int noise_std;
    int blur_size;
    int min_elps_width;
    int max_elps_width;
    int min_elps_height;
    int max_elps_height;
};

Config readConfig(const std::filesystem::path& config_path) {
    std::ifstream config_file(config_path);
    if (!config_file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + config_path.string());
    }

    Config config;
    config_file >> config.n;
    config_file >> config.bg_color;
    config_file >> config.elps_color;
    config_file >> config.noise_std;
    config_file >> config.blur_size;
    config_file >> config.min_elps_width;
    config_file >> config.max_elps_width;
    config_file >> config.min_elps_height;
    config_file >> config.max_elps_height;

    return config;
}

void writeEtalon(const std::filesystem::path& etalon_path,
    const Config& config,
    const std::vector<EllipseData>& ellipsesData) {
    std::ofstream etalon_file(etalon_path);
    if (!etalon_file.is_open()) {
        throw std::runtime_error("Failed to open etalon file: " + etalon_path.string());
    }

    etalon_file << config.n << "\n";
    etalon_file << config.bg_color << "\n";
    etalon_file << config.elps_color << "\n";
    etalon_file << config.noise_std << "\n";
    etalon_file << config.blur_size << "\n";
    etalon_file << config.min_elps_width << "\n";
    etalon_file << config.max_elps_width << "\n";
    etalon_file << config.min_elps_height << "\n";
    etalon_file << config.max_elps_height << "\n";

    etalon_file << ellipsesData.size() << "\n";

    for (const auto& data : ellipsesData) {
        etalon_file << data.params.width << "\n";
        etalon_file << data.params.height << "\n";
        etalon_file << data.params.angle << "\n";
        etalon_file << data.params.x << "\n";
        etalon_file << data.params.y << "\n";
        etalon_file << data.row << "\n";
        etalon_file << data.col << "\n";
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_path> [output_image] [output_etalon] [seed]\n";
        return 1;
    }

    std::filesystem::path config_path = argv[1];
    std::filesystem::path output_image = "collage.png";
    std::filesystem::path output_etalon = "etalon.txt";

    if (argc >= 3) output_image = argv[2];
    if (argc >= 4) output_etalon = argv[3];

    std::optional<unsigned> seed;
    if (argc >= 5) {
        try {
            seed = std::stoul(argv[4]);
        }
        catch (...) {
            std::cerr << "Invalid seed\n";
            return 1;
        }
    }

    try {
        // Read configuration
        Config config = readConfig(config_path);

        // Generate collage
        auto [collage, ellipsesData] = generateCollage(
            config.n,
            config.bg_color,
            config.elps_color,
            config.noise_std,
            config.blur_size,
            config.min_elps_width,
            config.max_elps_width,
            config.min_elps_height,
            config.max_elps_height
        );

        // Save results
        if (!cv::imwrite(output_image.string(), collage)) {
            throw std::runtime_error("Failed to write image to: " + output_image.string());
        }
        writeEtalon(output_etalon, config, ellipsesData);

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}