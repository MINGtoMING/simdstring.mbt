cmake_minimum_required(VERSION 3.28)
project(simdstring)

get_filename_component(MOON_HOME "~/.moon" ABSOLUTE)
add_library(runtime STATIC)
target_include_directories(runtime PUBLIC ${MOON_HOME}/include)
target_sources(runtime PRIVATE ${MOON_HOME}/lib/runtime.c)

add_compile_options(-march=rv64gcv -mabi=lp64d)

add_library(find_char STATIC src/find_char.c)
target_link_libraries(find_char runtime)
#target_compile_options(find_char PRIVATE -mavx2)
#target_compile_options(find_char PRIVATE -mavx512f -mavx512bw)
#target_compile_options(find_char PRIVATE -mno-sse2)
#target_compile_options(find_char PRIVATE -march=rv64gcv -mabi=lp64d)

add_library(rev_find_char STATIC src/rev_find_char.c)
target_link_libraries(rev_find_char runtime)
#target_compile_options(rev_find_char PRIVATE -mavx2)
#target_compile_options(rev_find_char PRIVATE -mno-sse2)
#target_compile_options(rev_find_char PRIVATE -mavx512f -mavx512bw)

add_library(find STATIC src/find.c)
target_link_libraries(find runtime)
#target_compile_options(find PRIVATE -mno-sse2)

# add_executable(main target/native/release/build/main/main.c)
# target_link_libraries(main runtime contains_char m)

