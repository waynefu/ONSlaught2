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

#ifdef _MSC_VER
#pragma once
#endif

#ifndef KERNEL_H
#define KERNEL_H

#include "Thread.h"
#include "UserInput.h"

namespace ONSlaught{

class Video;
class Audio;
class UserInput;
class ScriptInterpreter;

class CrossThreadAction;
class InteractiveObject;

class InteractionDelegate{
	InteractiveObject *io;
	typedef bool (InteractiveObject::*function_t)(const event_queue_t &);
	function_t f;
public:
	InteractionDelegate(InteractiveObject *io, function_t f): io(io), f(f){}
	bool operator()(const event_queue_t &q){
		return (this->io->*f)(q);
	}
};

class Kernel{
	std::auto_ptr<Video> video;
	std::auto_ptr<Audio> audio;
	std::auto_ptr<UserInput> input;
	std::auto_ptr<ScriptInterpreter> interpreter;
	std::list<InteractionDelegate> delegates;
	std::deque<CrossThreadAction *> actions;
	Mutex actions_mutex,
		delegates_mutex;

	void perform_actions();
	void run_delegates(const event_queue_t &);
public:
	Kernel();
	void run();
	void schedule(CrossThreadAction *);
};

extern std::auto_ptr<Kernel> global_kernel;

}

#endif
