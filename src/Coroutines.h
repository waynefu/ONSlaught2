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

#ifndef COROUTINES_H
#define COROUTINES_H

namespace ONSlaught{

#if defined USE_BOOST_COROUTINES
typedef void *coroutine_t;
#endif

typedef void (*coroutine_entry_point_t)(void *);

extern "C" coroutine_t coroutine_create(coroutine_entry_point_t function, void *data);
// Returns non-zero if the target function (the first parameter to
// coroutine_create) hasn't returned. In other words,
// while (coroutine_resume(c));
// fully executes the function.
extern "C" int coroutine_resume(coroutine_t);
extern "C" void coroutine_yield(coroutine_t);
extern "C" void coroutine_destroy(coroutine_t);

//Exception class that can be transferred across coroutine boundaries.
//Other classes are non-transferrable.
//Subclasses must be copiable.
class TransferrableException : public std::exception{
public:
	virtual ~TransferrableException(){}
};

class InternalErrorException{};
class CoroutineExitException{};
class OperationCrossesCoroutineBoundaryException{};

class CoroutineTask{
public:
	virtual ~CoroutineTask(){}
	virtual void coroutine_entry_point() = 0;
};

class InterCoroutineProcedureCall{
	friend class InterCoroutineProcedureCallList;
	InterCoroutineProcedureCall *next;
public:
	virtual ~InterCoroutineProcedureCall(){}
	virtual void procedure_call() = 0;
};

class InterCoroutineProcedureCallList{
	friend class CoroutineMetaList;
	InterCoroutineProcedureCall *head, *tail;
	InterCoroutineProcedureCallList *next_list;
	InterCoroutineProcedureCallList(const InterCoroutineProcedureCallList &){}
	void operator=(const InterCoroutineProcedureCallList &){}
public:
	InterCoroutineProcedureCallList(): head(0), tail(0), next_list(0){}
	~InterCoroutineProcedureCallList();
	void push(InterCoroutineProcedureCall *);
	InterCoroutineProcedureCall *pop();
};

class CoroutineMetaList{
	InterCoroutineProcedureCallList *head;
	CoroutineMetaList(const CoroutineMetaList &){}
	void operator=(const CoroutineMetaList &){}
public:
	CoroutineMetaList(): head(0){}
	~CoroutineMetaList();
	void push_list();
	void pop_list();
	void push(InterCoroutineProcedureCall *);
	InterCoroutineProcedureCall *pop(){
		if (!this->head)
			return 0;
		return this->head->pop();
	}
};

/*
 *
 * Rules for coroutines:
 *
 * 1. CoroutineExitException shall not be thrown by any user code. It is meant
 *    for internal use to unwind the coroutine stack prior to destruction, in
 *    cases when the Task instance goes out of scope before the coroutine
 *    returns without yielding.
 * 2. User code shall not catch instances of CoroutineExitException.
 * 3. User code shall catch all exceptions (that don't break any of the prior
 *    rules) before returning control to the Coroutine. Letting an exception
 *    propagate to the Coroutine has undefined behavior.
 * 4. OperationCrossesCoroutineBoundaryException is thrown when a stack-based
 *    control operation that would have crossed the coroutine boundary is
 *    detected. In the general case, it indicates that program state is no
 *    longer consistent. The program should terminate immediately upon catching
 *    it, or it should not be caught at all.
 * 5. longjmp()ing accross coroutine boundaries is forbidden, but cannot be
 *    detected, so be careful.
 *
 * Other notes:
 *
 * * It's safe to nest coroutines, as long as the following condition is met:
 *   Suppose a set of coroutines is nested as A(B(C)). Then A may resume to B
 *   and B to C, and C may yield to B and B to A. Other combinations are
 *   unsafe.
 *
 */
class Coroutine{
	friend class CoroutineManager;
	friend class ScopedCoroutineList;
public:
	typedef void (*function_t)();
private:
	coroutine_t coroutine;

	enum Mode{
		NormalFunction,
		OopFunction
	} mode;
	function_t function;
	CoroutineTask *task;

	bool dirty;
	bool terminate;
	bool exception_thrown;
	std::auto_ptr<TransferrableException> ex;
	static void static_internal_callback(void *);
	void handle_inner_exception(){
		if (this->exception_thrown){
			this->exception_thrown = 0;
			if (this->ex.get())
				throw *this->ex;
			throw OperationCrossesCoroutineBoundaryException();
		}
	}

	CoroutineMetaList metalist;

	void perform_procedure_calls(){
		InterCoroutineProcedureCall *p;
		while (p = this->metalist.pop()){
			p->procedure_call();
			delete p;
		}
	}

public:
	Coroutine(CoroutineTask *);
	Coroutine(function_t);
	~Coroutine();
	void operator()(){
		coroutine_resume(this->coroutine);
		this->handle_inner_exception();
	}
	void yield(){
		coroutine_yield(this->coroutine);
		if (this->terminate)
			throw CoroutineExitException();
		this->perform_procedure_calls();
	}
	bool has_returned() const{
		return !this->dirty;
	}
	void push_procedure_call(InterCoroutineProcedureCall *p){
		this->metalist.push(p);
	}
};

}
#endif
