add_executable(task06 "task06.cpp")

find_package(OpenCV REQUIRED)

target_link_libraries(task06 PRIVATE opencv_core opencv_highgui semcv)