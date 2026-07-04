cmake_minimum_required(VERSION 3.10)

project(MarioGame)

set(CMAKE_CXX_STANDARD 17) # Re-evaluate globs for new files
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- NuGet Package Management ---
set(NUGET_EXE "${CMAKE_CURRENT_SOURCE_DIR}/nuget.exe")
set(PACKAGES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/packages")
set(DXSDK_PACKAGE_NAME "Microsoft.DXSDK.D3DX.9.29.952.8")

# 1. Auto-download nuget.exe if missing
if(NOT EXISTS "${NUGET_EXE}")
    message(STATUS "Downloading nuget.exe...")
    file(DOWNLOAD "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe" "${NUGET_EXE}")
endif()

# 2. Auto-restore packages if missing
if(NOT EXISTS "${PACKAGES_DIR}/${DXSDK_PACKAGE_NAME}")
    message(STATUS "Restoring NuGet packages from packages.config...")
    execute_process(
        COMMAND "${NUGET_EXE}" install "${CMAKE_CURRENT_SOURCE_DIR}/packages.config" -OutputDirectory "${PACKAGES_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE NUGET_RESULT
    )
    if(NOT NUGET_RESULT EQUAL 0)
        message(FATAL_ERROR "NuGet restore failed. Please check internet connection or nuget.exe availability.")
    endif()
endif()

# 3. Setup DirectX SDK paths
set(DXSDK_DIR "${PACKAGES_DIR}/${DXSDK_PACKAGE_NAME}/build/native")
set(WEBVIEW2_PACKAGE_NAME "Microsoft.Web.WebView2.1.0.2151.40")
set(WEBVIEW2_DIR "${PACKAGES_DIR}/${WEBVIEW2_PACKAGE_NAME}/build/native")

if(NOT EXISTS "${DXSDK_DIR}")
    message(FATAL_ERROR "DirectX SDK package not found at ${DXSDK_DIR}. Check packages.config or NuGet restore.")
endif()

set(DXSDK_INCLUDE_DIR "${DXSDK_DIR}/include")
set(WEBVIEW2_INCLUDE_DIR "${WEBVIEW2_DIR}/include")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DXSDK_LIB_DIR "${DXSDK_DIR}/release/lib/x64")
    set(WEBVIEW2_LIB_DIR "${WEBVIEW2_DIR}/x64")
    set(WEBVIEW2_DLL "${WEBVIEW2_DIR}/x64/WebView2Loader.dll")
else()
    set(DXSDK_LIB_DIR "${DXSDK_DIR}/release/lib/x86")
    set(WEBVIEW2_LIB_DIR "${WEBVIEW2_DIR}/x86")
    set(WEBVIEW2_DLL "${WEBVIEW2_DIR}/x86/WebView2Loader.dll")
endif()
# --- End NuGet Package Management ---

# Source files
file(GLOB SOURCES "*.cpp")
file(GLOB_RECURSE ENGINE_SOURCES "engine/*.cpp")
file(GLOB_RECURSE GAME_SOURCES "game/*.cpp")
file(GLOB_RECURSE SYSTEMS_SOURCES "systems/*.cpp")
list(APPEND SOURCES ${ENGINE_SOURCES} ${GAME_SOURCES} ${SYSTEMS_SOURCES})

# Header files
file(GLOB HEADERS "*.h")
file(GLOB_RECURSE ENGINE_HEADERS "engine/*.h")
file(GLOB_RECURSE GAME_HEADERS "game/*.h")
file(GLOB_RECURSE SYSTEMS_HEADERS "systems/*.h")
list(APPEND HEADERS ${ENGINE_HEADERS} ${GAME_HEADERS} ${SYSTEMS_HEADERS})

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/core
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/Graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/utils
    ${DXSDK_INCLUDE_DIR}
    ${WEBVIEW2_INCLUDE_DIR}
)

# Create executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} resource.rc)

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/content"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/content")

# Copy UI to build directory at BUILD TIME (not configure time)
# This checks if ui/out exists each time you build, no need to re-run cmake configure
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/ui/out"
        -DDST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>/ui"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyUIIfExists.cmake"
    COMMENT "Copying UI output to build directory (if available)")

# Copy WebView2Loader.dll to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${WEBVIEW2_DLL}"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

# Link libraries
target_link_libraries(${PROJECT_NAME}
    d3d10.lib
    dxguid.lib
    dinput8.lib
    "${DXSDK_LIB_DIR}/d3dx10.lib"
    "${WEBVIEW2_LIB_DIR}/WebView2Loader.dll.lib"
)

# Set common preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    _UNICODE 
    UNICODE
)

# Set VS debugger working directory to project root
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()