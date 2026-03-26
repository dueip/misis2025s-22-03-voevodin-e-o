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
#define CheckReturn_Void(a) CheckReturn_Body(a); }

// I have a very similar at my work so I tried to make my own here. It ended up not being as fancy as my work's, but still helped me a lot through the coursework :)
#define CheckReturn(a, return_value) CheckReturn_NonVoid(a, return_value)

namespace misis {
   
    
    using Real = float;

    /// \brief Класс для вычисления GLDM (Gray Level Dependence Matrix) характеристик изображения.
    class GLDM final
    {
    public:
        /// \brief Конструктор по умолчанию.
        GLDM() = default;

        /// \brief Конструктор с загрузкой изображения.
        /// \param[in] img Путь к изображению.
        GLDM(const std::filesystem::path& img, const Real alpha, const Real delta);

        /// \brief Деструктор по умолчанию.
        ~GLDM() = default;

        /// \brief Загружает изображение из файла.
        /// \param[in] img Путь к изображению.
        /// \return `true`, если изображение успешно загружено, иначе `false`.
        bool readImage(const std::filesystem::path& img, const Real alpha, const Real delta);

        /// \brief Вычисляет GLDM и соответствующие признаки.
        /// \param[in] delta Параметр допуска для определения зависимостей уровней серого.
        /// \param[in] alpha Параметр веса зависимости.
        void computeGLDM(Real delta, Real alpha);

        /// \brief Импортирует изображение из объекта OpenCV `cv::Mat`.
        /// \param[in] mat Изображение.
        /// \return `true`, если импорт прошел успешно, иначе `false`.
        bool importImageFromMat(const cv::Mat& mat);

        /// \brief Проверяет верную загрузку изображения.
        bool [[nodiscard]] isImageLoaded() const;

        /// \brief Возвращает значение признака Dependence Non-Uniformity (DN).
        Real [[nodiscard]] getDependenceNonUniformityFeatureValue() const;
        
        /// \brief Возвращает значение признака Low Gray Level Emphasis (LGLE).
        Real [[nodiscard]] getLowGrayLevelEmphasisFeatureValue() const;

    private:
        cv::Mat image; ///< Изображение для анализа.
        bool wasGlDMComputed : 1 = false; ///< Флаг на вычисление матрицы.
    };
}

#endif