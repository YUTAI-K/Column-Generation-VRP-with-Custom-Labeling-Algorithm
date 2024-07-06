cmake_minimum_required(VERSION 3.16)
project(cg)

#if(NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE "Release")
#endif()

set(CMAKE_CXX_STANDARD 23)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR})

if(CXX)
    enable_language(CXX)
endif()
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


#if(MSVC AND MT)
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
#    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
#    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
#endif()

add_executable(cg
        src/Graph.cpp
        src/Graph.h
        src/main.cpp
        src/Route.cpp
        src/Route.h
        src/Solver.cpp
        src/Solver.h
        src/ShortestPath.h
        src/ShortestPath.cpp
        src/ElementaryLabel.cpp
        src/ElementaryLabel.h
        src/Solver_2.cpp
        src/Solver_3.cpp
)

# Check if the compiler is MSVC and set specific options
if(MSVC)
    set(WARNING_OPTIONS     /W4)
    set(DEBUG_OPTIONS       /Od /Zi)
    set(GDB_DEBUG_OPTIONS   /Zi /DEBUG)
    set(LINKER_OPTIONS      /INCREMENTAL:NO /OPT:REF)
else()
    # Assuming the else block is for GCC as per your original setup
    set(WARNING_OPTIONS     -Wall -Wextra -Werror)
    set(GCC_WARNING_OPTIONS -Wduplicated-cond -Wlogical-op -Wuseless-cast -Wrestrict)
    set(DEBUG_OPTIONS       -O0 -g)
    set(GDB_DEBUG_OPTIONS   -ggdb)
    set(LINKER_OPTIONS      -flto)
endif()

# Visual Studio compiler with static runtime libraries
#if(MSVC AND MT)
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
#    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
#    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
#endif()


find_package(GUROBI REQUIRED)
find_package(Boost REQUIRED COMPONENTS graph)

target_include_directories( cg SYSTEM PRIVATE ${GUROBI_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_compile_options(     cg PRIVATE ${WARNING_OPTIONS})
target_compile_options(     cg PRIVATE $<$<CXX_COMPILER_ID:GNU>:${GCC_WARNING_OPTIONS}>)
target_compile_options(     cg PRIVATE $<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>)
target_compile_options(     cg PRIVATE $<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:GNU>>:${GDB_DEBUG_OPTIONS}>)

################################################################
#target_link_libraries(      cg
#        optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(TARGET_LIB ${GUROBI_CXX_DEBUG_LIBRARY})
else()
    set(TARGET_LIB ${GUROBI_CXX_LIBRARY})
endif()
target_link_libraries(cg ${TARGET_LIB})
##################################################################3



target_link_libraries(      cg ${GUROBI_LIBRARY})
target_link_libraries(      cg PRIVATE ${BOOST_GRAPH_LIBRARY})


message(STATUS "GUROBI_LIBRARY IS ${GUROBI_LIBRARY}")
message(STATUS "GUROBI_CXX_LIBRARY IS ${GUROBI_CXX_LIBRARY}")
message(STATUS "GUROBI_CXX_DEBUG_LIBRARY IS ${GUROBI_CXX_DEBUG_LIBRARY}")