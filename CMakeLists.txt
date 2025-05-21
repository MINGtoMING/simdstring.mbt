cmake_minimum_required(VERSION 3.28)
project(simdstring)

#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)
#add_compile_options(-fsanitize=leak)
#add_link_options(-fsanitize=leak)
#add_compile_options(-fsanitize=memory)
#add_link_options(-fsanitize=memory)
add_compile_options(-fno-omit-frame-pointer -g)

get_filename_component(MOON_HOME "~/.moon" ABSOLUTE)
add_library(rt STATIC)
target_include_directories(rt PUBLIC ${MOON_HOME}/include)
target_sources(rt PRIVATE ${MOON_HOME}/lib/runtime.c)

add_library(avx2 STATIC)
target_link_libraries(avx2 rt)
target_include_directories(avx2 PRIVATE avx2/)
target_sources(avx2 PRIVATE avx2/contains_char.c)

add_library(sse STATIC)
target_link_libraries(sse rt)
target_include_directories(sse PRIVATE sse/)
target_sources(sse PRIVATE sse/contains_char.c)

add_library(scalar STATIC)
target_link_libraries(scalar rt)
target_include_directories(scalar PRIVATE sse/)
target_sources(scalar PRIVATE scalar/contains_char.c)

#add_executable(main target/native/release/build/main/main.c)
#target_link_libraries(main rt avx2 sse4_2 m)

#find_package(benchmark CONFIG REQUIRED)
#
#add_executable(bench bench.cpp)
#target_include_directories(bench PRIVATE avx2/)
##target_sources(bench PRIVATE avx2/contains.c)
#target_link_libraries(bench rt benchmark::benchmark benchmark::benchmark_main)