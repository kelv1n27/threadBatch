cmake_minimum_required(VERSION 3.15)
project(ThreadBatchRunner)

add_library(comp_flags INTERFACE)
target_compile_features(comp_flags INTERFACE cxx_std_23)

add_library(ThreadBatch threadBatchCore.cpp)
add_library(ThreadPool threadPoolBatchCore.cpp)

add_executable(ThreadBatchRunner threadBatchRunner.cpp)

target_link_libraries(ThreadBatch PUBLIC comp_flags)
target_link_libraries(ThreadPool PUBLIC comp_flags)
target_link_libraries(ThreadBatchRunner PUBLIC comp_flags ThreadBatch ThreadPool)