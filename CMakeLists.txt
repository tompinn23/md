cmake_minimum_required(VERSION 3.10)

include(CheckIncludeFile)

project(md)

#add_subdirectory(deps/uriparser-0.9.4)


check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(dlfcn.h  HAVE_DL_H)

configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)


include_directories(${CMAKE_BINARY_DIR})

add_library(fp_stdio SHARED src/fp_stdio.c src/mem.c src/utils.c)
set_target_properties(fp_stdio PROPERTIES PREFIX "")

add_executable(md src/main.c src/file.c src/mem.c src/utils.c)
target_link_libraries(md dl)
