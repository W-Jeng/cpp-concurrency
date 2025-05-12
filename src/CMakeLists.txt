add_executable(threadsafe_stack threadsafe_stack.cpp)
add_executable(hierarchical_mutex hierarchical_mutex.cpp)
add_executable(threadsafe_queue threadsafe_queue.cpp)

target_include_directories(threadsafe_stack PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(hierarchical_mutex PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(threadsafe_queue PRIVATE ${CMAKE_SOURCE_DIR}/include)