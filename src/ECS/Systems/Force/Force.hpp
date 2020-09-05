#pragma once

#include "Common.hpp"

struct Error;
struct World;

Error ForceSystem(World &GameWorld, DSeconds dt);