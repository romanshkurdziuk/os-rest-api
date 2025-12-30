cmake_minimum_required(VERSION 3.14)

project(TodoApi VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /EHsc /bigobj)
    add_definitions(-D_WIN32_WINNT=0x0601) # Windows 7+ API
else()
    add_compile_options(-Wall -Wextra)
endif()

# Глобальный флаг: используем Asio без Boost
add_definitions(-DASIO_STANDALONE)

include(FetchContent)

# ---------------------------------------------------------
# 1. Скачиваем ASIO (но не настраиваем его автоматически)
# ---------------------------------------------------------
FetchContent_Declare(
  asio
  URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Просто распаковываем файлы, чтобы получить пути
FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
  FetchContent_Populate(asio)
endif()

# ---------------------------------------------------------
# 2. Скачиваем CROW (и тоже не запускаем его CMake)
# ---------------------------------------------------------
FetchContent_Declare(
  crow
  URL https://github.com/CrowCpp/Crow/archive/refs/tags/v1.2.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(crow)
if(NOT crow_POPULATED)
  FetchContent_Populate(crow)
endif()

# ---------------------------------------------------------
# 3. Собираем приложение
# ---------------------------------------------------------
add_executable(todo_server 
    src/main.cpp 
    src/model.hpp
)

# 4. ВРУЧНУЮ указываем пути к заголовкам (Include Directories)
# Это самый надежный способ, так как мы точно знаем, где лежат файлы
target_include_directories(todo_server PRIVATE
    ${asio_SOURCE_DIR}/asio/include  # Путь к заголовкам Asio
    ${crow_SOURCE_DIR}/include       # Путь к заголовкам Crow
)

# Для Windows нужно подключить системную библиотеку сокетов
if(WIN32)
    target_link_libraries(todo_server PRIVATE ws2_32 mswsock)
endif()