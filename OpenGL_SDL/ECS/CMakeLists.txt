target_sources(OpenGL_SDL PRIVATE
	ComponentInterface.cpp
	ECS.cpp
	TransformedMinMax.cpp
)

add_subdirectory(Components)
add_subdirectory(Systems)