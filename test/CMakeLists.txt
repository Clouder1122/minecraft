cmake_minimum_required(VERSION 3.20)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_program(GLSLC_EXE glslc HINTS ${Vulkan_GLSLC_EXECUTABLE} ENV VULKAN_SDK PATH_SUFFIXES Bin bin)

if(NOT GLSLC_EXE)
    message(WARNING "glslc not found. Shader compilation will be skipped.")
endif()

include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${Vulkan_INCLUDE_DIRS}
    src
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Vulkan::Vulkan
    glfw
    glm::glm
)

file(GLOB SHADERS "shaders/*.vert" "shaders/*.frag")
set(SPV_SHADERS "")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

if (GLSLC_EXE)
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        set(SPV_FILE ${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv)
        
        add_custom_command(
            OUTPUT ${SPV_FILE}
            COMMAND ${GLSLC_EXE} ${SHADER} -o ${SPV_FILE}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader: ${SHADER_NAME}"
        )
        
        list(APPEND SPV_SHADERS ${SPV_FILE})
    endforeach()
endif()

add_custom_target(CompileShaders ALL DEPENDS ${SPV_SHADERS})
add_dependencies(${PROJECT_NAME} CompileShaders)
