# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/noneya/pico/otg_to_uart/build/_deps/picotool-src"
  "/home/noneya/pico/otg_to_uart/build/_deps/picotool-build"
  "/home/noneya/pico/otg_to_uart/build/_deps"
  "/home/noneya/pico/otg_to_uart/build/picotool/tmp"
  "/home/noneya/pico/otg_to_uart/build/picotool/src/picotoolBuild-stamp"
  "/home/noneya/pico/otg_to_uart/build/picotool/src"
  "/home/noneya/pico/otg_to_uart/build/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/noneya/pico/otg_to_uart/build/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/noneya/pico/otg_to_uart/build/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
