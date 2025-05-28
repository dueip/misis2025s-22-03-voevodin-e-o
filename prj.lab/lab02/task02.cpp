#include <iostream>
#include <opencv2/opencv.hpp>
#include <semcv/semcv.hpp>
#include <array>


namespace misis
{
	namespace
	{
		constexpr int ImageNum = 4;
		constexpr int NoiseNum = 3;
	}
	namespace Brightness
	{
		const std::array<const Vector3, ImageNum> Lumen = { Vector3{0, 127, 255}, Vector3{20, 127, 235}, Vector3{55, 127, 200}, Vector3{90, 127, 165}  };
		const std::array<int, NoiseNum> Clay = { 3, 7, 15 };
	}
	namespace BackgroundColors
	{
		const cv::Scalar FirstColor = cv::Scalar(195);
		const cv::Scalar SecondColor = cv::Scalar(235);
	}
}

cv::Mat make_hist(const cv::Mat& src, const cv::Scalar& bg_color) {
	constexpr int hist_size = misis::CanvasSize;
	const cv::Scalar hist_color = cv::Scalar(0);
	constexpr float range[] = { 0, misis::CanvasSize };
	const float* hist_range = { range };


	cv::Mat hist;
	calcHist(&src, 1, 0, cv::Mat(), hist, 1, &hist_size, &hist_range);

	cv::Mat hist_image(hist_size, hist_size, CV_8UC1, bg_color);

	double maxVal;
	minMaxLoc(hist, 0, &maxVal);
	hist = hist * (250.0 / maxVal);

	for (int i = 0; i < hist_size; i++) {
		line(
			hist_image,
			cv::Point(i, hist_size - cvRound(hist.at<float>(i))),
			cv::Point(i, 256),
			hist_color
		);
	}
	
	return hist_image;
}

cv::Mat make_hist_picture(cv::Mat& inp1, cv::Mat& inp2, cv::Mat& inp3, cv::Mat& inp4,
	cv::Scalar first_bg_color, cv::Scalar second_bg_color)
{
	cv::Mat outp;
	cv::Mat hist1 = make_hist(inp1, first_bg_color);
	cv::Mat hist2 = make_hist(inp2, second_bg_color);
	cv::Mat hist3 = make_hist(inp3, first_bg_color);
	cv::Mat hist4 = make_hist(inp4, second_bg_color);


	try {
		cv::vconcat(inp1, hist1, outp);
		cv::vconcat(outp, inp2, outp);
		cv::vconcat(outp, hist2, outp);
		cv::vconcat(outp, inp3, outp);
		cv::vconcat(outp, hist3, outp);
		cv::vconcat(outp, inp4, outp);
		cv::vconcat(outp, hist4, outp);
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return outp;
}

int main(int argc, char* argv[])
{
	if (!misis::is_debugger_present())
	{
		if (argc == 1)
		{
			std::cerr << "Savefile was not provided; Terminating...";
			return misis::Errors::InvalidParameter;
		}

		if (argc > 2)
		{
			std::cerr << "We save to multiple files. Terminating...";
			return misis::Errors::TooManyOpenFiles;
		}
	}
	
	cv::Mat img;
	cv::Mat hists;

	std::array<cv::Mat, misis::ImageNum> samples;

	for (int i = 0; i < misis::ImageNum; ++i)
	{
		const misis::Vector3 brightness = misis::Brightness::Lumen[i];
		samples[i] = gen_tgtimg00(brightness.x, brightness.y, brightness.z);
	}

	std::array<std::array<cv::Mat, misis::ImageNum>, misis::NoiseNum> noises;
	for (int i = 0; i < noises.size(); ++i)
	{
		for (int j = 0; j < noises[i].size(); ++j)
		{
			noises[i][j] = add_noise_gau(samples[j], misis::Brightness::Clay[i]);
			
		}
	}
	
	std::array<cv::Mat, misis::ImageNum> images;
	for (int i = 0; i < misis::ImageNum; ++i)
	{
		images[i] = make_hist_picture(samples[i], noises[0][i], noises[1][i], noises[2][i], 
			i % 2 == 0 ? misis::BackgroundColors::FirstColor : misis::BackgroundColors::SecondColor,
			i % 2 == 0 ? misis::BackgroundColors::SecondColor : misis::BackgroundColors::FirstColor);
	}
	cv::Mat output;
	cv::hconcat(images[0], images[1], output);
	cv::hconcat(output, images[2], output);
	cv::hconcat(output, images[3], output);
	try
	{
		if (!misis::is_debugger_present())
		{
			std::string path;
			path = argv[1];

			cv::imwrite(path, output);	
		}
		else
		{
			cv::imshow("lab02", output);
			cv::waitKey();
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what();
	}

}