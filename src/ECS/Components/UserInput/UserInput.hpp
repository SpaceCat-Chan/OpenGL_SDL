#pragma once

#include <vector>
#include <functional>

#include "SDL-Helper-Libraries/KeyTracker/KeyTracker.hpp"

#include "Common.hpp"

class World;
class Error;

/**
 * \brief structure for containing user input
 */
struct UserInput
{
	/**
	 * \brief the keyboard
	 */
	static KeyTracker Keyboard;

	/**
	 * \brief for events before polling begins
	 */
	static std::vector<std::function<Error(World &, DSeconds)>> PrePoll;
	/**
	 * \brief for events after polling has finished
	 */
	static std::vector<std::function<Error(World &, DSeconds)>> PostPoll;

	/**
	 * \brief for events before an SDL_Event is registred by the Keyboard
	 */
	static std::vector<std::function<Error(World &, DSeconds, SDL_Event *)>>
	    PreEvent;
	/**
	 * \brief for events after an SDL_Event is registered by the Keyboard
	 */
	static std::vector<std::function<Error(World &, DSeconds, SDL_Event *)>>
	    PostEvent;
};