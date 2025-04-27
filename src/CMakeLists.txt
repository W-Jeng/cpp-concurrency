add_executable(threadsafe_stack threadsafe_stack.cpp)
add_executable(hierarchical_mutex hierarchical_mutex.cpp)

target_include_directories(threadsafe_stack PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(hierarchical_mutex PRIVATE ${CMAKE_SOURCE_DIR}/include)