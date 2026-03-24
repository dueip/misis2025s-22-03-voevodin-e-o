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
    ///    \brief Структура, хранящий результат анализа.
    ///    
    /// Категории задаются по условиям и могут быть следущих типов:
    /// - Uniform Texture with Low Gray Levels
    /// - Heterogeneous Texture with Low Gray Levels
    /// - Heterogeneous Texture with Low Gray Levels
    /// - Heterogeneous Texture with Mixed or High Gray Levels
    struct AnalysisResult {
    std::string imageName; ///< Название файла. 
    double LGLE; ///< Признак низкого уровня серого.
    double DN; ///< Признак неравномерности.
    std::string category; ///< Категория итогово изображения.
};

     /// \brief Класс для анализа изображений с использованием GLDM.
     /// 
     /// Предоставляет методы анализа изображения и сохранения результатов в файл.
    class GLDMExtractor {
    public:
        /// \brief Возвращает синглтон класса..
        static GLDMExtractor& get()
        {
            static GLDMExtractor extractor;
            return extractor;
        }
        
         /// \brief Анализирует изображение и сохраняет результаты в файл.
         /// \param[in] imagePath Путь к исходному изображению.
         /// \param[in] output_path Путь к файлу для соранения результатов.
        void analyzeAndSaveSummary(const std::string& imagePath, const std::string& output_path);

        /// \brief Выполняет анализ изображения и возвращает результаты.
        /// \param[in] ImagePath Путь к анализируемому изображению..
        AnalysisResult analyze(const std::string& ImagePath);
    private:
        /// \brief Сохраняет результаты анализа в txt.
        /// \param[in] originalName Имя изображеения.
        /// \param[in] output_path Расположение выходного файла.
        /// \param[in] LGLE Значение признака LGLE.
        /// \param[in] DN Значение признака DN.
        void saveSummaryToFile(const std::string& originalName, const std::string& output_path, misis::Real LGLE, misis::Real DN);
        
    };
}

#endif