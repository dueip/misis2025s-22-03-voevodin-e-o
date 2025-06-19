#include "gldm.hpp"
#include "generator.hpp"
#include "extractor.hpp"
#include <filesystem>

void generateTestImages(const std::filesystem::path& path) {
    if (path.has_filename())
    {
        return;
    }
    GLDMFeatureImageGenerator generator;
    std::cout << path.string() << std::endl;
    generator.saveImage(generator.generateLowGrayLevelImage(), path.string() + ("low_gray.png"));
    generator.saveImage(generator.generateHighDependenceUniformityImage(), path.string() +  "uniform_dep.png");
    generator.saveImage(generator.generateHighDependenceNonUniformityImage(), path.string() + "nonuniform_dep.png");
    generator.saveImage(generator.generateRandomNoiseImage(), path.string() + "noise.png");
    generator.saveImage(generator.generateGradientImage(), path.string() + "gradient.png");
    generator.saveImage(generator.generateSpotImage(), path.string() + "spot.png");
}

#include <opencv2/highgui.hpp>

static void drawLabelWithBackground(cv::Mat& image,
    const std::string& text,
    cv::Point origin,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX,
    double fontScale = 0.6,
    int thickness = 1,
    cv::Scalar textColor = cv::Scalar(255, 255, 255),
    cv::Scalar bgColor = cv::Scalar(0, 0, 0),
    double alpha = 0.5)
{
    int baseline = 0;
    cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);
    cv::Rect bgRect(origin.x - 5,
        origin.y - textSize.height - 5,
        textSize.width + 10,
        textSize.height + 10);

    bgRect &= cv::Rect(0, 0, image.cols, image.rows);

    cv::Mat roi = image(bgRect);
    cv::Mat overlay = roi.clone();
    cv::rectangle(overlay, cv::Rect(0, 0, bgRect.width, bgRect.height), bgColor, cv::FILLED);

    cv::addWeighted(overlay, alpha, roi, 1.0 - alpha, 0.0, roi);
    
    cv::putText(image,
        text,
        cv::Point(origin.x, origin.y - 2),
        fontFace,
        fontScale,
        textColor,
        thickness,
        cv::LINE_AA);
}

void showGuiResults(const std::vector<misis::AnalysisResult>& results)
{
    for (const auto& res : results)
    {
        cv::Mat img = cv::imread(res.imageName, cv::IMREAD_GRAYSCALE);
        if (img.empty())
        {
            std::cerr << "[!] Failed to load " << res.imageName << '\n';
            continue;
        }

        cv::Mat display;
        cv::cvtColor(img, display, cv::COLOR_GRAY2BGR);

        int y = 30;
        drawLabelWithBackground(display, "Image: " + res.imageName,
            { 10, y });                          y += 30;

        drawLabelWithBackground(display, "LGLE: " + std::to_string(res.LGLE),
            { 10, y });                          y += 30;

        drawLabelWithBackground(display, "DN: " + std::to_string(res.DN),
            { 10, y });                          y += 30;

        drawLabelWithBackground(display, "Category:", { 10, y });    y += 25;

        drawLabelWithBackground(display, res.category, { 10, y });   y += 30;

        cv::imshow("GLDM Result", display);
        std::cout << "[i] Showing image: " << res.imageName << '\n';
        cv::waitKey(0); 
    }
    cv::destroyAllWindows();
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage:\n"
            << "  --generate <dir>             Generate test images in the <dir> directory\n"
            << "  --analyze <img1> <img2> ...  Analyze provided image(s)\n"
            << "  --output_directory           Output directory for analyzytor\n"
            << "  -gui                         Use GUI\n"
            << "  [--alpha <int>]              Threshold (default: 5)\n"
            << "  [--delta <int>]              Neighborhood radius (default: 1)\n";
        return 0;
    }

    std::vector<std::string> imagesToAnalyze;
    std::filesystem::path generationDir;
    std::filesystem::path outputDir;
    bool doGenerate = false;
    int alpha = 5;
    int delta = 1;

    bool guiMode = false;
    std::vector<misis::AnalysisResult> guiResults;


    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--generate" && i + 1 < argc && std::string(argv[i + 1]).rfind("--", 0) == std::string::npos) {
            doGenerate = true;
            generationDir = argv[i + 1];
            if (generationDir.has_filename())
            {
                std::cerr << "An incorrect directory was provided for generation. Aborting";
                return 1;
            }
            ++i;
            
        }
        else if (arg == "--analyze") {
            ++i;
            while (i < argc && std::string(argv[i]).rfind("--", 0) != 0) {
                imagesToAnalyze.push_back(argv[i]);
                ++i;
            }
            --i;
        }
        else if (arg == "--gui") {
            guiMode = true;
        }
        else if (arg == "--output_directory" && i + 1 < argc && std::string(argv[i + 1]).rfind("--", 0) == std::string::npos) {
            outputDir = argv[i + 1];
            if (outputDir.has_filename())
            {
                std::cerr << "An incorrect directory was provided for output. Aborting";
                return 1;
            }
            ++i;
        }
        else if (arg == "--alpha" && i + 1 < argc) {
            alpha = std::stoi(argv[++i]);
        }
        else if (arg == "--delta" && i + 1 < argc) {
            delta = std::stoi(argv[++i]);
        }
        else {
            std::cout << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }

    if (doGenerate) {
        generateTestImages(generationDir);
    }

    if (!imagesToAnalyze.empty()) {
        misis::GLDMExtractor& extractor = misis::GLDMExtractor::get();
        for (const std::string& img : imagesToAnalyze) {
            extractor.analyzeAndSaveSummary(img, outputDir.string());
            guiResults.push_back(extractor.analyze(img));
        }
    }

    if (guiMode && !guiResults.empty()) {
        showGuiResults(guiResults);
    }


    return 0;
}