project(gldm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(gldm src/gldm.cpp "src/main.cpp" "src/extractor.cpp")

target_include_directories(gldm PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(gldm PRIVATE ${OpenCV_LIBS})
