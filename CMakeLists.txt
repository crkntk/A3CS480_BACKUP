cmake_minimum_required(VERSION 3.27)
project(CS480Pages)

set(CMAKE_CXX_STANDARD 17)

add_executable(CS480PAGES main.cpp
        Map.cpp
        Map.h
        LoadedPage.cpp
        LoadedPage.h
        PageReplacement.cpp
        PageReplacement.h)
