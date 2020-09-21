#include "UserInput.hpp"

#include "ECS/ECS.hpp"

KeyTracker UserInput::Keyboard;
std::vector<std::function<Error(World &, DSeconds)>> UserInput::PrePoll;
std::vector<std::function<Error(World &, DSeconds)>> UserInput::PostPoll;
std::vector<std::function<Error(World &, DSeconds, SDL_Event *)>>
    UserInput::PreEvent;
std::vector<std::function<Error(World &, DSeconds, SDL_Event *)>>
    UserInput::PostEvent;