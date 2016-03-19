cmake_minimum_required(VERSION 2.8)

project(bmsbel)
include_directories("include")
file(GLOB SRC
  "bmsbel/*.cpp"
)
file(GLOB HEADER
  "include/bmsbel/*.h"
)
#add_library(bmsbel STATIC ${SRC} ${HEADER})
add_definitions(-std=c++11)
add_library(bmsbel SHARED
  "bmsbel/bms_bar.cpp"
  "bmsbel/bms_bms.cpp"
  "bmsbel/bms_buffer.cpp"
  "bmsbel/bms_channel.cpp"
  "bmsbel/bms_exception.cpp"
  "bmsbel/bms_header_table.cpp"
  "bmsbel/bms_note.cpp"
  "bmsbel/bms_parser.cpp"
  "bmsbel/bms_regist_array.cpp"
  "bmsbel/bms_text_file_reader.cpp"
  "bmsbel/bms_time.cpp"
  "bmsbel/bms_util.cpp"
  "bmsbel/bms_word.cpp"
)

#set (CMAKE_CXX_FLAGS "-liconv")
#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
#set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib")
#target_link_libraries("iconv")
