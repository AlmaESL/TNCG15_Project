cmake_minimum_required(VERSION 3.16)
project(MyRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8 # stable release
)
FetchContent_MakeAvailable(glfw)

# GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

# glm (optional math lib)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(glm)

# Your executable
add_executable(MyRenderer
    src/main.cpp
)

target_include_directories(MyRenderer PRIVATE include)

target_link_libraries(MyRenderer
    PRIVATE
        glfw
        glad
        glm
)

# macOS frameworks (skip if Windows/Linux)
if(APPLE)
    target_link_libraries(MyRenderer PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreFoundation" "-framework CoreVideo")
endif()
