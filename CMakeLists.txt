cmake_minimum_required(VERSION 3.14)
project(21_Garbage_collector_C__)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(21_Garbage_collector_C__
    gc_details.h
    gc_iterator.h
    gc_pointer.h
    LeakTester.h
    main.cpp)
