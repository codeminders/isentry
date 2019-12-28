## About ##

Use iSentry to instantly turn your computer with web camera into a
Internet-connected motion-sensing security system. Just launch
iSentry, point your camera to the region you want to monitor, and
prepare to receive a video or photo should any motion activity is
detected.

This is open source version of popular Mac and iPhone
application. This open source project is just a testbed for debugging
algorithms. If you want ready-to-use application with UI and support
please proceed to:

http://www.codeminders.com/isentry/

The motion detection algorithm discussed in the following paper (in
Russian):
    
http://www.crocodile.org/lord/motion_detection.pdf

## Build ##

On Ubuntu you need to install the following dependencies:

  1. libopencv-dev
  2. libconfig++-dev
  3. libopencv-highgui-dev
  4. cmake
  
To build:

`cmake CMakeLists.txt; make`

## Usage ##

Start by editing `isenty.cfg` file. The application could work in
headless mode or show a simple GUI. This and other options are
controlled by config.

Plot shows:

  1. Motion (instant) - **grey**
  2. Motion (smoothed/integrated) - **blue**
  3. Detection threshold - **green**

## AUTHORS ##

Developed by Codeminders http://www.codeminders.com/
