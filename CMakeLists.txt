cmake_minimum_required(VERSION 3.5)

set(SOEM_ROOT "SOEM-master/install")

include_directories(include)
link_directories(${SOEM_ROOT}/lib)
link_libraries(pthread rt)

add_library(os STATIC src/os.c)
add_library(stdio STATIC src/stdio.c)
add_library(soemio STATIC src/soem.c)

target_include_directories(soemio PUBLIC ${SOEM_ROOT}/include/soem)
target_link_libraries(soemio soem)
