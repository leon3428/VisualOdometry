//
// Created by leon3428 on 3/20/21.
//

#include "interfaces.h"

void ImageProcessor::setImageType(int W, int H, imageFormatEnum F)
{
    m_imageWidth = W;
    m_imageHeight = H;
    m_imageFormat = F;
}