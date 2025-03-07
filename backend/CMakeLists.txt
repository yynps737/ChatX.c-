cmake_minimum_required(VERSION 3.10)
project(ChatX VERSION 1.0.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

# 查找必要的包
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

# 添加 nlohmann_json
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
    ${SQLite3_INCLUDE_DIRS}
)

# 添加 ChatX 源文件
file(GLOB_RECURSE CHATX_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${CHATX_SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME}
    PRIVATE ${SQLite3_LIBRARIES}
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE Threads::Threads
)
