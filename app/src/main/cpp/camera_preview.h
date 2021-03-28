//
// Created by leon3428 on 3/20/21.
//

#ifndef VISUALODOMETRY_CAMERA_PREVIEW_H
#define VISUALODOMETRY_CAMERA_PREVIEW_H

#include <opencv2/core/core.hpp>
#include <android/native_window.h>
#include "interfaces.h"

class CameraPreview : public ImageProcessor {
private:
    ANativeWindow *m_window;
    bool m_bufferUpdate;
    ANativeWindow_Buffer m_buffer;

public:
    CameraPreview(ANativeWindow *srcWindow);

    CameraPreview();

    ~CameraPreview();

    void setImageType(int selectedW, int selectedH, imageFormatEnum selectedF);

    void setWindow(ANativeWindow *srcWindow);

    void image_callback(cv::Mat *src);
};

#endif //VISUALODOMETRY_CAMERA_PREVIEW_H
