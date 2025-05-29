add_executable(task04-01 "task04-01.cpp")
add_executable(task04-02 "task04-02.cpp")
add_executable(task04-03 "task04-03.cpp")
find_package(OpenCV REQUIRED)

target_link_libraries(task04-01 PRIVATE opencv_core opencv_highgui semcv)
target_link_libraries(task04-02 PRIVATE opencv_core opencv_highgui semcv)
target_link_libraries(task04-03 PRIVATE opencv_core opencv_highgui semcv)