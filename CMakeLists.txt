cmake_minimum_required(VERSION 3.10)
project(BetterAltTab_Unnamed10110 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Agregar recurso de ícono
set(APP_ICON_RESOURCE_WINDOWS appicon.rc)

add_executable(BetterAltTab_Unnamed10110_Optimized WIN32 main.cpp ${APP_ICON_RESOURCE_WINDOWS})

set_target_properties(BetterAltTab_Unnamed10110_Optimized PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(BetterAltTab_Unnamed10110_Optimized PRIVATE dwmapi user32 gdi32 shell32 ole32 oleaut32 comdlg32 comctl32 shlwapi)