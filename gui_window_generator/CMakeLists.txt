
# 3.15 fixes "overriding '/W3' with '/W4'" warning for MSVC
cmake_minimum_required(VERSION 3.15)

project(gui_window_generator)

add_executable(gui_window_generator "gui_window_generator.cpp")
set_target_properties(gui_window_generator PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)
target_compile_definitions(gui_window_generator PRIVATE UNICODE _UNICODE)

if(MSVC)
	target_compile_options(gui_window_generator PRIVATE "/W4;/WX;$<$<CONFIG:RELEASE>:/O2>")
else()
	target_compile_options(gui_window_generator PRIVATE "-Wall;-Wextra;-Werror;$<$<CONFIG:RELEASE>:-O3>")
	target_link_options(gui_window_generator PRIVATE "-municode")
endif()
