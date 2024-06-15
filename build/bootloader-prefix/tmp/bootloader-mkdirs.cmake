# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "E:/Software/Espressif/frameworks/esp-idf-v5.2.1/components/bootloader/subproject"
  "E:/Develop/MeowCard/build/bootloader"
  "E:/Develop/MeowCard/build/bootloader-prefix"
  "E:/Develop/MeowCard/build/bootloader-prefix/tmp"
  "E:/Develop/MeowCard/build/bootloader-prefix/src/bootloader-stamp"
  "E:/Develop/MeowCard/build/bootloader-prefix/src"
  "E:/Develop/MeowCard/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/Develop/MeowCard/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/Develop/MeowCard/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
