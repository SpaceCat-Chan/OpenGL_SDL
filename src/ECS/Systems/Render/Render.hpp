#pragma once

#include "Common.hpp"

struct World;
struct Error;


Error RenderSystem(World &GameWorld, DSeconds dt);