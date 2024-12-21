# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/szymon-gogulski/Desktop/ENGINEERING_THESIS/record_samples/build/_deps/picotool-src"
  "/home/szymon-gogulski/Desktop/ENGINEERING_THESIS/record_samples/build/_deps/picotool-build"
  "/home/szymon-gogulski/Desktop/ENGINEERING_THESIS/record_samples/build/_deps"
  "/home/szymon-gogulski/Desktop/ENGINEERING_THESIS/record_samples/build/picotool/tmp"
  "/home/szymon-gogulski/Desktop/ENGINEERING_THESIS/record_samples/build/picotool/src/picotoolBuild-stamp"
  "/home/szymon-gogulski/Desktop/ENGINEERING_THESIS/record_samples/build/picotool/src"
  "/home/szymon-gogulski/Desktop/ENGINEERING_THESIS/record_samples/build/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/szymon-gogulski/Desktop/ENGINEERING_THESIS/record_samples/build/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/szymon-gogulski/Desktop/ENGINEERING_THESIS/record_samples/build/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
