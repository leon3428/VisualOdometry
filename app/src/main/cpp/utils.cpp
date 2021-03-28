//
// Created by leon3428 on 3/21/21.
//

#include "utils.h"
#include <opencv2/core/core.hpp>

std::string mat2string(cv::Mat *src)
{
    std::string ret = "";
    for(int i = 0;i < src->rows;i++)
    {
        for(int j = 0;j < src->cols;j++)
        {
            ret += std::to_string(src->at<double>(i,j)) + '|';
        }
    }
    return ret;
}