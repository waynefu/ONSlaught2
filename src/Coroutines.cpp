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
#include "coroutines.h"

using ONSlaught::InterCoroutineProcedureCallList;
using ONSlaught::InterCoroutineProcedureCall;
using ONSlaught::CoroutineMetaList;
using ONSlaught::InternalErrorException;
using ONSlaught::Coroutine;
using ONSlaught::CoroutineTask;
using ONSlaught::coroutine_entry_point_t;
using ONSlaught::coroutine_t;

#if defined USE_BOOST_COROUTINES
#include "boost_coroutines.cpp"
#endif

InterCoroutineProcedureCallList::~InterCoroutineProcedureCallList(){
	InterCoroutineProcedureCall *p;
	while (p = this->pop())
		delete p;
}

InterCoroutineProcedureCall *InterCoroutineProcedureCallList::pop(){
	if (!this->head)
		return 0;
	auto ret = this->head;
	if (this->head == this->tail){
		this->head = this->tail = 0;
		return ret;
	}
	this->head = this->head->next;
	return ret;
}

void InterCoroutineProcedureCallList::push(InterCoroutineProcedureCall *p){
	if (!this->head)
		this->head = this->tail = p;
	else{
		this->tail->next = p;
		this->tail = p;
	}
}

CoroutineMetaList::~CoroutineMetaList(){
	while (this->head)
		this->pop_list();
}

void CoroutineMetaList::push_list(){
	auto new_list = new InterCoroutineProcedureCallList;
	new_list->next_list = this->head;
	this->head = new_list;
}

class EmptyCoroutineMetaListException : public InternalErrorException{
public:
	const char *what() const{
		return "Illegal operation on empty metalist.";
	}
};

void CoroutineMetaList::pop_list(){
	if (!this->head)
		throw EmptyCoroutineMetaListException();
	auto new_head = this->head->next_list;
	delete this->head;
	this->head = new_head;
}

void CoroutineMetaList::push(InterCoroutineProcedureCall *p){
	if (!this->head)
		throw EmptyCoroutineMetaListException();
	this->head->push(p);
}

Coroutine::Coroutine(CoroutineTask *task):
		task(task),
		dirty(0),
		terminate(0),
		exception_thrown(0){
	this->mode = OopFunction;
	this->coroutine = coroutine_create(static_internal_callback, this);
}

Coroutine::Coroutine(function_t f):
		function(f),
		dirty(0),
		terminate(0),
		exception_thrown(0){
	this->mode = NormalFunction;
	this->coroutine = coroutine_create(static_internal_callback, this);
}

Coroutine::~Coroutine(){
	if (this->dirty){
		//We want to unwind the stack to give user code a chance to clean up before destructing.
		this->terminate = 1;
		coroutine_resume(this->coroutine);
	}
	coroutine_destroy(this->coroutine);
}

void Coroutine::static_internal_callback(void *user_data){
	Coroutine *_this = (Coroutine *)user_data;

	_this->dirty = 1;
	try{
		_this->perform_procedure_calls();
		if (_this->mode == NormalFunction)
			_this->function();
		else
			_this->task->coroutine_entry_point();
	}catch (TransferrableException &te){
		_this->exception_thrown = 1;
		_this->ex.reset(new TransferrableException(te));
	}catch (CoroutineExitException &){
	}catch (...){
		_this->exception_thrown = 1;
	}
	_this->dirty = 0;
}
