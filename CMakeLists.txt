cmake_minimum_required(VERSION 3.10)

project(MarioGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB_RECURSE SOURCES 
    "*.cpp"
    "engine/*.cpp"
    "game/*.cpp"
    "systems/*.cpp"
)

# Header files
file(GLOB_RECURSE HEADERS 
    "*.h"
    "engine/*.h"
    "game/*.h"
    "systems/*.h"
)

# DirectX SDK NuGet package paths
set(DXSDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/packages/Microsoft.DXSDK.D3DX.9.29.952.8/build/native")
set(DXSDK_INCLUDE_DIR "${DXSDK_DIR}/include")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DXSDK_LIB_DIR "${DXSDK_DIR}/release/lib/x64")
else()
    set(DXSDK_LIB_DIR "${DXSDK_DIR}/release/lib/x86")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DXSDK_INCLUDE_DIR}
)

# Create executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    d3d10.lib
    dxguid.lib
    dinput8.lib
    "${DXSDK_LIB_DIR}/d3dx10.lib"
)

# Set common preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    _UNICODE 
    UNICODE
)
