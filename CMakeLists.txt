cmake_minimum_required(VERSION 3.10)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR} FORCE)
endif()

project(PanzerToy VERSION 0.1)

# C++17 only
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

configure_file(src/version.hpp.in src/version.hpp)

option(BUILD_SHARED_LIBS OFF)

option(ASSIMP_NO_EXPORT ON)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_BOOST_WORKAROUND TRUE)
option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_COLLADA_IMPORTER BOOL TRUE)
add_subdirectory(deps/assimp)

option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(deps/glfw)

add_subdirectory(deps/glm)
add_subdirectory(include)

set(SRC_FILES
	src/bullet.cpp
	src/camera.cpp
	src/enemy.cpp
	src/game.cpp
	src/main.cpp
	src/map.cpp
	src/mesh.cpp
	src/model.cpp
	src/player.cpp
	src/shader.cpp
	src/tank.cpp
	src/texture.cpp
)

add_executable(PanzerToy ${SRC_FILES})

target_link_libraries(PanzerToy assimp gl3w glfw glm)

target_include_directories(PanzerToy PUBLIC ${PROJECT_BINARY_DIR}/src)
target_include_directories(PanzerToy PUBLIC ${PROJECT_SOURCE_DIR}/include)

install(TARGETS PanzerToy RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_BINARY_DIR}/bin)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/models DESTINATION ${PROJECT_BINARY_DIR}/bin)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/shaders DESTINATION ${PROJECT_BINARY_DIR}/bin)
