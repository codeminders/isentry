CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(iSentry)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall")

ADD_EXECUTABLE(iSentry iSentry.cxx iSentryConfig.cxx
MotionDetector.cxx MotionEstimator.cxx FrameProcessor.cxx
MediaRecorder.cxx ImageRecorder.cxx VideoRecorder.cxx  cvplot.cxx)

TARGET_LINK_LIBRARIES(iSentry ${OpenCV_LIBS} m config++ ${EXTERNAL_LIBS})

