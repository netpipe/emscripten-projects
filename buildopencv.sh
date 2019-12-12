##!/bin/bash
source /home/Dev/libs/Scripting/emsdk/emsdk_env.sh


#mine was not compiling yet

emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -DCPU_BASELINE='' -DCPU_DISPATCH='' -DCV_TRACE=OFF -DBUILD_SHARED_LIBS=OFF -DWITH_1394=OFF -DWITH_ADE=OFF -DWITH_VTK=OFF -DWITH_EIGEN=OFF -DWITH_FFMPEG=ON -DWITH_GSTREAMER=ON -DWITH_GTK=ON -DWITH_GTK_2_X=ON -DWITH_IPP=OFF -DWITH_JASPER=OFF -DWITH_JPEG=ON -DWITH_WEBP=OFF -DWITH_OPENEXR=OFF -DWITH_OPENGL=OFF -DWITH_OPENVX=OFF -DWITH_OPENNI=OFF -DWITH_OPENNI2=OFF -DWITH_PNG=ON -DWITH_TBB=OFF -DWITH_PTHREADS_PF=OFF -DWITH_TIFF=ON -DWITH_V4L=ON -DWITH_OPENCL=ON -DWITH_OPENCL_SVM=ON -DWITH_OPENCLAMDFFT=ON -DWITH_OPENCLAMDBLAS=ON -DWITH_GPHOTO2=ON -DWITH_LAPACK=ON -DWITH_ITT=ON -DWITH_QUIRC=ON -DBUILD_ZLIB=ON -DBUILD_opencv_apps=ON -DBUILD_opencv_calib3d=ON -DBUILD_opencv_dnn=ON -DBUILD_opencv_features2d=ON -DBUILD_opencv_flann=ON -DBUILD_opencv_gapi=ON -DBUILD_opencv_ml=ON -DBUILD_opencv_photo=ON -DBUILD_opencv_imgcodecs=ON -DBUILD_opencv_shape=ON -DBUILD_opencv_videoio=ON -DBUILD_opencv_videostab=ON -DBUILD_opencv_highgui=ON -DBUILD_opencv_superres=ON -DBUILD_opencv_stitching=ON -DBUILD_opencv_java=OFF -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_opencv_js=ON -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF -DBUILD_opencv_python_bindings_generator=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PACKAGE=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF
