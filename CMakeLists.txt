cmake_minimum_required(VERSION 3.16)
project(cloudflare C)

set(CMAKE_C_STANDARD 99)

add_executable(icmp main.c icmp.c icmp.h)