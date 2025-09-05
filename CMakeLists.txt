cmake_minimum_required(VERSION 3.10)
project(BetterAltTab_Unnamed10110 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurar para generar ejecutable en la raíz
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Configurar para Release por defecto y optimizaciones
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")

# Deshabilitar generación de archivos adicionales
set(CMAKE_SUPPRESS_REGENERATION TRUE)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# Agregar recurso de ícono
set(APP_ICON_RESOURCE_WINDOWS appicon.rc)

add_executable(BetterAltTab_Unnamed10110_Optimized WIN32 main.cpp ${APP_ICON_RESOURCE_WINDOWS})

set_target_properties(BetterAltTab_Unnamed10110_Optimized PROPERTIES 
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "BetterAltTab"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_link_libraries(BetterAltTab_Unnamed10110_Optimized PRIVATE dwmapi user32 gdi32 shell32 ole32 oleaut32 comdlg32 comctl32 shlwapi)