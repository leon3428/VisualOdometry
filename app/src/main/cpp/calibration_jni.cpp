//
// Created by leon3428 on 3/21/21.
//

#include "calibration_jni.h"
#define LOG_TAG "calibartion jni"
#include <jni.h>
#include <opencv2/core/mat.hpp>
#include "camera_manager.h"
#include "camera_preview.h"
#include "utils.h"
#include <android/native_window_jni.h>
#include "config.h"
#include "camera_calibration.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_example_visualodometry_CalibrationActivity_nativeOnPause(JNIEnv *env, jclass clazz) {
    calib_manager -> closeSession();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_visualodometry_CalibrationActivity_nativeOnResume(JNIEnv *env, jclass clazz) {
    calib_manager -> openSession();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_visualodometry_CalibrationActivity_nativeOnCreate(JNIEnv *env, jclass clazz) {
    if(calib_preview == nullptr)
        calib_preview = new CameraPreview();
    if(calibration == nullptr)
        calibration = new CameraCalibration();
    if(calib_manager == nullptr)
        calib_manager = new CameraManager(calibration);

    calibration -> setPreview(calib_preview);
    calib_manager -> openSession();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_visualodometry_CalibrationActivity_nativeOnDestroy(JNIEnv *env, jclass clazz) {
    delete calib_preview;
    delete calib_manager;
    delete calibration;
    calib_preview = nullptr;
    calib_preview = nullptr;
    calibration = nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_visualodometry_CalibrationActivity_setSurface(JNIEnv *env, jclass clazz, jobject surface) {
    LOGI("setting up camera preview\n");
    ANativeWindow *surfaceWindow = ANativeWindow_fromSurface(env, surface);
    if(calib_preview == nullptr)
        calib_preview = new CameraPreview();
    calib_preview->setWindow(surfaceWindow);
    calib_preview->setImageType(image_width, image_height, IMAGE_RGB);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_visualodometry_CalibrationActivity_getImageNumber(JNIEnv *env, jclass clazz) {
    int n = calibration -> imagePointsSize();
    return n;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_visualodometry_CalibrationActivity_nativeCalibrate(JNIEnv *env, jclass clazz) {
    std::string ret = calibration -> calibrate();
    LOGI("%s\n", ret.c_str());
    return env->NewStringUTF(ret.c_str());
}