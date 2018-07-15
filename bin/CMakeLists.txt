file(GLOB SRC_FILES *.cpp)

add_executable(tinyscript ${SRC_FILES})
target_link_libraries(tinyscript tinyvm)
install(TARGETS tinyscript DESTINATION bin)
