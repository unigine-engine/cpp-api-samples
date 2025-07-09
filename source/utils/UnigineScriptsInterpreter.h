#pragma once
#include <UnigineEngine.h>
#include <UnigineInterpreter.h>

#include <functional>

const static char *sourse_str = "From [C++]:";

// We use this way to add extern functions and classes (instead of just adding them in Components
// init() methods inside samples code) because those functions and classes in order to be used in
// editor, system and world scripts must be added before initialization of editor, system and
// world logic respectively (and Components init() called after WorldLogic::init()).
// As result we use InterpreterRegistrator::initialize() method after engine init but before
// entering engine main loop (see main.cpp)

// Singleton which on initialization invokes signal that notifies it's time to add all extern
// functions and classes to UnigineScript
class InterpreterRegistrator
{
public:
	static InterpreterRegistrator *get();
	~InterpreterRegistrator() {}

	void initialize();
	Unigine::Event<> &getEventInit() { return invoker; }

private:
	InterpreterRegistrator() {};

private:
	Unigine::EventInvoker<> invoker;
};

// class used to collect callbacks responsible for adding extern user functions and classes to
// UnigineScript on InterpreterRegistrator init
struct USCInterpreter
{
public:
	USCInterpreter(std::function<void()> func)
	{
		InterpreterRegistrator::get()->getEventInit().connect(connection, func);
	}
	~USCInterpreter() { connection.disconnect(); }

private:
	Unigine::EventConnection connection;
};
