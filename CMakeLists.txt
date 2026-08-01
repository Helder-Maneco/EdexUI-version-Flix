cmake_minimum_required(VERSION 3.10)
project(edex-cli VERSION 0.1.0 LANGUAGES CXX)

#  Configurações Globais
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ====================================
#  Biblioteca Core
# ====================================
add_library(core STATIC
    src/core/cpu_stats.cpp
    src/core/disk_stats.cpp
    src/core/mem_stats.cpp
    src/core/net_stats.cpp
    src/core/stats_collector.cpp
    src/core/temp_stats.cpp
    src/core/system_info.cpp
)

target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(core PRIVATE pthread rt)

# ====================================
#  Biblioteca UI (NCurses)
# ====================================
find_package(PkgConfig REQUIRED)
pkg_check_modules(NCURSES REQUIRED ncurses)

if(NOT NCURSES_FOUND)
    message(FATAL_ERROR "Ncurses não encontrado!")
endif()

message(STATUS "Ncurses found: ${NCURSES_INCLUDE_DIRS}")

add_library(ui STATIC
    src/ui/renderer.cpp
)

target_include_directories(ui PUBLIC 
    ${PROJECT_SOURCE_DIR}/src
    ${NCURSES_INCLUDE_DIRS}
)

target_link_libraries(ui PRIVATE core pthread ${NCURSES_LIBRARIES})

# ====================================
#  Biblioteca IPC (Socket Server)
# ====================================
add_library(ipc STATIC
    src/ipc/socket_server.cpp
)

target_include_directories(ipc PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ipc PRIVATE core pthread)

# ====================================
#  Executável Principal
# ====================================
add_executable(edex-cli
    src/main.cpp
)

target_link_libraries(edex-cli PRIVATE ui ipc core ${NCURSES_LIBRARIES})

# ====================================
#  Mensagem Final
# ====================================
message(STATUS "")
message(STATUS " Edex-CLI Configuration Complete!")
message(STATUS "   Version: ${PROJECT_VERSION}")
message(STATUS "   Build Type: Release")
message(STATUS "   UI Renderer: NCurses")
message(STATUS "")
