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
#include "Thread.h"

#define NOTIFY_ONE 0

using ONSlaught::Event;
using ONSlaught::EventImpl;

#if defined USE_BOOST_MUTEX
template <typename T>
class BoostEvent{
	bool ready;
	T var;
	boost::condition_variable cv;
	boost::mutex mutex;
public:
	BoostEvent(): ready(0){}
	void set(const T &var){
		boost::unique_lock<boost::mutex> lock(this->mutex);
		this->var = var;
		this->ready = 1;
#if NOTIFY_ONE
		this->cv.notify_one();
#else
		this->cv.notify_all();
#endif
	}
	const T &wait(){
		boost::unique_lock<boost::mutex> lock(this->mutex);
		while (!this->ready)
			this->cv.wait(lock);
		this->ready = 0;
		return this->var;
	}
};

struct EventImpl{
	BoostEvent<bool> event;
};

Event::Event(){
	this->impl = new EventImpl;
}

Event::~Event(){
	delete this->impl;
}

void Event::set(){
	this->impl->event.set(1);
}

void Event::wait(){
	this->impl->event.wait();
}
#endif
