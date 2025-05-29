add_executable(task05 "task05.cpp")
find_package(OpenCV REQUIRED)

target_link_libraries(task05 PRIVATE opencv_core opencv_highgui semcv)