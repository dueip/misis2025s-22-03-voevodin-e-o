add_executable(task07 "task07.cpp")

find_package(OpenCV REQUIRED)

target_link_libraries(task07 PRIVATE opencv_core opencv_highgui semcv)