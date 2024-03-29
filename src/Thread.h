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

#ifndef THREAD_H
#define THREAD_H

namespace ONSlaught{

#if defined USE_BOOST_MUTEX
class Mutex{
	boost::recursive_mutex m;
	Mutex(const Mutex &){}
	void operator=(const Mutex &){}
public:
	Mutex(){}
	void lock(){
		this->m.lock();
	}
	void unlock(){
		this->m.unlock();
	}
};

class AutoMutex{
	Mutex &m;
	AutoMutex(AutoMutex &m): m(m.m){}
	void operator=(const AutoMutex &){}
public:
	AutoMutex(Mutex &m): m(m){
		this->m.lock();
	}
	~AutoMutex(){
		this->m.unlock();
	}
};
#endif //defined USE_BOOST_MUTEX

struct EventImpl;

class Event{
	std::auto_ptr<EventImpl> impl;
public:
	Event();
	~Event();
	void set();
	void wait();
};

class CrossThreadAction{
	bool sync;
	Event function_finished_event;
	virtual void perform() = 0;
public:
	CrossThreadAction(bool sync): sync(sync){}
	virtual ~CrossThreadAction(){}
	void operator()(){
		this->perform();
		if (this->sync)
			this->function_finished_event.set();
	}
	void wait(){
		if (this->sync)
			this->function_finished_event.wait();
	}
};

class Thread{
#ifdef USE_BOOST_THREAD
	std::auto_ptr<boost::thread> thread;
#endif
public:
	typedef void (*thread_f)(void *);
	~Thread(){
		this->join();
	}
	void start(thread_f function, void *user_data);
	void join();
};

#ifdef USE_BOOST_ATOMIC
#define Atomic boost::atomic
#else
#error TODO: Implement me.
#endif

}

#endif
