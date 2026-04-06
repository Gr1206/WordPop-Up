# Versão mínima do CMake
cmake_minimum_required(VERSION 3.10)

# Nome do teu projeto
project(WORDPOPUP)

# Definir que queremos C++ 17 (bom para iniciantes e moderno)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) # Importante para Signals/Slots
set(CMAKE_AUTOUIC ON) # Se usares ficheiros .ui
set(CMAKE_AUTORCC ON) # Se usares recursos (.qrc)
set(Qt6_DIR "C:/Qt/6.11.0/mingw_64/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Criar o executável a partir do main.cpp
add_executable(DicionarioPopUp main.cpp)

# Se usares funções do Windows, o CMake trata das ligações automáticas

target_link_libraries(DicionarioPopUp 
    PRIVATE 
        Qt6::Widgets 
        user32 
        gdi32
)