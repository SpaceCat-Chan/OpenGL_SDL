#pragma once

#include <chrono>

using DSeconds = std::chrono::duration<double>;


DSeconds operator ""_ds(long double time) {
	return DSeconds(time);
}