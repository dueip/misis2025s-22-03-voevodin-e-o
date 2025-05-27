add_executable(task01-01 "task01-01.cpp")
#add_executable(task01-02 "task01-02.cpp")

find_package(OpenCV REQUIRED)

target_link_libraries(task01-01 PRIVATE opencv_core opencv_highgui semcv)
#target_link_libraries(task01-02 PRIVATE opencv_core opencv_highgui semcv)