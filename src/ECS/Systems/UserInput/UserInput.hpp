#pragma once

#include "Common.hpp"

struct World;
struct Error;

Error UserInputSystem(World &GameWorld, DSeconds dt);