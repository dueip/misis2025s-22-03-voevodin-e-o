#pragma once

#ifndef GLDM_2025
#define GLDM_2025

#include <filesystem>
#include <opencv2/opencv.hpp>

#ifdef _WIN32
#include <Windows.h>
#define DEBUG_BREAK() DebugBreak();
#else
#define DEBUG_BREAK() ;
#endif

namespace 
{
    bool is_debugger_present()
    {
#ifdef _WIN32
        return IsDebuggerPresent();
#endif
        return false;
    }
    __forceinline void debug_break()
    {
#ifdef _WIN32
        DebugBreak();
#endif
    }
}

#define CheckReturn_Body(a) if (!(a)) { if (is_debugger_present()) {  debug_break();  } std::cerr << std::string("==CHECK REPORTED== \nAn error occured, condition ") + #a + " was false! Check happened at: " + __FILE__ + ":" + std::to_string(__LINE__)<< std::endl;
#define CheckReturn_NonVoid(a, return_value)  CheckReturn_Body(a); return return_value;  } 
#define CheckReturn_Void(a) CheckReturn_Body(a);

// I have a very similar at my work so I tried to make my own here. It ended up not being as fancy as my work's, but still helped me a lot through the coursework :)
#define CheckReturn(a, return_value) CheckReturn_NonVoid(a, return_value)

namespace misis {
   
    
    using Real = float;


    class GLDM final
    {
    public:
        GLDM() = default;
        GLDM(const std::filesystem::path& img);
        ~GLDM() = default;

        bool readImage(const std::filesystem::path& img);
        void computeGLDM(Real delta, Real alpha);
        bool importImageFromMat(const cv::Mat& mat);

        bool [[nodiscard]] isImageLoaded() const;


        Real [[nodiscard]] getDependenceNonUniformityFeatureValue() const;

        Real [[nodiscard]] getLowGrayLevelEmphasisFeatureValue() const;


    private:
        cv::Mat image;
        bool wasGlDMComputed : 1 = false;
    };
}

#endif