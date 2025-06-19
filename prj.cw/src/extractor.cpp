#include "extractor.hpp"
#include "gldm.hpp"

void misis::GLDMExtractor::saveSummaryToFile(const std::string& originalName, const std::string& output_path, misis::Real LGLE, misis::Real DN) {
    std::string outName = output_path + "summary_" + originalName + ".txt";
    std::ofstream file(outName);

    CheckReturn(file.is_open());

    file << "GLDM Feature Analysis Summary for Image: " << originalName << "\n";
    file << "----------------------------------------------\n";
    file << std::fixed << std::setprecision(6);
    file << "Low Gray Level Emphasis (LGLE): " << LGLE << "\n";
    file << "Dependence Non-Uniformity (DN): " << DN << "\n";
    file << "\nInterpretation:\n";

    if (LGLE > 0.05)
        file << "- High LGLE indicates a concentration of low-intensity pixels.\n";
    else
        file << "- Low LGLE suggests fewer low-gray pixels in the image.\n";

    if (DN < 0.5)
        file << "- Low DN indicates uniform texture and smooth dependencies.\n";
    else
        file << "- High DN suggests complex or heterogeneous texture patterns.\n";

    file.close();
    std::cout << "Summary written to: " << outName << std::endl;
}

void misis::GLDMExtractor::analyzeAndSaveSummary(const std::string& imagePath, const std::string& output_path) {

    misis::GLDM gldm(imagePath);
    double LGLE = gldm.getLowGrayLevelEmphasisFeatureValue();
    double DN = gldm.getDependenceNonUniformityFeatureValue();

    size_t pos = imagePath.find_last_of("/\\");
    std::string nameOnly = (pos != std::string::npos) ? imagePath.substr(pos + 1) : imagePath;

    saveSummaryToFile(nameOnly, output_path, LGLE, DN);
}

misis::AnalysisResult misis::GLDMExtractor::analyze(const std::string& imagePath)
{
        cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
        if (image.empty()) {
            std::cerr << "Failed to load image: " << imagePath << std::endl;
            return { imagePath, -1, -1, "Invalid" };
        }

        misis::GLDM gldm(imagePath);
        double LGLE = gldm.getLowGrayLevelEmphasisFeatureValue();
        double DN = gldm.getDependenceNonUniformityFeatureValue();

        std::string category;
        if (LGLE > 0.1 && DN < 500)
            category = "Uniform Texture with Low Gray Levels";
        else if (LGLE > 0.1 && DN >= 500)
            category = "Heterogeneous Texture with Low Gray Levels";
        else if (LGLE <= 0.1 && DN < 500)
            category = "Uniform Texture with Mixed Gray Levels";
        else
            category = "Heterogeneous Texture with Mixed or High Gray Levels";

        return { imagePath, LGLE, DN, category };
}
