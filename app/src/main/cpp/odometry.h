//
// Created by leon3428 on 3/20/21.
//

#ifndef VISUALODOMETRY_ODOMETRY_H
#define VISUALODOMETRY_ODOMETRY_H

#include <opencv2/core/core.hpp>
#include "interfaces.h"
#include "camera_preview.h"
#include <chrono>

class Odometry : public ImageProcessor{
private:
    cv::Mat m_cameraMatrix, m_distCoeffs;
    CameraPreview* m_preview;

    cv::Mat m_previous;
    std::vector<cv::Point2f> m_curFeatures, m_prevFeatures;
    std::vector<uchar> m_status;
    cv::Mat m_R, m_t;
    double m_s;
    cv::Mat m_path;

    void visualizePath();

public:
    bool undistort = true;
    static int MIN_POINTS;
    static int MAX_DISTANCE;
    static double PLOT_SCALE;
    CameraPreview m_path_preview;
    
    ~Odometry();
    Odometry();
    void image_callback(cv::Mat *src) override;
    bool load_params(std::string params);
    void setPreview(CameraPreview* preview);
    void featureDetection(cv::Mat src, std::vector<cv::Point2f>& points);
    void featureTracking(cv::Mat frame1, cv::Mat frame2, std::vector<cv::Point2f>& points1, std::vector<cv::Point2f>& points2, std::vector<uchar>& status);
    float pointDistance(cv::Point2f a, cv::Point2f b);
    void clearPath();

};

#endif //VISUALODOMETRY_ODOMETRY_H
