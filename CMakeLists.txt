cmake_minimum_required(VERSION 3.16)

# ==================================================
# Конфигурационный блок — все настройки здесь
# ==================================================
set(PROJECT_NAME "Task2")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Путь к исходникам
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Путь к nlohmann/json (если лежит в third_party/nlohmann_json)
set(NLOHMANN_JSON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann_json")

# Имя исполняемого файла
set(EXECUTABLE_NAME "${PROJECT_NAME}")

# Тип сборки
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}")

# Предупреждения как ошибки
set(WARNINGS_AS_ERRORS OFF)

# ==================================================
# Конец конфигурационного блока
# ==================================================

project(${PROJECT_NAME} LANGUAGES CXX)

# Автоматический сбор исходников
file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.c"
    "${SOURCE_DIR}/*.cc"
)

file(GLOB_RECURSE HEADERS
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.hpp"
)

# Подключение nlohmann/json как интерфейсной библиотеки
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE "${NLOHMANN_JSON_DIR}/single_include")
target_compile_features(nlohmann_json INTERFACE cxx_std_11)

# Создание исполняемого файла
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS})

# Привязка JSON-библиотеки
target_link_libraries(${EXECUTABLE_NAME} PRIVATE nlohmann_json)

# Настройка стандарта C++
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD ${CMAKE_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
    CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
)

# Настройка предупреждений
if(WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(${EXECUTABLE_NAME} PRIVATE /WX)
    else()
        target_compile_options(${EXECUTABLE_NAME} PRIVATE -Werror)
    endif()
endif()

# Добавление путей
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${SOURCE_DIR})

# Вывод отладочной информации
# message(STATUS "Project: ${PROJECT_NAME}")
# message(STATUS "Source dir: ${SOURCE_DIR}")
# message(STATUS "JSON lib dir: ${NLOHMANN_JSON_DIR}")
# message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
# message(STATUS "Found sources: ${SOURCES}")