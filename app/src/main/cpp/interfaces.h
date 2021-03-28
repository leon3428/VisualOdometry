//
// Created by leon3428 on 3/20/21.
//

#ifndef VISUALODOMETRY_INTERFACES_H
#define VISUALODOMETRY_INTERFACES_H

#include <opencv2/core/core.hpp>

enum imageFormatEnum {IMAGE_GRAY, IMAGE_RGB};

class ImageProcessor{
public:
    int m_imageWidth;
    int m_imageHeight;
    imageFormatEnum m_imageFormat;

    virtual void image_callback(cv::Mat* src) = 0;
    void setImageType(int W, int H, imageFormatEnum F);

};

#endif //VISUALODOMETRY_INTERFACES_H
