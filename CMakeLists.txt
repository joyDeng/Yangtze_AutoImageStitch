cmake_minimum_required (VERSION 2.8.3)
project(Yangtze)

add_subdirectory(ext)

if (WIN32)
  SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
endif()

add_definitions(-DDATA_DIR=R\"\(${PROJECT_SOURCE_DIR}/data\)\")

set(EIGEN_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/eigen)

include_directories(
  # Yangtze include files
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  # Eigen linear algebra library
  ${EIGEN_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/ext
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/stb
)

set(ext_stb_srcs
    ext/stb/stb_image_write.h
    ext/stb/stb_image.h
)

set(common_srcs
    include/array3D.h
    include/exceptions.h
    src/floatimage.cpp
    include/floatimage.h
    include/utils.h
    include/common.h
)

# Compile common library
add_library(common_lib STATIC
    ${common_srcs}
    ${ext_stb_srcs}
)

SOURCE_GROUP("ext\\stb" FILES ${ext_stb_srcs})

# The following lines build the main executable.
add_executable(yangtze

  # Header files
  include/stiches.hpp
  include/PanoImage.hpp

  # Source code files
  src/main.cpp
  src/stiches.cpp
  src/PanoImage.cpp
)


target_link_libraries(yangtze common_lib)

# Create the output data directory
add_custom_command(TARGET yangtze POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/data/output") # a6

