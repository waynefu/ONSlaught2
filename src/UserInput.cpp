/*
* Copyright (c) 2013-2014, Helios (helios.vmg@gmail.com)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, 
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of the author may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*     * Products derived from this software may not be called "ONSlaught" nor
*       may "ONSlaught" appear in their names without specific prior written
*       permission from the author. 
*
* THIS SOFTWARE IS PROVIDED BY HELIOS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL HELIOS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "stdafx.h"
#ifndef PRECOMPILED_HEADERS_ARE_AVAILABLE
#endif
#include "UserInput.h"

using ONSlaught::UserInput;
using ONSlaught::event_queue_t;

UserInput::UserInput(){
	this->setup_joysticks();
}

UserInput::~UserInput(){
	this->free_joysticks();
}

event_queue_t UserInput::get(){
	event_queue_t ret;
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		this->transform_event(event);
		ret.push_back(event);
	}
	return ret;
}

template <typename T>
inline T abs(T x){
	return x < 0 ? -x : x;
}

bool decode_joystick_event(SDL_Event &event, std::vector<SDL_Joystick *> &joysticks){
	if (event.type == SDL_JOYAXISMOTION){
		int value = event.jaxis.value;
		if (abs(value) < (1<<14))
			return 0;
		int axis = 0;
		if (SDL_JoystickNumAxes(joysticks[event.jaxis.which]) >= 2)
			axis = 1;
		if (event.jaxis.axis != axis)
			return 0;
		SDL_Keycode key = (value < 0) ? SDLK_UP : SDLK_DOWN;
		event.type = SDL_KEYDOWN;
		event.key.keysym.sym = key;
	}else if (event.type == SDL_JOYBUTTONDOWN){
		if (!event.jbutton.state || event.jbutton.button >= 5)
			return 0;
		static SDL_Keycode buttons[] = {
			SDLK_RETURN,
			SDLK_ESCAPE,
			SDLK_PERIOD,
			SDLK_f,
			SDLK_F12
		};
		SDL_Keycode key = buttons[event.jbutton.button];
		event.type = SDL_KEYDOWN;
		event.key.keysym.sym = key;
	}
	return 1;
}

void UserInput::setup_joysticks(){
	this->free_joysticks();
	int n = SDL_NumJoysticks();
	for (int a = 0; a < n; a++){
		SDL_Joystick *j = SDL_JoystickOpen(a);
		if (j && (SDL_JoystickNumAxes(j) < 1 || SDL_JoystickNumButtons(j) < 2)){
			SDL_JoystickClose(j);
			j = 0;
		}
		this->joysticks.push_back(j);
	}
	SDL_JoystickEventState(SDL_ENABLE);
}

void UserInput::free_joysticks(){
	for (auto j : this->joysticks)
		SDL_JoystickClose(j);
	this->joysticks.clear();
}
