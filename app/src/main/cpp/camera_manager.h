//
// Created by leon3428 on 3/20/21.
//

#ifndef VISUALODOMETRY_CAMERA_MANAGER_H
#define VISUALODOMETRY_CAMERA_MANAGER_H

#include "camera_preview.h"
#include <opencv2/core/core.hpp>
#include <media/NdkImageReader.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraManager.h>
#include "interfaces.h"

class CameraManager{
private:
    ACameraDevice *m_cameraDevice;
    ACaptureRequest *m_captureRequest;
    ANativeWindow *m_window;
    ACameraOutputTarget *m_cameraOutputTarget;
    ACaptureSessionOutput *m_sessionOutput;
    ACaptureSessionOutputContainer *m_captureSessionOutputContainer;
    ACameraCaptureSession *m_captureSession;
    AImageReader *m_reader = nullptr;
    cv::Mat *m_inputImg = nullptr;
    bool m_sessionRunning = false;
    ACameraDevice_StateCallbacks m_deviceStateCallbacks;
    ACameraCaptureSession_stateCallbacks m_captureSessionStateCallbacks;
    ImageProcessor *m_output_processor = nullptr;

    void openCamera();
    void imageProcessor();
    static void camera_device_on_disconnected(void *context, ACameraDevice *device);
    static void camera_device_on_error(void *context, ACameraDevice *device, int error);
    static void capture_session_on_ready(void *context, ACameraCaptureSession *session);
    static void capture_session_on_active(void *context, ACameraCaptureSession *session);


public:
    void openSession();
    void closeSession();
    void setProcessor(ImageProcessor *processor);
    ~CameraManager();
    CameraManager();
    CameraManager(ImageProcessor *processor);
};

#endif //VISUALODOMETRY_CAMERA_MANAGER_H
