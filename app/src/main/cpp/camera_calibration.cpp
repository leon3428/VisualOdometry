//
// Created by leon3428 on 3/21/21.
//

#include "camera_calibration.h"
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d.hpp>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include "config.h"
#include "utils.h"
#define LOG_TAG "camera calibration"
#include <iostream>

void CameraCalibration::image_callback(cv::Mat* src){
    if(m_data_lock)
        return;

    auto time = std::chrono::system_clock::now();
    std::chrono::duration<double> t = time - m_lastTry;
    if(t.count() < 2)
    {
        if(m_preview != nullptr)
            m_preview -> image_callback(src);
        return;
    }


    cv::Mat gray;

    cv::cvtColor(*src, gray, cv::COLOR_RGB2GRAY);

    std::vector<cv::Point2f> point_buf;

    bool found = findChessboardCorners(gray, cv::Size(m_chSize[0],m_chSize[1]), point_buf, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
    if(found)
    {
        cornerSubPix( gray, point_buf, cv::Size(11, 11),
                      cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.0001 ));

        if(m_preview != nullptr)
        {
            cv::drawChessboardCorners(*src, cv::Size(m_chSize[0], m_chSize[1]), point_buf, found);
            m_preview -> image_callback(src);
        }


        m_imagePoints.push_back(point_buf);
        m_objectPoints.push_back(m_objp);
    }

    m_lastTry = std::chrono::system_clock::now();
}

int CameraCalibration::imagePointsSize() {
    return m_imagePoints.size();
}

void CameraCalibration::setPreview(CameraPreview *preview){
    m_preview = preview;
}

void CameraCalibration::generateObjectPoints() {
    for(int i = 0;i < m_chSize[1];i++)
    {
        for(int j = 0;j < m_chSize[0];j++)
        {
            m_objp.push_back(cv::Point3f(j*25, i*25, 0));
        }
    }
}

CameraCalibration::CameraCalibration() {
    generateObjectPoints();
}

std::string CameraCalibration::calibrate() {
    m_data_lock = true;
    if(m_imagePoints.size() < 20)
        return "Failed";
    cv::Mat cameraMatrix,distCoeffs,R,T;
    cv::calibrateCamera(m_objectPoints, m_imagePoints, cv::Size(image_height, image_width), cameraMatrix, distCoeffs, R, T);

    std::string ret = mat2string(&cameraMatrix) + mat2string(&distCoeffs);

    m_data_lock = false;
    return ret;
}