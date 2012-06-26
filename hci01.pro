
TEMPLATE = app

win32 {
INCLUDEPATH += E:/opencv/install/include/opencv\
               E:/opencv/install/include/opencv2\
               E:/opencv/install/include/
LIBS += E:/opencv/install/lib/libopencv_calib3d231.dll.a\
        E:/opencv/install/lib/libopencv_contrib231.dll.a\
        E:/opencv/install/lib/libopencv_core231.dll.a\
        E:/opencv/install/lib/libopencv_features2d231.dll.a\
        E:/opencv/install/lib/libopencv_flann231.dll.a\
        E:/opencv/install/lib/libopencv_gpu231.dll.a\
        E:/opencv/install/lib/libopencv_highgui231.dll.a\
        E:/opencv/install/lib/libopencv_imgproc231.dll.a\
        E:/opencv/install/lib/libopencv_legacy231.dll.a\
        E:/opencv/install/lib/libopencv_ml231.dll.a\
        E:/opencv/install/lib/libopencv_objdetect231.dll.a\
        E:/opencv/install/lib/libopencv_video231.dll.a
}

HEADERS +=

SOURCES += main.cpp
