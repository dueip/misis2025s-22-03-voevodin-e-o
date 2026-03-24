add_executable(task03 "task03.cpp")

find_package(OpenCV REQUIRED)

target_link_libraries(task03 PRIVATE opencv_core opencv_highgui semcv)

install_lab(task03 03)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Images DESTINATION lab03)