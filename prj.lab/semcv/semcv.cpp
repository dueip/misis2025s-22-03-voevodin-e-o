#include <semcv/semcv.hpp>
#include <fstream>
#include <iostream>

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