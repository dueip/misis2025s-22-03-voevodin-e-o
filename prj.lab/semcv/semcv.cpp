#include <semcv/semcv.hpp>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif
#include <random>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

std::string strid_from_mat(const cv::Mat& img, const int n)
{
    using namespace misis;
    
    const int width = img.cols;
    const int height = img.rows;
    const int channels = img.channels();
    
    TYPE image_type;
    switch (img.depth())
    {
		case CV_8U:
			image_type = ImageType::uint08;
			break;
		case CV_8S:
			image_type = ImageType::sint08;
			break;
		case CV_16U:
			image_type = ImageType::uint16;
			break;
		case CV_16S:
			image_type = ImageType::sint16;
			break;
		case CV_32S:
			image_type = ImageType::sint32;
			break;
		case CV_32F:
			image_type = ImageType::real32;
			break;
		case CV_64F:
			image_type = ImageType::real64;
			break;
	}
   
    std::stringstream sstream;
    std::stringstream height_stream;
    sstream << std::setfill('0') << std::setw(n) << width;
    const std::string& padded_width = sstream.str();
	height_stream << std::setfill('0') << std::setw(n) << height;
    const std::string& padded_height = height_stream.str();

    // Format as WWWWxHHHH.C.TYPE. WWWWxHHHH is an n x n structure
    return std::format("{}x{}.{}.{}", padded_width, padded_height, channels, image_type);
}


std::vector<std::filesystem::path> get_list_of_file_paths(const std::filesystem::path& path_lst)
{
	std::vector<std::filesystem::path> files;

	std::ifstream lstfile;

	lstfile.open(path_lst);
	std::string line;

	if (!lstfile.is_open())
	{
		std::cerr << "Could not open the file; Terminating...";
		lstfile.close(); // Useless, but a good practice nonetheless! 
		return files;
	}

	while (std::getline(lstfile, line))
	{
		std::filesystem::path current_file_path = path_lst.parent_path().concat("/" + line);
		files.push_back(current_file_path);
	}

	lstfile.close();

	return files;
}

cv::Mat gen_tgtimg00(const int lev0, const int lev1, const int lev2)
{
	cv::Mat img = cv::Mat(misis::CanvasSize, misis::CanvasSize, CV_8UC1, cv::Scalar(lev0));

	cv::Point AA = cv::Point(23, 23);
	cv::Point BB = cv::Point(233, 233);
	cv::Rect AABB = cv::Rect(AA, BB);

	cv::rectangle(img, AABB, cv::Scalar(lev1), -1);

	cv::Point centerPoint = cv::Point(misis::CanvasSize / 2.0, misis::CanvasSize  / 2.0);

	cv::circle(img, centerPoint, misis::CircleRadius, cv::Scalar(lev2), -1);

	return img;
}

cv::Mat add_noise_gau(const cv::Mat& img, const int std)
{
	cv::Mat out = img.clone();

	std::default_random_engine generator;
	std::normal_distribution<double> z_dist(0, std);

	for (int i = 0; i < out.rows; ++i)
	{
		for (int j = 0; j < out.cols; ++j)
		{
			const double noise = z_dist(generator);
			double new_value = out.at<uchar>(i, j) + noise;
			new_value = std::clamp(new_value, 0.0, 255.0);
			out.at<uchar>(i, j) = static_cast<uchar>(new_value);
		}
	}

	return out;
}
// There is a c++26 function that allows detect this without using external libraries,
// but I don't really want to support c++26 yet :) 
bool misis::is_debugger_present()
{
#ifdef _WIN32
	return IsDebuggerPresent();
#endif
	// Just a stub for other platforms. I do not really debug on anything but windows, so it's an okay-ish way of dealing with this
	// Probably should call std::is_debugger_present later :) 
	return false;
}