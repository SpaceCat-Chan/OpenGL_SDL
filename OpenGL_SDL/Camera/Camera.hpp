#pragma once

#include <iostream>
#include <tuple>

#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

constexpr double NaN = std::numeric_limits<double>::signaling_NaN();

/**
 * \class Camera
 * \brief a class that controls the projection and view matrix with a simpler interface
 * 
 * 
 */
class Camera
{
	glm::dmat4x4 m_Projection, m_View;
	glm::dvec3 m_Position, m_LookingAt, m_FPSUp={0,1,0};
	glm::dquat m_Rotation{glm::quatLookAt(glm::normalize(glm::dvec3{0,0,1}), glm::dvec3{0,1,0})};

	

	bool m_LookingAtPosition = false;
	bool m_FPSmode = true;

public:
	/**
	 * \brief updates the View matrix to reflect new variables
	 */
	void UpdateView();


	Camera() = default;
	Camera(glm::dmat4x4 Projection, glm::dmat4x4 View);
	Camera(const Camera &Copy);
	Camera(Camera &&Move);

	const Camera &operator=(const Camera &Copy);
	const Camera &operator=(Camera &&Move);

	void SetProjection(const glm::dmat4x4& Projection);
	glm::dmat4x4 GetProjection() const;

	glm::dvec3 GetViewVector() const;

	void SetView(const glm::dmat4x4& View);
	glm::dmat4x4 GetView() const;

	glm::dmat4x4 GetMVP() const;

	
	public:
	void CreateProjection(double fovY, double AspectRatio, double NearClip, double FarClip);
	void CreateProjectionX(double fovX, double AspectRatio, double NearClip, double FarClip);

	void LookIn(glm::dvec3 Direction, glm::dvec3 Up = {0, 1, 0});
	void LookIn(bool FPSmode, glm::dvec3 Direction, glm::dvec3 Up = {0, 1, 0});

	void LookAt(bool KeepLookingAt, glm::dvec3 Position, glm::dvec3 Up = {0, 1, 0});
	void LookAt(bool KeepLookingAt, bool FPSmode, glm::dvec3 Position, glm::dvec3 Up = {0, 1, 0});

	void RotatePitch(double Angle);
	void RotateYaw(double Angle);
	void RotateRoll(double Angle);

	glm::dvec3 GetPosition() const;
	void Move(glm::dvec3 DeltaPosition);
	void MoveTo(glm::dvec3 Position);
};
