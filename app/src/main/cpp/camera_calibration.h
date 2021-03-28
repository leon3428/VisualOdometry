//
// Created by leon3428 on 3/21/21.
//

#ifndef VISUALODOMETRY_CAMERA_CALIBRATION_H
#define VISUALODOMETRY_CAMERA_CALIBRATION_H

#include "interfaces.h"
#include <opencv2/core/core.hpp>
#include <vector>
#include "camera_preview.h"
#include <chrono>

class CameraCalibration : public ImageProcessor{
private:
    std::vector< std::vector<cv::Point3f> > m_objectPoints;
    std::vector<std::vector<cv::Point2f> > m_imagePoints;
    std::vector<cv::Point3f> m_objp;
    CameraPreview *m_preview = nullptr;
    std::chrono::time_point<std::chrono::system_clock> m_lastTry;
    int m_chSize[2] = {9, 6};
    bool m_data_lock = false;

public:
    void image_callback(cv::Mat* src);
    int imagePointsSize();
    void setPreview(CameraPreview *preview);
    void generateObjectPoints();
    std::string calibrate();
    CameraCalibration();
};

#endif //VISUALODOMETRY_CAMERA_CALIBRATION_H
