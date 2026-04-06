# Versão mínima do CMake
cmake_minimum_required(VERSION 3.10)

# Nome do teu projeto
project(WORDPOPUP)

# Definir que queremos C++ 17 (bom para iniciantes e moderno)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Criar o executável a partir do main.cpp
add_executable(DicionarioPopUp main.cpp)

# Se usares funções do Windows, o CMake trata das ligações automáticas
if(WIN32)
    target_link_libraries(DicionarioPopUp user32 gdi32)
endif()