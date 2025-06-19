#pragma once

#ifndef GLDMExtractor_2025
#define GLDMExtractor_2025
#include <opencv2/opencv.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "gldm.hpp"

namespace misis
{
    struct AnalysisResult {
    std::string imageName;
    double LGLE;
    double DN;
    std::string category;
};

    class GLDMExtractor {
    public:
        static GLDMExtractor& get()
        {
            static GLDMExtractor extractor;
            return extractor;
        }
        void analyzeAndSaveSummary(const std::string& imagePath, const std::string& output_path);
        AnalysisResult analyze(const std::string& ImagePath);
    private:
        void saveSummaryToFile(const std::string& originalName, const std::string& output_path, misis::Real LGLE, misis::Real DN);
        
    };
}

#endif