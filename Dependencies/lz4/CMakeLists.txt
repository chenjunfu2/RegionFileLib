cmake_minimum_required(VERSION 3.13)
project(lz4 LANGUAGES CXX)

add_library(lz4
    STATIC
        lz4.c
        lz4file.c
        lz4frame.c
        lz4hc.c
)