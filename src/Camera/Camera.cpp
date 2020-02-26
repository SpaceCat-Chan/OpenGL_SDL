#include "Camera.hpp"

Camera::Camera(glm::dmat4x4 Projection, glm::dmat4x4 View)
{
	m_Projection = Projection;
	m_View = View;
	//no way i know that we can get the Up, Eye and Center vectors from this
}

Camera::Camera(const Camera &Copy)
{
	m_Projection = Copy.m_Projection;
	m_View = Copy.m_View;
	m_Position = Copy.m_Position;
	m_LookVector = Copy.m_LookVector;
	m_Up = Copy.m_Up;
	m_LookAt = Copy.m_LookAt;
}

Camera::Camera(Camera &&Move)
{
	m_Projection = std::move(Move.m_Projection);
	m_View = std::move(Move.m_View);
	m_Position = std::move(Move.m_Position);
	m_LookVector = std::move(Move.m_LookVector);
	m_Up = std::move(Move.m_Up);
	m_LookAt = Move.m_LookAt;
}

const Camera &Camera::operator=(const Camera &Copy)
{
	m_Projection = Copy.m_Projection;
	m_View = Copy.m_View;
	m_Position = Copy.m_Position;
	m_LookVector = Copy.m_LookVector;
	m_Up = Copy.m_Up;
	m_LookAt = Copy.m_LookAt;

	return *this;
}

const Camera &Camera::operator=(Camera &&Move)
{
	m_Projection = std::move(Move.m_Projection);
	m_View = std::move(Move.m_View);
	m_Position = std::move(Move.m_Position);
	m_LookVector = std::move(Move.m_LookVector);
	m_Up = std::move(Move.m_Up);
	m_LookAt = Move.m_LookAt;

	return *this;
}

void Camera::SetProjection(glm::dmat4x4 Projection)
{
	m_Projection = Projection;
}
glm::dmat4x4 Camera::GetProjection()
{
	return m_Projection;
}

void Camera::SetView(glm::dmat4x4 View)
{
	m_View = View;
}
glm::dmat4x4 Camera::GetView()
{
	return m_View;
}
glm::dvec3 Camera::GetPosition()
{
	return m_Position;
}

glm::dmat4x4 Camera::GetMVP()
{
	return m_Projection * m_View;
}

void Camera::LockViewPosition()
{
	m_LookAt = Locked::Position;
	UpdateView();
}
void Camera::LockViewDirection()
{
	m_LookAt = Locked::Direction;
	UpdateView();
}

glm::dvec3 Camera::GetViewVector()
{
	return m_LookVector;
}
Camera::Locked Camera::GetViewLock()
{
	return m_LookAt;
}

void Camera::CreateProjection(double fovY, double AspectRatio, double NearClip, double FarClip)
{
	m_Projection = glm::perspective<double>(fovY, AspectRatio, NearClip, FarClip);
}
void Camera::CreateProjectionX(double fovX, double AspectRatio, double NearClip, double FarClip)
{
	double fovY = (2 * glm::atan(glm::tan(fovX * 0.5) * AspectRatio));
	m_Projection = glm::perspective<double>(fovY, AspectRatio, NearClip, FarClip);
}

void Camera::LookIn(glm::dvec3 Direction, glm::dvec3 Up /*={0, 1, 0}*/)
{
	m_LookVector = Direction;
	m_Up = Up;

	LockViewDirection();
}

void Camera::LookAt(glm::dvec3 Position, glm::dvec3 Up /*={0, 1, 0}*/)
{
	m_LookVector = Position;
	m_Up = Up;

	LockViewPosition();
}

void Camera::Move(glm::dvec3 DeltaPosition)
{
	m_Position += DeltaPosition;
	UpdateView();
}
void Camera::MoveTo(glm::dvec3 Position)
{
	m_Position = Position;
	UpdateView();
}

void Camera::UpdateView()
{
	if (m_LookAt == Locked::Direction)
	{
		m_View = glm::lookAt(m_Position, m_Position + m_LookVector, m_Up);
	}
	else
	{
		m_View = glm::lookAt(m_Position, m_LookVector, m_Up);
	}
}