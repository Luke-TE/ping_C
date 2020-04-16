cmake_minimum_required(VERSION 3.16)
project(cloudflare C)

set(CMAKE_C_STANDARD 99)

add_executable(ping main.c icmp.c icmp.h lib/checksum.h lib/checksum.c)