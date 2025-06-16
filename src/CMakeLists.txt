cmake_minimum_required(VERSION 3.25)

project(MyProj LANGUAGES C CXX)

#### DEFINED BUILD TYPES ####
# for seemless integration with vscode cmake-tools, keep them up to date with projects cmake-variants.yaml file
# multi-config generators not supported (easy to implement if found necessary, see Professional CMake book)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY 
    STRINGS Debug Release
)


#### COMPILATION FLAGS (HARD-CODED) ####
# assuming gcc compiler
# default MinSizeRel and RelWithDebInfo unused

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(WARNING "Unexpected compiler (ID=${CMAKE_CXX_COMPILER_ID}) used!")
endif()

#-Wpedantic
set(COMMON_FLAGS -Wall -Wextra -fopenmp -masm=intel -mclflushopt) # ensure that clflushopt instruction is available on target architecture!
set(RELEASE_FLAGS -O3)
set(DEBUG_FLAGS -O0 -g -DNDEBUG)
set(SANITIZER_FLAGS -fsanitize=address,undefined)
set(ASM_FLAGS -S)

list(JOIN COMMON_FLAGS " " _COMMON_FLAGS_)
list(JOIN RELEASE_FLAGS " " _RELEASE_FLAGS_)
list(JOIN DEBUG_FLAGS " " _DEBUG_FLAGS_)
list(JOIN SANITIZER_FLAGS " " _SANITIZER_FLAGS_)
list(JOIN ASM_FLAGS " " _ASM_FLAGS_)

# DEBUG builds
set(CMAKE_C_FLAGS_DEBUG   "${_COMMON_FLAGS_} ${_DEBUG_FLAGS_} ${_SANITIZER_FLAGS_}" CACHE STRING "Flags used by the C compiler during DEBUG builds." FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${_COMMON_FLAGS_} ${_DEBUG_FLAGS_} ${_SANITIZER_FLAGS_}" CACHE STRING "Flags used by the CXX compiler during DEBUG builds." FORCE)

# RELASE builds
set(CMAKE_C_FLAGS_RELEASE   "${_COMMON_FLAGS_} ${_RELEASE_FLAGS_}" CACHE STRING "Flags used by the C compiler during RELEASE builds." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${_COMMON_FLAGS_} ${_RELEASE_FLAGS_}" CACHE STRING "Flags used by the CXX compiler during RELEASE builds." FORCE)


#### DEFINE CUSTOM HELPER FUNCTIONS ####

## Add asm output to Release builds
function(add_asm target_name)
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        # Create asm folder if nonexistant
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/asm)
        # Instruct gcc to save temporaries into asm folder, prepending the target name
        # -save-temps saves .i, .s and .o files (but cmake saves .o's in CMakeFiles/target.dir anyways)
        target_compile_options(${target_name} PRIVATE -save-temps -dumpdir ${CMAKE_CURRENT_BINARY_DIR}/asm/${target_name}__)
        # Remove *.ii files from asm folder
        # add_custom_command(TARGET ${target_name} POST_BUILD
        #     COMMAND /bin/sh -c "rm -f ${target_name}*.i" # note that the target_name (or something alike) is also needed here in order for deletions not to interfere with compilation of another executable from the same binary directory
        #     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/asm
        #     VERBATIM
        # )
    endif()
endfunction()


#### GENERATE EXECUTABLES ####

add_executable(l2u  main.cpp load-use-latency.cpp)
add_asm(l2u)

add_executable(latency_vs_bandwidth  latency-vs-bandwidth.cpp)

add_executable(basiccache  basiccache.cpp)
target_compile_options(basiccache PRIVATE -mcldemote)

add_executable(replacement l1-replacement-strategy.cpp)

add_executable(cache_coherence cache_coherence.cpp)
add_asm(cache_coherence)

add_executable(cc_read cache_coherence.cpp)
target_compile_definitions(cc_read PRIVATE "-DOP=read")

add_executable(cc_readTwice cache_coherence.cpp)
target_compile_definitions(cc_readTwice PRIVATE "-DOP=readTwice")

add_executable(cc_aread cache_coherence.cpp)
target_compile_definitions(cc_aread PRIVATE "-DOP=alternatingRead")

add_executable(cc_fread cache_coherence.cpp)
target_compile_definitions(cc_fread PRIVATE "-DOP=fencedRead")

add_executable(cc_write cache_coherence.cpp)
target_compile_definitions(cc_write PRIVATE "-DOP=write")

add_executable(cc_writeTwo cache_coherence.cpp)
target_compile_definitions(cc_writeTwo PRIVATE "-DOP=writeTwo")

add_executable(cc_awrite cache_coherence.cpp)
target_compile_definitions(cc_awrite PRIVATE "-DOP=alternatingWrite")

add_executable(cc_fwrite cache_coherence.cpp)
target_compile_definitions(cc_fwrite PRIVATE "-DOP=fencedWrite")

add_executable(cc_update cache_coherence.cpp)
target_compile_definitions(cc_update PRIVATE "-DOP=update")

add_executable(cc_atomicUpdate cache_coherence.cpp)
target_compile_definitions(cc_atomicUpdate PRIVATE "-DOP=atomicUpdate")
