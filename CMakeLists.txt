cmake_minimum_required(VERSION 2.8)
project("Simple Animation Engine")

add_executable(animation main.cpp
			 regression.cpp
			 animation.cpp
			 animation_engine.cpp
			 animation_text.cpp
			 animation_texture.cpp)

if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-std=c++11)
	add_definitions(-pthread)
endif()

target_link_libraries(animation /usr/src/gmock/libgmock.a)

include_directories("/usr/src/gmock/gtest/include")
