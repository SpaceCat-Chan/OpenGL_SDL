#include "UserInput.hpp"

#include "ECS/ECS.hpp"

Error UserInputSystem(World &GameWorld, DSeconds dt)
{

	for (auto &Handler : UserInput::PrePoll)
	{
		Handler(GameWorld, dt);
	}

	SDL_Event Event;
	UserInput::Keyboard.Update(1);
	while (SDL_PollEvent(&Event))
	{
		for (auto &Handler : UserInput::PreEvent)
		{
			Handler(GameWorld, dt, &Event);
		}

		UserInput::Keyboard.UpdateKey(&Event);
		switch (Event.type)
		{

		case SDL_QUIT:
			GameWorld.Quit = true;
			break;

		case SDL_MOUSEMOTION:
			constexpr double Sensitivity = 0.1;
			/*
			GameWorld.View.OffsetPitchYaw(
			    glm::radians(Event.motion.yrel * Sensitivity * -1),
			    glm::radians(Event.motion.xrel * Sensitivity),
			    0,
			    glm::radians(-89.0),
			    glm::radians(89.0));
			*/
			GameWorld.View.RotatePitch(
			    glm::radians(Event.motion.yrel * Sensitivity));
			GameWorld.View.RotateYaw(
			    glm::radians(Event.motion.xrel * Sensitivity));
		}

		for (auto &Handler : UserInput::PostEvent)
		{
			Handler(GameWorld, dt, &Event);
		}
	}

	for (auto &Handler : UserInput::PostPoll)
	{
		Handler(GameWorld, dt);
	}

	if (UserInput::Keyboard[SDL_SCANCODE_ESCAPE].Clicked)
	{
		GameWorld.Quit = true;
	}

	if (UserInput::Keyboard[SDL_SCANCODE_W].Active)
	{
		GameWorld.View.Move(
		    (glm::normalize(GameWorld.View.GetViewVector()))
		    * (1.0 * dt.count()));
	}
	if (UserInput::Keyboard[SDL_SCANCODE_S].Active)
	{
		GameWorld.View.Move(
		    (glm::normalize(GameWorld.View.GetViewVector()))
		    * (1.0 * -dt.count()));
	}

	if (UserInput::Keyboard[SDL_SCANCODE_E].Active)
	{
		*GameWorld[20].Position() += glm::dvec3{0.1, 0, 0} * dt.count();
		GameWorld.UpdatedEntities.push_back(20);
	}
	if (UserInput::Keyboard[SDL_SCANCODE_Q].Active)
	{
		*GameWorld[19].Position() += glm::dvec3{0.1, 0, 0} * dt.count();
		GameWorld.UpdatedEntities.push_back(19);
	}

	if (UserInput::Keyboard[SDL_SCANCODE_LEFT].Clicked)
	{
		GameWorld.View.RotateRoll(glm::radians(-10.0));
	}
	if (UserInput::Keyboard[SDL_SCANCODE_RIGHT].Clicked)
	{
		GameWorld.View.RotateRoll(glm::radians(10.0));
	}
	if (UserInput::Keyboard(SDLK_UP).Clicked)
	{
		GameWorld.View.UpdateView();
	}

	return Error(Error::Type::None);
}
