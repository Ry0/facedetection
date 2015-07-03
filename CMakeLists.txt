cmake_minimum_required(VERSION 2.8)
project( facedetect )
find_package( OpenCV REQUIRED )


#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../)
ADD_DEFINITIONS(
  -g
  -O0
  -Wall
  -std=c++11
  )

add_executable( 
  facedetect facedetect.cpp 
  )
target_link_libraries( facedetect ${OpenCV_LIBS} )
