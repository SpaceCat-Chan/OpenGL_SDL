#pragma once

#include "Common.hpp"

struct World;
struct Error;

/**
 * \brief the system responsible for updating Matrixes of type
 * Transform::Type::AutoPosition
 */
Error AutoPositionSystem(World &GameWorld, DSeconds dt);