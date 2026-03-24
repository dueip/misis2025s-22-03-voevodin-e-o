#include <opencv2/opencv.hpp>
#include <string>

/// \brief Генератор тестовых изображений для анализа GLDM.
/// 
/// Класс позволяет генерировать разные синтетические изображения для анализа и дебага.
class GLDMFeatureImageGenerator {
public:
    /// \brief Конструктор генератора с заданными размерами.
    /// \param[in] width Ширина генерируемых изображений.
    /// \param[in] height Высота генерируемых изображений.
    GLDMFeatureImageGenerator(int width = 1024, int height = 512)
        : width(width), height(height) {}

    /// \brief Генерирует изображение с низким уровнем серого.
    cv::Mat generateLowGrayLevelImage() {
        cv::Mat img(height, width, CV_64F);
        cv::randu(img, cv::Scalar(0), cv::Scalar(50));
        return img;
    }

    /// \brief Генерирует изображение с высокой однородностью.
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

    /// \brief Генерирует изображение с низкой однородностью.
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

    /// \brief Генерирует градиентное изображение слева направо.
    cv::Mat generateGradientImage() {
        cv::Mat img(height, width, CV_8UC1);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                img.at<uchar>(y, x) = static_cast<uchar>((255.0 * x) / width);
            }
        }
        return img;
    }

    /// \brief Генерирует изображение с кругом в центре.
    cv::Mat generateSpotImage() {
        cv::Mat img(height, width, CV_8UC1, cv::Scalar(0));
        cv::Point center(width / 2, height / 2);
        int radius = min(width, height) / 4;
        circle(img, center, radius, cv::Scalar(200), -1);  
        return img;
    }

    /// \brief Генерирует изображение со случайным шумом.
    cv::Mat generateRandomNoiseImage() {
        cv::Mat img(height, width, CV_64F);
        cv::randu(img, cv::Scalar(0), cv::Scalar(255));
        return img;
    }

    /// \brief Сохраняет изображение в файл.
    /// \param[in] img Изображение, которое нужно сохранить.
    /// \param[in] filename Путь файла для сохранения.
    void saveImage(const cv::Mat& img, const std::string& filename) {
        cv::imwrite(filename, img);
    }

private:
    int width; ///< Ширина генерируемых изображений.
    int height; ///< Высота генерируемых изображений.
};
