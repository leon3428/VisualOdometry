#include <jni.h>
#include <string>
#include <opencv2/core/mat.hpp>
#include "camera_manager.h"
#include "config.h"
#include "utils.h"
#include <android/native_window_jni.h>
#include "native-lib.h"
#include "odometry.h"

#define LOG_TAG "jni"

extern "C" JNIEXPORT void JNICALL
Java_com_example_visualodometry_MainActivity_setPathSurface(JNIEnv *env, jclass clazz, jobject surface) {
    ANativeWindow *surfaceWindow = ANativeWindow_fromSurface(env, surface);
    if(odometry == nullptr)
        odometry = new Odometry();
    odometry -> m_path_preview.setImageType(360, 360, IMAGE_RGB);
    odometry -> m_path_preview.setWindow(surfaceWindow);

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_visualodometry_MainActivity_setSurface(JNIEnv *env, jclass clazz, jobject surface) {
    LOGI("setting up camera preview\n");
    ANativeWindow *surfaceWindow = ANativeWindow_fromSurface(env, surface);
    if(preview == nullptr)
        preview = new CameraPreview();
        preview -> setImageType(image_width, image_height, IMAGE_GRAY);
    preview->setWindow(surfaceWindow);

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_visualodometry_MainActivity_nativeOnCreate(JNIEnv *env, jclass clazz, jstring params) {
    if(preview == nullptr)
        preview = new CameraPreview();
        preview -> setImageType(image_width, image_height, IMAGE_GRAY);
    if(odometry == nullptr)
        odometry = new Odometry();
    if(manager == nullptr)
        manager = new CameraManager(odometry);
    manager -> openSession();

    const char *cstr = env->GetStringUTFChars(params, NULL);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(params, cstr);

    odometry -> load_params(str);
    odometry -> setPreview(preview);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_visualodometry_MainActivity_nativeOnDestroy(JNIEnv *env, jclass clazz) {
    manager -> closeSession();
    delete manager;
    manager = nullptr;
    delete odometry;
    odometry = nullptr;
    delete preview;
    preview = nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_visualodometry_MainActivity_nativeOnPause(JNIEnv *env, jclass clazz) {
    manager -> closeSession();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_visualodometry_MainActivity_nativeOnResume(JNIEnv *env, jclass clazz) {
    manager -> openSession();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_visualodometry_MainActivity_undistortCheckbox(JNIEnv *env, jclass clazz, jboolean flag) {
    if(odometry != nullptr)
        odometry -> undistort = flag;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_visualodometry_MainActivity_clearPath(JNIEnv *env, jclass clazz) {
    odometry -> clearPath();
}