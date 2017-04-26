set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (bidekkida_SRCS
    main.cpp
    anonymizer.h
    anonymizer.cpp
)

add_executable(bidekkida ${bidekkida_SRCS})
target_link_libraries(bidekkida
    Qt5::Core
)

install(TARGETS bidekkida DESTINATION ${CMAKE_INSTALL_BINDIR})
