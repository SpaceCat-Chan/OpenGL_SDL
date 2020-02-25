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
	glm::dvec3 m_Position, m_LookVector, m_Up;

	/**
	 * \brief updates the View matrix to reflect new variables
	 */
	void UpdateView();

	enum class Locked {
		Position,
		Direction
	};
	Locked m_LookAt=Locked::Direction;

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

	/**
	 * \brief get the product of the projection and view matrix
	 */
	glm::dmat4x4 GetMVP();

	/**
	 * \brief sets LookAt to Locked::Position, meaning the camera will try to look at the same position
	 */
	void LockViewPosition();
	/**
	 * \brief set LookAt to Locked::Direction, meaning the camera will try to look in the same direction
	 */
	void LockViewDirection();

	/**
	 * \brief Creates a projection matrix
	 * 
	 * \param fovY the fov in the y direction
	 * \param AspectRatio the aspect ratio of the window
	 * \param NearClip the near clipping plane
	 * \param FarClip the far clipping plane
	 */
	void CreateProjection(double fovY, double AspectRatio, double NearClip, double FarClip);

	/**
	 * \brief will make the camera look in a direction
	 * 
	 * will also set LookAt to to Locked::Direction, meaning if the camera moves
	 * it will keep loking in the same direction
	 * 
	 * \param Direction the direction to look in
	 * \param Up a vector pointing upwards
	 */
	void LookIn(glm::dvec3 Direction, glm::dvec3 Up={0, 1, 0});
	/**
	 * \brief will make the camera look at a position
	 * 
	 * will also set LookAt to be Locked::Position, meaning if the camera moves
	 * it will keep loking in the same direction
	 * 
	 * \param Position the position to look at
	 * \param Up a vector pointing upwards
	 */
	void LookAt(glm::dvec3 Position, glm::dvec3 Up={0, 1, 0});
	

	glm::dvec3 GetPosition();
	/**
	 * \brief makes the camera move by a certain amount
	 * 
	 * \param DeltaPosition the amount to move by
	 */
	void Move(glm::dvec3 DeltaPosition);
	/**
	 * \brief makes the camera move to a certain position
	 * 
	 * \param Position the position to move to
	 */
	void MoveTo(glm::dvec3 Position);
};

