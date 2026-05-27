cmake_minimum_required(VERSION 3.10)
project(TourAgencySystem VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(tourism_system
    src/main.cpp
    src/sqlite3.c
    src/database.cpp
    src/admin.cpp
    src/filter.cpp
    src/tour.cpp
    src/ui.cpp
    src/user.cpp
)
