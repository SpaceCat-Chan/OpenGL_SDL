#pragma once

#include <Mesh/Mesh.hpp>
#include <Camera/Camera.hpp>
#include <Shader/Shader.hpp>

/**
 * \brief renders a mesh
 * 
 * \param ToRender the Mesh to render
 * \param Camera the Camera that should be used
 * \param ShaderProgram the Shader to use in the rendering pipeline, expected to have a uniform named MVP
 * \param Transform any extra transforms to use, defaults to the unit matrix
 */
void Render(Mesh& ToRender, Camera& Camera, Shader& ShaderProgram, glm::dmat4x4 Transform=glm::dmat4x4(1));
