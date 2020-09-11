set (CMAKE_CXX_STANDARD 11)
file (GLOB SOURCES "src/*")
add_executable (yalta ${SOURCES})

if (MSVC)
    target_compile_definitions(yalta PRIVATE _CRT_SECURE_NO_WARNINGS)
endif(MSVC)
