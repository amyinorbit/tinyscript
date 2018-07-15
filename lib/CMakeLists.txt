file(GLOB COMPILER_FILES compiler/*.cpp)
file(GLOB RUNTIME_FILES runtime/*.cpp)

add_library(tinyvm STATIC ${COMPILER_FILES} ${RUNTIME_FILES})
target_include_directories(tinyvm INTERFACE ${PROJECT_SOURCE_DIR}/include)
install(TARGETS tinyvm DESTINATION lib)
