#pragma once

#include "Common.hpp"

struct World;
struct Error;

Error UpdateOctree(World &GameWorld, DSeconds dt);
Error HandleCollisions(World &GameWorld, DSeconds dt);