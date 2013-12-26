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

#ifndef UTIL_H
#define UTIL_H

namespace ONSlaught{

template <typename T>
class auto_array_ptr{
public:
	typedef void (*deallocation_fp)(void *);
private:
	T *pointer;
public:
	auto_array_ptr(T *p = 0): pointer(p){}
	auto_array_ptr(auto_array_ptr<T> &o): pointer(o.release()){}
	~auto_array_ptr(){
		this->reset();
	}
	void set_deallocation_function(deallocation_fp f){
		this->func = f;
	}
	const auto_array_ptr<T> &operator=(auto_array_ptr<T> &o){
		this->reset(o.release());
		return *this;
	}
	T *release(){
		T *ret = pointer;
		this->pointer = 0;
		return ret;
	}
	void reset(T *p = 0){
		delete[] pointer;
		this->pointer = p;
	}
	operator bool() const{
		return !!this->pointer;
	}
	bool operator!() const{
		return !this->pointer;
	}
	T *get(){
		return this->pointer;
	}
	const T *get() const{
		return this->pointer;
	}
	T &operator*(){
		return *this->pointer;
	}
	const T &operator*() const{
		return *this->pointer;
	}
	T &operator[](size_t index){
		return this->pointer[index];
	}
	const T &operator[](size_t index) const{
		return this->pointer[index];
	}
};

}

#endif
