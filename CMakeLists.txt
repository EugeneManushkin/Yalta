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

set(PRODUCT_DIR ${CMAKE_BINARY_DIR}/$<CONFIG>/yalta)
file(GLOB README_FILES "${CMAKE_SOURCE_DIR}/README*.md")
file(GLOB SCRIPTS "${CMAKE_SOURCE_DIR}/scripts/*")
add_custom_command(TARGET yalta POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PRODUCT_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:yalta> ${PRODUCT_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${README_FILES} ${PRODUCT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${SCRIPTS} ${PRODUCT_DIR}
)
