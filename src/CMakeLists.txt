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

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(SYSTEMD QUIET systemd)
    if (SYSTEMD_FOUND)
        message(STATUS "Found systemd version ${SYSTEMD_VERSION}. Enabling systemd journal support.")
        add_definitions("-DWITH_SYSTEMD")
        target_link_libraries(bidekkida systemd)
    else(SYSTEMD_FOUND)
        message(STATUS "Can not find systemd. Disabling systemd journal support.")
    endif(SYSTEMD_FOUND)
else(PKG_CONFIG_FOUND)
    message(STATUS "Can not find pkg-config. Disabling systemd journal support.")
endif(PKG_CONFIG_FOUND)

install(TARGETS bidekkida DESTINATION ${CMAKE_INSTALL_BINDIR})
