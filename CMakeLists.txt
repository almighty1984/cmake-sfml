cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#file(COPY ${CMAKE_BINARY_DIR}/bin/${OUTPUT_NAME}
 #    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#set(SFML_DIR "D:\\Dev\\C++\\Libraries\\SFML-3.0.0\\lib\\cmake\\SFML\\")


include(FetchContent)
FetchContent_Declare(SFML
                     GIT_REPOSITORY https://github.com/SFML/SFML.git
                     GIT_TAG 3.0.0
                     GIT_SHALLOW ON
                     EXCLUDE_FROM_ALL
                     SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_subdirectory(core)
add_subdirectory(app)

add_executable(main src/main.cxx)
target_compile_features(main PRIVATE cxx_std_23)
target_link_libraries(main PRIVATE app)