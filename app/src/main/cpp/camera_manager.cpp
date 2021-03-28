//
// Created by leon3428 on 3/20/21.
//

#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraManager.h>
#include <media/NdkImage.h>
#include <camera/NdkCameraMetadata.h>
#include <cmath>
#include <algorithm>
#include <media/NdkImageReader.h>
#include <thread>
#include <chrono>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "utils.h"
#include "config.h"
#include "camera_manager.h"
#include "camera_preview.h"

#define LOG_TAG "camera_manager"


CameraManager::~CameraManager(){
    closeSession();
}

CameraManager::CameraManager()
{
    LOGI("Creating CameraManager object\n");
}

CameraManager::CameraManager(ImageProcessor *processor)
    : m_output_processor(processor){}

void CameraManager::setProcessor(ImageProcessor *processor) {
    m_output_processor = processor;
}

void CameraManager::imageProcessor()
{
    AImage *image = nullptr;
    while(m_sessionRunning)
    {
        if(m_output_processor == nullptr)
            continue;
        media_status_t status = AImageReader_acquireLatestImage(m_reader, &image);
        if(status != AMEDIA_OK)
        {
            AImage_delete(image);
            continue;
        }

        uint8_t *data = nullptr;
        int len = 0;

        AImage_getPlaneData(image, 0, &data, &len);

        int height, width, format;
        AImage_getHeight(image, &height);
        AImage_getWidth(image, &width);
        AImage_getFormat(image, &format);

        const int size[]={len};

        *m_inputImg = cv::imdecode(cv::Mat(1, size, CV_8UC1, data), 1);

        m_output_processor -> image_callback(m_inputImg);

        AImage_delete(image);
    }
    m_sessionRunning = true;
}

void CameraManager::camera_device_on_disconnected(void *context, ACameraDevice *device) {
    LOGI("Camera(id: %s) is disconnected.\n", ACameraDevice_getId(device));
}

void CameraManager::camera_device_on_error(void *context, ACameraDevice *device, int error) {
    LOGE("Error(code: %d) on Camera(id: %s).\n", error, ACameraDevice_getId(device));
}
void CameraManager::capture_session_on_ready(void *context, ACameraCaptureSession *session) {
    LOGI("Session is ready.\n");
}

void CameraManager::capture_session_on_active(void *context, ACameraCaptureSession *session) {
    LOGI("Session is active.\n");
}

void CameraManager::openCamera()
{
    const char *selectedCameraID;
    ACameraIdList *cameraIdList = NULL;
    camera_status_t camera_status = ACAMERA_OK;

    ACameraManager *cameraManager = ACameraManager_create();
    camera_status = ACameraManager_getCameraIdList(cameraManager, &cameraIdList);

    if( camera_status != ACAMERA_OK || cameraIdList->numCameras < 1)
    {
        LOGE("No camera device detected.\n");
        return;
    }
    ACameraMetadata* metadataObj;

    LOGI("id1: %s", cameraIdList->cameraIds[0]);

    for(int i = 0;i < cameraIdList->numCameras;i++)
    {
        selectedCameraID = cameraIdList->cameraIds[i];

        ACameraManager_getCameraCharacteristics(cameraManager, selectedCameraID, &metadataObj);

        ACameraMetadata_const_entry lensInfo = { 0 };
        ACameraMetadata_getConstEntry(metadataObj, ACAMERA_LENS_FACING, &lensInfo);

        auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>(
                lensInfo.data.u8[0]);

        // Found a back-facing camera
        if (facing == ACAMERA_LENS_FACING_BACK)
            break;
    }
    ACameraMetadata_free(metadataObj);


    m_deviceStateCallbacks.onDisconnected = camera_device_on_disconnected;
    m_deviceStateCallbacks.onError = camera_device_on_error;

    camera_status = ACameraManager_openCamera(cameraManager, selectedCameraID, &m_deviceStateCallbacks, &m_cameraDevice);
    if(camera_status != ACAMERA_OK)
        LOGE("Camera device could not be opened\n");

    ACameraManager_deleteCameraIdList(cameraIdList);
    ACameraManager_delete(cameraManager);
}

void CameraManager::openSession()
{
    if(m_sessionRunning)
        return;

    m_sessionRunning = true;

    openCamera();

    camera_status_t  camera_status = ACameraDevice_createCaptureRequest(m_cameraDevice, TEMPLATE_PREVIEW,
                                                                        &m_captureRequest);
    ACaptureSessionOutputContainer_create(&m_captureSessionOutputContainer);

    if(camera_status == ACAMERA_OK)
        LOGI("capture request created successfully\n");

    media_status_t status = AImageReader_new(image_width, image_height, AIMAGE_FORMAT_JPEG, 5, &m_reader);

    AImageReader_getWindow(m_reader, &m_window);
    if(m_cameraOutputTarget != nullptr)
    {
        ACameraOutputTarget_free(m_cameraOutputTarget);
        m_cameraOutputTarget = nullptr;
    }
    ACameraOutputTarget_create(m_window, &m_cameraOutputTarget);
    ACaptureRequest_addTarget(m_captureRequest, m_cameraOutputTarget);

    if(m_sessionOutput != nullptr)
    {
        ACaptureSessionOutputContainer_remove(m_captureSessionOutputContainer, m_sessionOutput);
        ACaptureSessionOutput_free(m_sessionOutput);
        m_sessionOutput = nullptr;
    }

    ACaptureSessionOutput_create(m_window, &m_sessionOutput);
    ACaptureSessionOutputContainer_add(m_captureSessionOutputContainer, m_sessionOutput);


    m_captureSessionStateCallbacks.onReady = capture_session_on_ready;
    m_captureSessionStateCallbacks.onActive = capture_session_on_active;

    ACameraDevice_createCaptureSession(m_cameraDevice, m_captureSessionOutputContainer, &m_captureSessionStateCallbacks, &m_captureSession);

    ACameraCaptureSession_setRepeatingRequest(m_captureSession, nullptr, 1, &m_captureRequest, NULL);

    m_inputImg = new cv::Mat(image_height, image_width, CV_8UC3);

    std::thread processingWorker(&CameraManager::imageProcessor, this);
    processingWorker.detach();
}

void CameraManager::closeSession()
{
    if(!m_sessionRunning)
        return;
    m_sessionRunning = false;
    //shutting down processing thread
    while(!m_sessionRunning)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_sessionRunning = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(200)); //wait for everything to sync


    if (m_captureRequest != nullptr) {
        ACaptureRequest_free(m_captureRequest);
        m_captureRequest = nullptr;
    }

    if (m_cameraOutputTarget != nullptr) {
        ACameraOutputTarget_free(m_cameraOutputTarget);
        m_cameraOutputTarget = nullptr;
    }

    camera_status_t camera_status = ACameraDevice_close(m_cameraDevice);
    if (camera_status != ACAMERA_OK) {
        LOGE("Failed to close CameraDevice.\n");
    }

    if (m_captureSessionOutputContainer != nullptr) {
        ACaptureSessionOutputContainer_free(m_captureSessionOutputContainer);
        m_captureSessionOutputContainer = nullptr;
    }

    if(m_reader != nullptr)
    {
        AImageReader_delete(m_reader);
        m_reader = nullptr;
    }

    m_inputImg->release();

    LOGI("session closed\n");
}