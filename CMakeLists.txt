cmake_minimum_required(VERSION 3.10)

project(WORDPOPUP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTOUIC ON) 
set(CMAKE_AUTORCC ON) 
set(Qt6_DIR "C:/Qt/6.11.0/mingw_64/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(DicionarioPopUp main.cpp)


target_link_libraries(DicionarioPopUp 
    PRIVATE 
        Qt6::Widgets 
        user32 
        gdi32
)