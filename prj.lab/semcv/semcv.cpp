#include <semcv/semcv.hpp>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
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

cv::Mat autocontrast(const cv::Mat& img, const double q_black, const double q_white) 
{

	std::vector<cv::Mat> channels;
	cv::split(img, channels);

	int canvas_size = misis::CanvasSize;
	float range[] = { 0, canvas_size };
	const float* histRange = range;

	for (auto& channel : channels) 
	{

		cv::Mat hist;
		cv::calcHist(&channel, 1, 0, cv::Mat(), hist, 1, &canvas_size, &histRange);

		const double area = img.rows * img.cols;
		if (std::abs(area) < std::numeric_limits<double>::epsilon())
		{
			return cv::Mat();
		}
		double sum_min = 0;

		int minEdge = 0;
		int maxEdge = 0xFF;

		for (int i = 0; i < canvas_size; ++i) 
		{
			sum_min += hist.at<float>(i);
			if (sum_min / area > q_black)
			{
				minEdge = i;
				break;
			}
		}
		double sum_max = 0;
		for (int i = 0xFF; i >= 0; --i)
		{
			sum_max += hist.at<float>(i);
			if (sum_max / area > q_white) 
			{
				maxEdge = i;
				break;
			}
		}
		double normal = static_cast<double>(0xFF) / (maxEdge - minEdge);
		cv::Mat edges(1, canvas_size, CV_8U);
		for (int i = 0; i < canvas_size; ++i) 
		{

			if (i < minEdge) 
			{
				edges.at<uchar>(i) = 0;
			}
			else if (i > maxEdge) 
			{
				edges.at<uchar>(i) = 0xFF;
			}
			else 
			{
				edges.at<uchar>(i) = cv::saturate_cast<uchar>((i - minEdge) * normal);
			}
		}
		cv::LUT(channel, edges, channel);
	}
	cv::Mat contrasted;
	cv::merge(channels, contrasted);
	return contrasted;
}

cv::Mat autocontrast_rgb(const cv::Mat& img, const double q_black, const double q_white)
{
	cv::Mat result;

	std::vector<cv::Mat> channels;
	cv::split(img, channels);

	int hist_size = misis::CanvasSize;
	float range[] = { 0, hist_size };
	const float* histRange = range;
	
	cv::Mat hist;
	cv::Mat white_hist;
	cv::Mat black_hist;
	cv::Mat min_bg;
	cv::Mat max_bg;
	cv::Mat white;
	cv::Mat black;

	cv::min(channels[0], channels[1], min_bg);
	cv::min(min_bg, channels[2], black);
	cv::max(channels[0], channels[1], max_bg);
	cv::max(max_bg, channels[2], white);

	cv::calcHist(&black, 1, 0, cv::Mat(), black_hist, 1, &hist_size, &histRange);
	cv::calcHist(&white, 1, 0, cv::Mat(), white_hist, 1, &hist_size, &histRange);

	const double area = img.rows * img.cols;
	double sum_min = 0;
	int min_edge = 0;
	int max_edge = 0xFF;
	for (int i = 0; i < hist_size; ++i) 
	{
		sum_min += black_hist.at<float>(i);
		if (sum_min / area > q_black) 
		{
			min_edge = i;
			break;
		}
	}
	double sum_max = 0;
	for (int i = 0xFF; i >= 0; --i) 
	{
		sum_max += white_hist.at<float>(i);
		if (sum_max / area > q_white) 
		{
			max_edge = i;
			break;
		}
	}
	double normal = static_cast<float>(0xFF) / (max_edge - min_edge);
	cv::Mat edges(1, hist_size, CV_8U);
	for (int i = 0; i < hist_size; ++i) 
	{
		if (i < min_edge) 
		{
			edges.at<uchar>(i) = 0;
		}
		else if (i > max_edge)
		{
			edges.at<uchar>(i) = 255;
		}
		else 
		{
			edges.at<uchar>(i) = cv::saturate_cast<uchar>((i - min_edge) * normal);
		}
	}
	for (auto& channel : channels) 
	{
		cv::LUT(channel, edges, channel);
	}
	cv::merge(channels, result);

	return result;
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