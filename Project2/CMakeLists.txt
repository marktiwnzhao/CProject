cmake_minimum_required(VERSION 3.20)
project(Project2 C)

set(CMAKE_C_STANDARD 11)

add_executable(PlaneFight PlaneFight.c)
target_link_libraries(PlaneFight mingw32)
target_link_libraries(PlaneFight SDL2main)
target_link_libraries(PlaneFight SDL2)
target_link_libraries(PlaneFight SDL2_image)
target_link_libraries(PlaneFight SDL2_ttf)
target_link_libraries(PlaneFight SDL2_mixer)
