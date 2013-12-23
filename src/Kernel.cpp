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
#include "Video.h"
#include "Kernel.h"

using ONSlaught::Kernel;

std::auto_ptr<ONSlaught::Kernel> ONSlaught::global_kernel;

Kernel::Kernel(){
}

void Kernel::perform_actions(){
	AutoMutex am(this->actions_mutex);
	while (this->actions.size()){
		(*this->actions.front())();
		this->actions.pop_front();
	}
}

void Kernel::run_delegates(const ONSlaught::event_queue_t &input){
	AutoMutex am(this->delegates_mutex);
	for (auto i = this->delegates.begin(); i != this->delegates.end();){
		if (!(*i)(input)){
			auto copy = i++;
			this->delegates.erase(copy);
		}else
			++i;
	}
}

void Kernel::run(){
	while (1){
		this->perform_actions();
		this->run_delegates(this->input->get());
		this->video->draw();
	}
}

void Kernel::schedule(ONSlaught::CrossThreadAction *cta){
	{
		AutoMutex am(this->actions_mutex);
		this->actions.push_back(cta);
	}
	cta->wait();
}
