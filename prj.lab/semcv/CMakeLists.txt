add_library(semcv semcv.hpp semcv.cpp)
target_link_libraries(semcv opencv_core opencv_imgproc)

install(TARGETS semcv ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(FILES semcv.hpp DESTINATION include/semcv)