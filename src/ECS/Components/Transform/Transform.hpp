#pragma once

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

/**
 * \brief the component containing information about how to transform from Model
 * space to View space
 */
struct Transform
{
	enum class Space
	{
		Tangent,
		Model,
		World,
		View,
		Clip
	};

	/**
	 * Type::Mix should be used for matrixes with unknown transformations
	 *
	 *
	 *
	 * about Type::AutoPosition:
	 *
	 * the first Matrix in Transform::Tranformations that is of type
	 * Type::AutoPosition will be set to a translation matrix to translate
	 * everything by the amount specified in the position compoent
	 *
	 * all other matrixes with type Type::AutoPosition will be set to the
	 * identity matrix
	 *
	 * this is only true if:
	 *
	 * 1. there is a position component active for this entity
	 * 2. PositionComponentSpace is set to Space::View
	 * 3. ::AutoPositionSystem is active
	 *
	 * and it will only be updated after ::AutoPositionSystem has been run
	 */
	enum class Type
	{
		Translate,
		Rotate,
		Scale,
		Mix,
		AutoPosition
	};

	/**
	 * \brief the space that the PositionComponent is in
	 *
	 * it is not expected for the value to be anything other than Space::Model
	 * or Space::View, but is can be other things too
	 */
	Space PositionComponentSpace = Space::View;
	/**
	 * \brief the space that positions of other components are in
	 *
	 * it is not expected for the value to be anything other than Space::Model
	 * or Space::View, but is can be other things too
	 */
	Space PositionSpace = Space::Model;

	/**
	 * \brief a list of transformations
	 *
	 * Transformations[i].first contains the type of tranformation that matrix
	 * does
	 *
	 * Transformations[i].second contains the actual matrix
	 */
	std::vector<std::pair<Type, glm::dmat4x4>> Tranformations;

	/**
	 * \brief calculates the full transformation matrix
	 *
	 * \return the full transformation matrix
	 */
	glm::dmat4x4 CalculateFull(const World &GameWorld, size_t id, bool UseBackup = false) const
	{
		glm::dmat4x4 Result(1);
		for (auto &Matrix : Tranformations)
		{
			if (Matrix.first == Type::AutoPosition)
			{
				if (GameWorld[id].Position() && !UseBackup)
				{
					Result = glm::translate(
					             glm::dmat4{1},
					             *GameWorld[id].Position())
					         * Result;
				}
				else if(GameWorld[id].BasicBackup()->Position && UseBackup)
				{
					Result = glm::translate(
					             glm::dmat4{1},
					             *GameWorld[id].BasicBackup()->Position)
					         * Result;
				}
			}
			else
			{
				Result = Matrix.second * Result;
			}
		}
		return Result;
	}

	/**
	 * \brief checks if there are any rotation transformations
	 *
	 * \return if there are any rotation transformations
	 */
	bool ContainsRotations() const
	{
		for (auto &Matrix : Tranformations)
		{
			if (Matrix.first == Type::Rotate || Matrix.first == Type::Mix)
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * \brief applies the entire transformation to a vector
	 *
	 * \param V the vector to transform
	 * \param UseRotations wether or not to ignore Matrixes with type
	 * Type::Rotate and Type::Mix
	 *
	 * \return the transformed vector
	 */
	glm::dvec4 operator()(glm::dvec4 V, const World &GameWorld, size_t id, bool UseBackup = false) const
	{
		return CalculateFull(GameWorld, id, UseBackup) * V;
	}
};
