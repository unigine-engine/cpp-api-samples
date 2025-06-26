#pragma once
#include <UnigineEvent.h>
#include <memory>
#include <type_traits>

// Simple helper for add task to events
// addTask requires function to return to bool castable type, will execute until function return false
// addSingleshotTask Unity do once analogue
// Requires to callables same as in Unigine::Event
// Usage samples:
//
// Printing counter on end world update until counter less than 100
//
// int counter = 0;
// Utils::Task<>::addTask(Unigine::Engine::get()->getEventEndWorldUpdate(), Utils::mutLambda([counter] mutable {
//     Unigine::Log::message("Counter is %i\n", counter);
//	   return ++counter < 100;
// }));

namespace Utils
{

template <class L, class... Args>
struct is_lambda
{
	static constexpr bool value = Unigine::LambdaTraits<L>::arity == sizeof...(Args) && std::is_same<typename Unigine::LambdaTraits<L>::args, std::tuple<Args...>>::value;
};

template <class... Args>
class Task
{
public:
	template <class L, typename std::enable_if<is_lambda<L>::value, bool>::type = false>
	UNIGINE_INLINE static void addTask(Unigine::Event<Args...>& event, L f, Unigine::LambdaHelper<Args...> = {})
	{
		auto pConnection = new Unigine::EventConnection;
		event.connect(*pConnection, [f, pConnection](Args... args) {
			if (!static_cast<bool>(f(args...)))
				delete pConnection;
			});
	}

	template <class Ret, class... Extra, typename std::enable_if<std::is_convertible<Ret, bool>::value, bool>::type = false>
	UNIGINE_INLINE static void addTask(Unigine::Event<Args...>& event, Ret(*f)(Args..., Extra...), Extra... extra)
	{
		auto pConnection = new Unigine::EventConnection;
		event.connect(*pConnection, [f, pConnection, extra...](Args... args) {
			if (!static_cast<bool>(f(args..., extra...)))
				delete pConnection;
			});
	}

	template <class Class, class Ret, class... Extra, typename std::enable_if<std::is_convertible<Ret, bool>::value, bool>::type = false>
	UNIGINE_INLINE static void addTask(Unigine::Event<Args...>& event, Class* c, Ret(Class::* f)(Args..., Extra...), Extra... extra)
	{
		auto pConnection = new Unigine::EventConnection;
		event.connect(*pConnection, [c, f, pConnection, extra...](Args... args) {
			if (!static_cast<bool>((c->*f)(args..., extra...)))
				delete pConnection;
			});
	}

	template <class L, typename std::enable_if<is_lambda<L>::value, bool>::type = false>
	UNIGINE_INLINE static void addSingleshotTask(Unigine::Event<Args...>& event, L f, Unigine::LambdaHelper<Args...> = {})
	{
		auto pConnection = new Unigine::EventConnection;
		event.connect(*pConnection, [f, pConnection](Args... args) {
			f(args...);
			delete pConnection;
			});
	}

	template <class Ret, class... Extra>
	UNIGINE_INLINE static void addSingleshotTask(Unigine::Event<Args...>& event, Ret(*f)(Args..., Extra...), Extra... extra)
	{
		auto pConnection = new Unigine::EventConnection;
		event.connect(*pConnection, [f, pConnection, extra...](Args... args) {
			f(args..., extra...);
			delete pConnection;
			});
	}

	template <class Class, class Ret, class... Extra>
	UNIGINE_INLINE static void addSingleshotTask(Unigine::Event<Args...>& event, Class* c, Ret(Class::* f)(Args..., Extra...), Extra... extra)
	{
		auto pConnection = new Unigine::EventConnection;
		event.connect(*pConnection, [c, f, pConnection, extra...](Args... args) {
			(c->*f)(args..., extra...);
			delete pConnection;
			});
	}
};

}
