#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

bool write_file_yuv(std::vector<cv::Mat> org_img_set, std::vector<std::string> image_name_set) {

    for (int i = 0; i < org_img_set.size(); i++) {

        cv::Mat conv_img;
        cv::cvtColor(org_img_set[i], conv_img, cv::COLOR_BGR2YUV); //test;

        cv::Mat dst[3];
        cv::split(conv_img, dst);

        cv::resize(dst[1], dst[1], cv::Size(), 0.5, 0.5, cv::INTER_CUBIC);
        cv::resize(dst[2], dst[2], cv::Size(), 0.5, 0.5, cv::INTER_CUBIC);
        
        std::string filepath = image_name_set[i];
        std::ofstream stream(filepath, std::ios::binary);


        if (!stream.is_open()) {

            throw std::runtime_error("Failed to open YUV output image");
        }
        stream.write(reinterpret_cast<char const*>(dst[0].data), dst[0].size().area() * dst[0].elemSize());
        stream.write(reinterpret_cast<char const*>(dst[1].data), dst[1].size().area() * dst[1].elemSize());
        stream.write(reinterpret_cast<char const*>(dst[2].data), dst[2].size().area() * dst[2].elemSize());
    }

    return true;//test 
}


int main(int argc, char* argv[])
{
    if (argc < 3) {
        fprintf(stdout, "ERROR::No input dataset\nIn order to run the program type: 'Mat2YUV.exe [the path of input data] [the path of output data] [(optional) data name]'\n");
        fprintf(stdout, "EXAMPLE::Pose_Estimation.exe ./pic cam_param.json 1 ./out_pic'\n");
        return -1; 
    }

   std::string path_ = argv[1];
   std::string out_path_ = argv[2];
   std::string data_name_ = (argc > 3) ? argv[3] : "MV";
   std::vector<cv::String> img_names;
    try
    {
        cv::glob(path_, img_names, true);
        if (img_names.size() < 2)
            throw   img_names;
    }
    catch (cv::Exception)
    {
        std::cout << "There are no folder named " << path_ << " in tihs directory" << std::endl;
        getchar();
        return -1;
    }
    catch (std::vector<cv::String> img_names_)
    {
        std::cout << "Need more images in " << path_ << std::endl;
        getchar();
        return -1;
    }
    int num_images = static_cast<int>(img_names.size());

    if (num_images >= 1000) {
        std::cerr << "Too many pictures to process" << std::endl;
        return -1;
    }

    std::vector<cv::Mat> org_img_set(num_images);

    std::cout << "# of pictures : " << num_images << std::endl;
 
    //cv::Mat --> yuv stream
    std::vector<std::string> yuv_name_set(num_images);
    for (int i = 0; i < num_images; ++i)
    {
        org_img_set[i] = cv::imread(img_names[i]);

        std::string zero_idx;
        if (num_images < 100 && i < 10) zero_idx = "0";
        else if (num_images > 100 && i < 10) zero_idx = "00";
        else if (num_images > 100 && i < 100) zero_idx = "0";
        else zero_idx = "";
        std::string mid_name = std::to_string(org_img_set[i].cols) + "x" + std::to_string(org_img_set[i].rows) + "_" + zero_idx + std::to_string(i);
        yuv_name_set[i] = out_path_ + "/" + data_name_ + "_" + mid_name + ".yuv";
        //std::cout << yuv_name_set[i] << std::endl;
    }

    if (!write_file_yuv(org_img_set, yuv_name_set)) {
        fprintf(stdout, "ERROR::write_file_yuv'\n");
        return -1;
    }

    std::cout << "[DONE]" << std::endl;
    return 0;
}
