#pragma once
#ifndef MISIS2025S_3_SEMCV
#define MISIS2025S_3_SEMCV

#include <filesystem>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

namespace misis 
{
    namespace Errors 
    {
#ifdef _WIN32
        constexpr unsigned char TooManyOpenFiles = 4;
        constexpr unsigned char InvalidParameter = 87;
        constexpr unsigned char InvalidName = 123;
#else
#ifdef __linux__
        // Linux does not really have errorcodes, so I used
        // 1 (a general error code) to indicate toomanyopenfiles and 
        // 73 (a "cannot create") to indicate invalidname. Thought they'd fit :)
        constexpr unsigned char TooManyOpenFiles = 1;
        constexpr unsigned char InvalidParameter = 3;
        constexpr unsigned char InvalidName = 73;
#endif

#endif
    }
    using TYPE = std::string_view;
    namespace ImageType
    {
        constexpr TYPE uint08 = "uint08";
        constexpr TYPE sint08 = "sint08";
        constexpr TYPE uint16 = "uint16";
        constexpr TYPE sint16 = "sint16";
        constexpr TYPE uint32 = "uint32"; // CV_32U does not exits :/ Will still leave it here since it was required, but yeah
        constexpr TYPE sint32 = "sint32";
        constexpr TYPE real32 = "real32";
        constexpr TYPE real64 = "real64";
    }

    struct Vector3 final
    {
        int x = 0;
        int y = 0;
        int z = 0;
    };

    constexpr double CircleRadius = 83.0;
    constexpr double CanvasSize = 256;

    bool is_debugger_present();
}

cv::Mat add_noise_gau(const cv::Mat& img, const int std);

std::string strid_from_mat(const cv::Mat& img, const int n = 4);

std::vector<std::filesystem::path> get_list_of_file_paths(const std::filesystem::path& path_lst);
    
cv::Mat gen_tgtimg00(const int lev0, const int lev1, const int lev2);

#endif