//
// Created by leon3428 on 3/20/21.
//

#include <android/native_window.h>
#include "camera_preview.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "config.h"
#include "utils.h"

#define LOG_TAG "NCameraPreview"

CameraPreview::CameraPreview() :m_window(nullptr), m_bufferUpdate(false)
{
    m_imageWidth = 0;
    m_imageHeight = 0;
    m_imageFormat = IMAGE_RGB;
}

CameraPreview::CameraPreview(ANativeWindow *srcWindow) : m_window(srcWindow), m_bufferUpdate(false)
{
    m_imageWidth = 0;
    m_imageHeight = 0;
    m_imageFormat = IMAGE_RGB;
}

CameraPreview::~CameraPreview()
{
    ANativeWindow_release(m_window);
}

void CameraPreview::image_callback(cv::Mat* srcImgPtr)
{
    if(m_window == nullptr)
        return;

    if(m_bufferUpdate)
    {
        ANativeWindow_setBuffersGeometry(m_window, m_imageWidth, m_imageHeight, AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM);
        m_bufferUpdate = false;
    }

    if(srcImgPtr == nullptr)
        return;

    if(m_imageFormat == IMAGE_GRAY)
        cv::cvtColor(*srcImgPtr, *srcImgPtr, cv::COLOR_GRAY2BGR);

    int status = ANativeWindow_lock(m_window, &m_buffer, NULL);
    if(status != 0)
    {
        LOGE("Window lock failed with error: %i\n", status);
        return;
    }

    uchar *dstBuf;
    uchar *srcBuf;
    dstBuf = reinterpret_cast<uchar *>(m_buffer.bits);
    srcBuf = (*srcImgPtr).data;
    int srcRows = (*srcImgPtr).rows;
    int srcCols = (*srcImgPtr).cols;

    for (int i = 0; i < srcRows; i++) {
        memcpy(dstBuf, srcBuf, srcCols * 3);
        srcBuf += srcCols * 3;
        dstBuf += m_buffer.stride * 3;
    }

    ANativeWindow_unlockAndPost(m_window);
}

void CameraPreview::setImageType(int selectedW, int selectedH, imageFormatEnum selectedF)
{
    m_imageWidth = selectedW;
    m_imageHeight = selectedH;
    m_imageFormat = selectedF;
    m_bufferUpdate = true;
}

void CameraPreview::setWindow(ANativeWindow *srcWindow)
{
    m_window = srcWindow;
}
