cmake_minimum_required(VERSION 3.6)

project(websocketscpp)

include(websocketscpp/make/configure.cmake)

include(websocketscpp/make/base64.cmake)
include(websocketscpp/make/sha1.cmake)

include(websocketscpp/make/websocketscpp.cmake)
include(websocketscpp/make/websocketscpp_sample.cmake)
