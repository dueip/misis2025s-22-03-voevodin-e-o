#include <iostream>
#include <semcv/semcv.hpp>
#include <opencv2/highgui.hpp>
#include <filesystem>

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cerr << "Lstfile was not provided; Terminating...";
        return misis::Errors::InvalidParameter;
    }
    if (argc > 2)
    {
        std::cerr << "We cannot open more than one Lstfile. Terminating...";
        return misis::Errors::TooManyOpenFiles;
    }
    
    std::filesystem::path path = argv[1];

    for (auto& el : get_list_of_file_paths(path))
    {
        cv::Mat Mat = cv::imread(el.string(), cv::IMREAD_UNCHANGED);
        
        const std::string strid = strid_from_mat(Mat);
        if (strid == el.filename().replace_extension(""))
        {
            std::cout << el.filename().string() << "\t" << "good" << std::endl;
        }
        else
        {
            std::cout << el.filename().string() << "\t" << "bad, should be " << strid << std::endl;
        }
    }
}