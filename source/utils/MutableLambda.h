#pragma once

// Simple helper for use mutable lambdas with events
// Usage sample:
// int counter = 0;
// Unigine::EventConnection connection;
// Unigine::Engine::get()->getEventEndWorldUpdate()-connect(connection, Utils::mutableLambda([counter] mutable {
//     Unigine::Log::message("Counter is %i\n", counter++);
// }));

namespace Utils
{

template <class F, class... Args>
class MutableLambdaWrapper
{
public:
	MutableLambdaWrapper(F f)
		: _f(f)
	{
	}

	auto operator()(Args... args) const
	{
		return _f(args...);
	}

private:
	mutable F _f;
};

template <class F, class... Args>
auto mutableLambda(F f)
{
	return MutableLambdaWrapper<F, Args...>(f);
}

}
