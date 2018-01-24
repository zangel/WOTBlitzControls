cmake_minimum_required(VERSION 2.8)
project(WOTBControlsCli)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(BOOST_ROOT "" CACHE PATH "Path to boost")

include_directories(${BOOST_ROOT}/include)

include_directories(../WOTBControlsProto)
link_directories(${BOOST_ROOT}/lib)

add_executable(WOTBControlsCli
	../WOTBControlsProto/WOTBControlsProto.hpp
	Config.hpp
	Config.cpp
	Application.hpp
	Application.cpp
	main.cpp
)

target_link_libraries(WOTBControlsCli
	boost_system
)

if(APPLE)
	target_link_libraries(WOTBControlsCli "-framework ApplicationServices")
else()

endif()
