cmake_minimum_required(VERSION 3.10)

project(WORDPOPUP)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTOUIC ON) 
set(CMAKE_AUTORCC ON) 

set(Qt6_DIR "C:/Qt/6.11.0/mingw_64/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/mingw_64")



# Configurar CURL para MinGW
set(CURL_INCLUDE_DIR "C:/curl-8.19.0_6-win64-mingw/include")
set(CURL_LIBRARY "C:/curl-8.19.0_6-win64-mingw/lib/libcurl.dll.a")


FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)


find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(DicionarioPopUp main.cpp)

target_include_directories(DicionarioPopUp PRIVATE ${CURL_INCLUDE_DIR})

target_link_libraries(DicionarioPopUp 
    PRIVATE 
        ${CURL_LIBRARY}
        Qt6::Widgets 
        nlohmann_json::nlohmann_json
        user32 
        gdi32
        ws2_32
)