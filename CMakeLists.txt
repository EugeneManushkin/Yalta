project(Yalta)
set (YALTA_VERSION_MAJOR 0 CACHE STRING "Plugin major version")
set (YALTA_VERSION_MINOR 0 CACHE STRING "Plugin minor version")
set (YALTA_BUILD 0 CACHE STRING "Plugin build number")
execute_process(COMMAND git rev-parse HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE YALTA_GIT_COMMIT
                OUTPUT_STRIP_TRAILING_WHITESPACE
)
set (CMAKE_CXX_STANDARD 11)
file (GLOB SOURCES "src/*")
add_executable (yalta ${SOURCES})
target_compile_definitions(yalta PRIVATE YALTA_VERSION_MAJOR=${YALTA_VERSION_MAJOR}
                                         YALTA_VERSION_MINOR=${YALTA_VERSION_MINOR}
                                         YALTA_BUILD=${YALTA_BUILD}
                                         YALTA_GIT_COMMIT=${YALTA_GIT_COMMIT}
)
if (MSVC)
    target_compile_definitions(yalta PRIVATE _CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
endif(MSVC)
