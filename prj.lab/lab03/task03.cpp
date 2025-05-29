#include <iostream>
#include <semcv/semcv.hpp>

constexpr std::string_view help_string = "This program should be supplied with 5 arguments in total. They should be entered as follows: \n 1) autocontrast type (rgb | naive) \n 2) image path \n 3) black quantile \n 4) white quntile \n 5) output image path";

namespace
{
    struct RequiredParams
    {
        misis::TYPE autocontrast_type;
        std::filesystem::path input_image;
        double black_quantile = 0;
        double white_quantile = 0;
        std::filesystem::path output_image;
    };

    const int parse_params(int argc, char* argv[], RequiredParams& OutParams)
    {
        if (argc == 1)
        {

            std::cerr << help_string << "; Terminating...";
            return misis::Errors::InvalidParameter;
        }

        if (argc == 2 || argv[1] == "help")
        {
            std::cout << help_string;
            return misis::Errors::InvalidName;
        }

        if (argc != 6)
        {
            std::cerr << "Incorrect input; Terminating...";
            return misis::Errors::InvalidParameter;
        }

        if (argv[1] != misis::AutocontractType::naive && argv[1] != misis::AutocontractType::rgb)
        {
            std::cerr << "type help to get help";
            return misis::Errors::InvalidParameter;
        }

        OutParams.autocontrast_type = argv[1];
        OutParams.input_image = argv[2];
        try {
            OutParams.black_quantile = std::stod(argv[3]);
            OutParams.white_quantile = std::stod(argv[4]);
            OutParams.output_image = argv[5];
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what();
            return misis::Errors::InvalidParameter;
        }
        return 0;
    }
}

int main(int argc, char* argv[])
{
    RequiredParams Params;
    if (!misis::is_debugger_present())
    {
       
        const auto error = parse_params(argc, argv, Params);
        if (error != 0)
        {
            return error;
        }
    }

    try
    {
        cv::Mat out;
        cv::Mat img = cv::imread(Params.input_image.string());
        if (Params.autocontrast_type == misis::AutocontractType::naive)
        {
            out = autocontrast(img, Params.black_quantile, Params.white_quantile);
        }
        if (Params.autocontrast_type == misis::AutocontractType::rgb)
        {
            out = autocontrast_rgb(img, Params.black_quantile, Params.white_quantile);
        }
        cv::imwrite(Params.output_image.string(), out);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what();
    }
}
