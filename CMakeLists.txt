CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(iSentry)

include_directories(/opt/local/include /opt/local/include/opencv
/opt/local/include/opencv2 /opt/local/include/opencv2/highgui/ /usr/include/opencv /usr/include/opencv2)
link_directories (/opt/local/lib)

set(CMAKE_CXX_FLAGS "-g -Wall")

ADD_EXECUTABLE(iSentry iSentry.cxx iSentryConfig.cxx
MotionDetector.cxx MotionEstimator.cxx FrameProcessor.cxx
MediaRecorder.cxx ImageRecorder.cxx VideoRecorder.cxx  cvplot.cxx)

TARGET_LINK_LIBRARIES(iSentry opencv_core opencv_imgproc
opencv_calib3d opencv_features2d opencv_highgui m config++ ${EXTERNAL_LIBS})


