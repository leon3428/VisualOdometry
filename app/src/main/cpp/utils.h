//
// Created by leon3428 on 3/20/21.
//

#ifndef VISUALODOMETRY_UTILS_H
#define VISUALODOMETRY_UTILS_H

#include <android/log.h>
#include <jni.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <opencv2/core/core.hpp>


#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

std::string mat2string(cv::Mat *src);

#endif //VISUALODOMETRY_UTILS_H
