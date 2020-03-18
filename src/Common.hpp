#pragma once

#include <chrono>

using DSeconds = std::chrono::duration<double>;


inline DSeconds operator ""_ds(long double time) {
	return DSeconds(time);
}