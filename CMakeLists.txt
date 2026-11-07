cmake_minimum_required(VERSION 3.31.6)
project(MasterPasswordsCppCLI LANGUAGES CXX)

# Настройки стандарта C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)

# Рекурсивный поиск исходных файлов и заголовков
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

# Исключаем main.cc из общего списка, так как он задает точку входа исполняемого
# файла
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc")

# Собираем единый список файлов для форматирования (включая main.cc)
set(ALL_SOURCE_FILES ${SOURCES} ${HEADERS}
                     "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc")

# Поиск clang-format в системе (работает из коробки на Debian)
find_program(CLANG_FORMAT_EXE NAMES clang-format)

# Альтернативный поиск clang-format на Windows внутри папки Visual Studio
if(NOT CLANG_FORMAT_EXE AND WIN32)
  file(
    GLOB
    VS_CLANG_FORMAT_EDITIONS
    "C:/Program Files/Microsoft Visual Studio/*/*/VC/Tools/Llvm/x64/bin/clang-format.exe"
  )
  if(VS_CLANG_FORMAT_EDITIONS)
    list(GET VS_CLANG_FORMAT_EDITIONS 0 CLANG_FORMAT_EXE)
  endif()
endif()

# Создание цели форматирования code style
if(CLANG_FORMAT_EXE)
  message(STATUS "Clang-format tool found: ${CLANG_FORMAT_EXE}")

  # Кроссплатформенный и безопасный запуск форматирования через CMake-скрипт
  add_custom_target(
    format
    COMMAND
      ${CMAKE_COMMAND} -E __run_co_compile --clang-format=${CLANG_FORMAT_EXE}
      --style=file -- ${ALL_SOURCE_FILES}
    COMMENT "Formatting code with Clang-Format...")
else()
  message(WARNING "clang-format not found! Target 'format' is disabled.")
endif()

# Создание исполняемого файла
add_executable(MasterPasswordsCppCLI src/main.cc)

# Библиотеки
target_link_libraries(MasterPasswordsCppCLI PRIVATE OpenSSL::Crypto)

# Привязка всех зависимых файлов к цели
target_sources(MasterPasswordsCppCLI PRIVATE ${SOURCES} ${HEADERS})

# Позволяет писать #include "src/folder/file.h" из любой точки проекта
target_include_directories(MasterPasswordsCppCLI
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
