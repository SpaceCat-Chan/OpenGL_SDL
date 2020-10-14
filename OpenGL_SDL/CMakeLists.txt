find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(OpenGL_SDL)

target_sources(OpenGL_SDL PRIVATE
	Camera/Camera.cpp
	Mesh/Mesh.cpp
	Octree/Octree.cpp
	Shader/Shader.cpp
	Texture/Texture.cpp
	TexturedMesh/TexturedMesh.cpp
	Window/Window.cpp
	main.cpp
	SDL-Helper-Libraries/KeyTracker/KeyTracker.cpp
	SDL-Helper-Libraries/sfstream/sfstream.cpp
	minitrace/minitrace.cpp
)

add_subdirectory(ECS)

target_include_directories(OpenGL_SDL PRIVATE ./ ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})

target_link_libraries(OpenGL_SDL ${SDL2_LIBRARIES})
target_link_libraries(OpenGL_SDL GLEW::GLEW)
target_link_libraries(OpenGL_SDL ${OPENGL_LIBRARY})
target_link_libraries(OpenGL_SDL ${SDL_image_LIBRARIES})
target_link_libraries(OpenGL_SDL ${SDL_ttf_LIBRARIES})

file(COPY ../res DESTINATION ${CMAKE_BINARY_DIR}/OpenGL_SDL)