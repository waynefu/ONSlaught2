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
