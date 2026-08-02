cmake_minimum_required(VERSION 3.20)

project(DataStructure LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DATA_STRUCTURE_SOURCES
    "complexity_adt.cpp"
    "Linked list.cpp"
    "linear_structures.cpp"
    "string_match.cpp"
    "binary_tree.cpp"
    "heap.cpp"
    "avl_tree.cpp"
    "redblacktree.cpp"
    "splaytree.cpp"
    "multiwaytree.cpp"
    "bplustree.cpp"
    "kdtree.cpp"
    "graph.cpp"
    "hashing.cpp"
    "disjointset.cpp"
    "sort.cpp"
    "suffixtree.cpp"
    "suffixarray.cpp"
)

include(CTest)

foreach(source_file IN LISTS DATA_STRUCTURE_SOURCES)
    get_filename_component(target_name "${source_file}" NAME_WE)
    string(REPLACE " " "_" target_name "${target_name}")

    add_executable("${target_name}" "${source_file}")

    if(MSVC)
        target_compile_options("${target_name}" PRIVATE /W4 /permissive-)
    else()
        target_compile_options("${target_name}" PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(BUILD_TESTING)
        add_test(NAME "${target_name}" COMMAND "${target_name}")
    endif()
endforeach()
