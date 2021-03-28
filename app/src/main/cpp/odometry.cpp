//
// Created by leon3428 on 3/20/21.
//

#include "odometry.h"
#include "utils.h"
#define LOG_TAG "odometry"
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/video.hpp>
#include <chrono>

bool Odometry::load_params(std::string params) {
    std::string number = "";
    std::vector<float> nums;

    for (int i = 0; i < params.size(); i++) {
        if (params[i] == '|') {
            nums.push_back(stof(number));
            number = "";
        } else
            number += params[i];
    }

    if (nums.size() != 14)
        return false;

    int ind = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_cameraMatrix.at<float>(i, j) = nums[ind];
            ind++;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        m_distCoeffs.at<float>(0,i) = nums[ind];
        ind++;
    }


    return true;
}

void Odometry::image_callback(cv::Mat *src) {
    cv::Mat grey, undistorted;
    cv::cvtColor(*src, grey, cv::COLOR_RGB2GRAY);

    if(undistort)
        cv::undistort(grey, undistorted, m_cameraMatrix, m_distCoeffs);
    else
        undistorted = grey;

    cv::blur(undistorted, undistorted, cv::Size(5,5));

    if(m_prevFeatures.size() < MIN_POINTS)
    {
        featureDetection(undistorted, m_curFeatures);
    }
    else
    {
        featureTracking(m_previous, undistorted, m_prevFeatures, m_curFeatures, m_status);

        if(m_curFeatures.size() >= 10)
        {
            cv::Mat transform = cv::estimateAffinePartial2D(m_prevFeatures, m_curFeatures);

            double x = transform.at<double>(0,0);
            double y = transform.at<double>(1,0);
            double s = sqrt(x*x + y*y);
            cv::Mat temp = transform.colRange(0,2);
            cv::Mat rotmat = temp.clone();
            rotmat /= s;

            cv::Mat tvec = transform.colRange(2,3);

            m_R = rotmat * m_R;
            m_s *= s;
            m_t = m_t + m_R * tvec;
            visualizePath();
        }

    }

    m_previous = undistorted.clone();
    for(auto pt : m_curFeatures)
    {
        cv::circle(undistorted, pt, 10, 255, -1);
    }
    for(auto pt : m_prevFeatures)
    {
        //m_prevFeatures[i] = cv::Point2f(0,0);
        cv::circle(undistorted, pt, 10, 0, -1);
    }

    m_prevFeatures = m_curFeatures;

    m_preview -> image_callback(&undistorted);
}

Odometry::~Odometry() {
    m_cameraMatrix.release();
    m_distCoeffs.release();
}

Odometry::Odometry() {
    m_cameraMatrix = cv::Mat(3,3,CV_32F);
    m_distCoeffs = cv::Mat(1,5,CV_32F);
    clearPath();
    srand (time(NULL));
}

void Odometry::setPreview(CameraPreview *preview) {
    m_preview = preview;
}

void Odometry::featureDetection(cv::Mat src, std::vector<cv::Point2f> &points) {
    cv::goodFeaturesToTrack(src, points, 500, 0.1, 10);
}

void Odometry::featureTracking(cv::Mat frame1, cv::Mat frame2, std::vector<cv::Point2f> &points1,
                               std::vector<cv::Point2f> &points2, std::vector <uchar> &status) {
    std::vector<float> error;
    cv::Size winSize(21,21);
    cv::TermCriteria termcrit(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.08);

    cv::calcOpticalFlowPyrLK(frame1, frame2, points1, points2, status, error, winSize, 4, termcrit, 0, 0.001);

    int indexCorrection = 0;
    for(int i = 0;i < status.size();i++)
    {
        cv::Point2f pt2 = points2[i - indexCorrection];
        cv::Point2f pt1 = points1[i - indexCorrection];
        if(pointDistance(pt1, pt2) > MAX_DISTANCE)
            status[i] = 0;
        if (status[i] == 0 || pt2.x < 0 || pt2.y < 0)
        {
            if(pt2.x < 0 || pt2.y < 0)	{
                status[i] = 0;
            }
            points1.erase (points1.begin() + i - indexCorrection);
            points2.erase (points2.begin() + i - indexCorrection);
            indexCorrection++;
        }

    }
}

float Odometry::pointDistance(cv::Point2f a, cv::Point2f b) {
    float ret = (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
    return sqrt(ret);
}

void Odometry::clearPath() {
    m_R = cv::Mat::eye(2,2,CV_64F);
    m_t = cv::Mat::zeros(2,1, CV_64F);
    m_s = 1.0;
    m_path = cv::Mat(360, 360, CV_8UC3, cv::Scalar(0));
}

void Odometry::visualizePath() {
    cv::circle(m_path, cv::Point2d(m_t.at<double>(0) * PLOT_SCALE + 180.0, m_t.at<double>(1) * PLOT_SCALE + 180.0), 5, cv::Scalar(255,0,0), -1);
    m_path_preview.image_callback(&m_path);
}

int Odometry::MIN_POINTS = 10;
int Odometry::MAX_DISTANCE = 80;
double Odometry::PLOT_SCALE = 0.008;