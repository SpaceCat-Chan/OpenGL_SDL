#pragma once

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

class World;
class Error;

/**
 * \brief a struct which contains information about an entities child-parent
 * Relatioships
 */
struct Children
{
	long Parent = -1;
	std::vector<size_t> Children;

	/**
	 * \brief return the parents Transform matrix
	 *
	 * \param GameWorld the world this entity is in
	 *
	 * \return the Transform matrix of the parent
	 */
	glm::dmat4x4 CalculateParentTransform(World &GameWorld);

	/**
	 * \brief return this entities transform matrix multipled with the parents
	 *
	 * \param GameWorld the world this entity is in
	 * \param Me the id of this entity
	 *
	 * \return this entities full transform matrix
	 */
	glm::dmat4x4 CalculateFullTransform(World &GameWorld, size_t Me);

	/**
	 * \brief makes sure that all other entities have this entity listed in the
	 * correct place
	 *
	 * \param Gameworld the world this entity is in
	 * \param Me this entities ID
	 */
	Error EnforceCorrectness(World &GameWorld, size_t Me);
};