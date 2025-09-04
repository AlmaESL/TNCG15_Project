cmake_minimum_required(VERSION 3.14)
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

function(enable_warnings target)
target_compile_options(${target} PUBLIC 
$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
$<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra>
)
endfunction()


# Your executable
#add_executable(MyRenderer
 #   src/main.cpp
 #"include/dummyHeader.h")

 file(GLOB SOURCES "src/*.cpp")
 file(GLOB HEADERS "include/*.h")

 add_executable(MyRenderer ${SOURCES} ${HEADERS})
 enable_warnings(MyRenderer)

target_include_directories(MyRenderer PUBLIC ${PROJECT_SOURCE_DIR}/ext/glm ${PROJECT_SOURCE_DIR})

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
