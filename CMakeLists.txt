cmake_minimum_required(VERSION 3.28)
project(simdstring)

get_filename_component(MOON_HOME "~/.moon" ABSOLUTE)
add_library(runtime STATIC)
target_include_directories(runtime PUBLIC ${MOON_HOME}/include)
target_sources(runtime PRIVATE ${MOON_HOME}/lib/runtime.c)

add_library(contains_char STATIC src/contains_char.c)
target_link_libraries(contains_char runtime)
#target_compile_options(contains_char PRIVATE -mavx2)
#target_compile_options(contains_char PRIVATE -mno-sse2)
# target_compile_options(contains_char PRIVATE -mavx512f -mavx512bw)
#target_compile_options(contains_char PRIVATE -march=rv64gcv)

 add_executable(main target/native/release/build/main/main.c)
 target_link_libraries(main runtime contains_char m)

