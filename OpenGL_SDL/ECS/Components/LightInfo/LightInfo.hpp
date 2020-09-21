#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

struct LightInfo
{
	enum class Type
	{
		Point,
		Direction
	};
	/**
	 * \brief the color of the light
	 */
	glm::dvec3 Color = {1, 1, 1};

	/**
	 * \brief wether the light is a point light or a directional light
	 *
	 * exmaples:
	 * point light - a light bulb
	 * directional light - the sun
	 */
	Type LightType = Type::Point;
	/**
	 * \brief position of the light
	 *
	 * ignored for directional lights
	 */
	glm::dvec3 Position = {0, 0, 0};
	/**
	 * \brief the direction the light is pointing in
	 *
	 * ignored for point lights with default cutoff angle
	 */
	glm::dvec3 Direction = {1, 0, 0};

	/**
	 * \brief the angle where the light gets cut off
	 *
	 * ignored for directional lights
	 */
	double CutoffAngle = glm::pi<double>();
	/**
	 * \brief the max distance the light can affect objects
	 *
	 * ignored for directional lights
	 */
	double CutoffDistance = 100;

	/**
	 * \name attenuation
	 *
	 * theese variables are needed for calculating the attenuation of the light,
	 * given by this formula:
	 *
	 * 1.0 / (Constant + Linear * Distance + Quadratic * pow(distance, 2))
	 *
	 * the standard is tuned for a max distance of 100
	 *
	 * ignored by directional lights
	 */
	///@{
	double Constant = 1;
	double Linear = 0.045;
	double Quadratic = 0.0075;
	///@}

	LightInfo() = default;
	LightInfo(const LightInfo &) = default;
	LightInfo(LightInfo &&) = default;
	LightInfo(
	    Type _LightType,
	    double _CutoffDistance,
	    double _CutoffAngle = glm::pi<double>() * 2,
	    double _Constant = 1,
	    double _Linear = 0.045,
	    double _Quadratic = 0.0075)
	{
		LightType = _LightType;
		CutoffAngle = _CutoffAngle;
		CutoffDistance = _CutoffDistance;
		Constant = _Constant;
		Linear = _Linear;
		Quadratic = _Quadratic;
	}

	constexpr LightInfo &operator=(const LightInfo &) = default;
	constexpr LightInfo &operator=(LightInfo &&) = default;
};