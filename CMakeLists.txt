#CMakeLists
cmake_minimum_required(VERSION 3.13.0...3.20)
project(MyRenderer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

function(enable_warnings target)
target_compile_options(${target} PUBLIC 
$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
$<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra>
)
endfunction()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
include_directories(include/glm)


# Collect all source files in the 'src' and 'include' directories
set(HEADER_FILES
	"include/glad/glad.h"
	"include/KHR/khrplatform.h"
	"include/GLFW/glfw3.h"
	"include/GLFW/glfw3native.h"
	"include/computeNormals.h"

)

set(SOURCE_FILES
	main.cpp
	glad.c
)

link_directories(lib)

add_executable(MyRenderer ${SOURCE_FILES} ${HEADER_FILES})
# target_include_directories(Raytracing PUBLIC include ext/stb)
target_link_libraries(MyRenderer 
	glfw3.dll
)


enable_warnings(MyRenderer)

# Add the include directory for headers
# include_directories(${PROJECT_SOURCE_DIR}/include)

# /////////////////////////////////////////////////////////////
# Don't mind this I left this here to show that there are multiple ways of linking to an external library but they don't work as well with Visual Studio. 

# set(glm_DIR ${PROJECT_SOURCE_DIR}/ext/glm)
# find_package(glm REQUIRED)
# add_subdirectory(${PROJECT_SOURCE_DIR}/ext/glm)
# target_link_libraries(Raytracing PUBLIC glm::glm)

# /////////////////////////////////////////////////////////////

# Adds the location of the external libraries and main project to Visual studio, because it forgot where it was, stupid fucking program. This is the easiest way to do this because glm is a header library and cannot be used with target_link_libraries
# If you want to add more than one external library it is recommended to remove /glm from the line below.
# To access glm you need to include your header file glm.hpp. If you have removed /glm like suggested above, accessing glm with a glm/ prefix is needed. ex #include "glm/glm.hpp".
target_include_directories(MyRenderer PUBLIC ${PROJECT_SOURCE_DIR}/ext/glm ${PROJECT_SOURCE_DIR})


# Good luck!