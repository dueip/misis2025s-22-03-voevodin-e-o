add_executable(task03 "task03.cpp")

find_package(OpenCV REQUIRED)

target_link_libraries(task03 PRIVATE opencv_core opencv_highgui semcv)