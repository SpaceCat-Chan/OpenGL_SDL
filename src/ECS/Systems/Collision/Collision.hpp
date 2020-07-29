#pragma once

#include "Common.hpp"

class World;
class Error;

Error UpdateOctree(World &GameWorld, DSeconds dt);
Error HandleCollisions(World &GameWorld, DSeconds dt);