cmake_minimum_required(VERSION 2.8)
project(WOTBControlsSrv)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(BOOST_ROOT "" CACHE PATH "Path to boost")
set(EIGEN_ROOT "" CACHE PATH "Path to Eigen")

include_directories(${BOOST_ROOT}/include)
include_directories(${EIGEN_ROOT})

include_directories(../WOTBControlsProto)
link_directories(${BOOST_ROOT}/lib)

add_executable(WOTBControlsSrv
	../WOTBControlsProto/WOTBControlsProto.hpp
	InputEvent.hpp
	Config.hpp
	Config.cpp
	Application.hpp
	Application.cpp
	main.cpp
)

if(APPLE)
	target_link_libraries(WOTBControlsSrv
		boost_system
	)	
else()
target_link_libraries(WOTBControlsSrv
	boost_system-clang-darwin-mt-1_65_1
)
endif()
