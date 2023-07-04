cmake_minimum_required(VERSION 3.24)
project(raylib_experiments)

set(CMAKE_DEBUG_FIND_MODE ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# raylib
#

## todo: il path di raylib andrebbe messo in una variabile di ambiente
#list(APPEND CMAKE_PREFIX_PATH "/extra/gioprj.on_lnx.cpp.checkout/cpp.raylib/raylib.install.static")
## bisogna specificare il path dove si trova raylib-config per qualche motivo find_package non lo trova in automatico
#set(raylib_DIR /extra/gioprj.on_lnx.cpp.checkout/cpp.raylib/raylib.install.static/lib64/cmake/raylib)
#find_package(raylib REQUIRED CONFIG)

#
# il seguito per installare raylib da github
#

set(RAYLIB_VERSION 4.5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED

if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)

    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )

    FetchContent_GetProperties(raylib)

    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        set(SUPPORT_IMAGE_GENERATION ON)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif ()

endif ()

function(add_git_dependency libName gitURL gitTag)
    FetchContent_Declare(${libName}
            GIT_REPOSITORY ${gitURL}
            GIT_TAG ${gitTag}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            )
    FetchContent_MakeAvailable(${libName})
    target_compile_options(${libName} PRIVATE "-w")
endfunction()


#
#
#

add_executable(texture_panning texture_panning.c)
target_link_libraries(texture_panning PRIVATE raylib)

add_executable(texture_pixelate texture_pixelate.c raygui.h)
target_link_libraries(texture_pixelate PRIVATE raylib)
