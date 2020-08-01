#pragma once

#include "Common.hpp"

struct Error;
struct World;

Error BasicBackupSystem(World &GameWorld, DSeconds dt);