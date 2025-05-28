add_executable(task02 "task02.cpp")

find_package(OpenCV REQUIRED)

target_link_libraries(task02 PRIVATE opencv_core opencv_highgui semcv)