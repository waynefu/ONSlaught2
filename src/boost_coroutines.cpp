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

typedef boost::coroutines::coroutine<void()> task_t;
typedef task_t::caller_type caller_t;
typedef std::vector<caller_t *> stack_t;

boost::thread_specific_ptr<stack_t> stack;

struct coroutine_wrapper{
	task_t task;
	coroutine_entry_point_t function;
	void *user_data;
	bool only_once;
	coroutine_wrapper(coroutine_entry_point_t function, void *user_data):
		task(*this),
		function(function),
		user_data(user_data),
		only_once(1){}
	void operator()(caller_t &c){
		if (this->only_once){
			c();
			this->only_once = 0;
		}
		stack->push_back(&c);
		this->function(this->user_data);
		stack->pop_back();
	}
};

extern "C"{

coroutine_t coroutine_create(coroutine_entry_point_t function, void *data){
	if (!stack.get())
		stack.reset(new stack_t);
	return new coroutine_wrapper(function, data);
}

int coroutine_resume(coroutine_t c){
	auto &task = ((coroutine_wrapper *)c)->task;
	return (bool)task();
}

void coroutine_yield(coroutine_t){
	(*stack->back())();
}

void coroutine_destroy(coroutine_t c){
	delete (coroutine_wrapper *)c;
}

} //extern "C"
