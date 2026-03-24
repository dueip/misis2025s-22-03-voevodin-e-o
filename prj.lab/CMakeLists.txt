function(install_lab target lab_number)
    install(TARGETS ${target} DESTINATION lab${lab_number})

    install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/ DESTINATION lab${lab_number} CONFIGURATIONS Debug FILES_MATCHING PATTERN "*.dll*")
    install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/ DESTINATION lab${lab_number} CONFIGURATIONS Release FILES_MATCHING PATTERN "*.dll*")
    install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}/ DESTINATION lab${lab_number} CONFIGURATIONS RelWithDebInfo FILES_MATCHING PATTERN "*.dll*")
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md DESTINATION lab${lab_number})
endfunction()

add_subdirectory(semcv)
add_subdirectory(lab01)
add_subdirectory(lab02)
add_subdirectory(lab03)
add_subdirectory(lab04)
add_subdirectory(lab05)
add_subdirectory(lab06)
add_subdirectory(lab07)

find_package(OpenCV REQUIRED)