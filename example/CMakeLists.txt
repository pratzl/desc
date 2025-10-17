# Example programs for the graph library

add_executable(basic_usage basic_usage.cpp)
target_link_libraries(basic_usage PRIVATE graph::graph)

# Future examples will be added here as they are created
# add_executable(adapting_third_party AdaptingThirdPartyGraph/example.cpp)
# add_executable(cppcon2021 CppCon2021/example.cpp)
# add_executable(cppcon2022 CppCon2022/example.cpp)
# add_executable(pagerank PageRank/example.cpp)
