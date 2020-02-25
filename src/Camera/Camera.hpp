/**
 * \file Camera.hpp 
 */
#pragma once

#include <iostream>

#include <glm/ext.hpp>

/**
 * \class Camera
 * \brief a class that controls the projection and view matrix with a simpler interface
 * 
 * 
 */
class Camera {
	glm::dmat4x4 m_Projection, m_View;
	glm::dvec4 m_Position, m_LookDirection;

	public:

	Camera() = default;
	/**
	 * \brief Construct a Camera with a projection matrix and a view matrix
	 * 
	 * \param Projection the projection matrix to use
	 * \param View the view matrix to use
	 */
	Camera(glm::dmat4x4 Projection, glm::dmat4x4 View);
	Camera(const Camera& Copy);
	Camera(Camera&& Move);

	const Camera& operator=(const Camera& Copy);
	const Camera& operator=(Camera&& Move);

	void SetProjection(glm::dmat4x4 Projection);
	glm::dmat4x4 GetProjection();

	void SetView(glm::dmat4x4 View);
	glm::dmat4x4 GetView();


	void CreateProjection(double fovY, double AspectRatio, double NearClip, double FarClip);
	void LookInDirection(glm::dvec4 Direction, glm::dvec4 Up);
	void LookAt(glm::dvec4 Position, glm::dvec4 Up);
	
};