#pragma once

#ifndef META_HPP_
#define META_HPP_

#include <type_traits>

namespace Meta
{
	template<template<typename...> class Template, typename T>
	struct is_instantiation_of : std::false_type {};

	template<template<typename...> class Template, typename... Args>
	struct is_instantiation_of<Template, Template<Args...>> : std::true_type {};

	template<typename T, typename U>
	struct transfer_qualifiers { using type = std::decay_t<U>; };

	template<typename T, typename U>
	struct transfer_qualifiers<const T&, U> { using type = const std::decay_t<U>&; };

	template<typename T, typename U>
	struct transfer_qualifiers<const T&&, U> { using type = const std::decay_t<U>&&; };

	template<typename T, typename U>
	struct transfer_qualifiers<T&, U> { using type = std::decay_t<U>&; };

	template<typename T, typename U>
	struct transfer_qualifiers<T&&, U> { using type = std::decay_t<U>&&; };


	template<typename T, typename U>
	using transfer_qualifiers_t = typename transfer_qualifiers<T, U>::type;

	template<template<typename...> class Template1, template<typename...> class Template2>
	struct is_same_template : std::false_type {};

	template<template<typename...> class Template>
	struct is_same_template<Template, Template> : std::true_type {}; 


	template<typename>
	constexpr bool dependent_false = false;

	template<typename... args>
	struct conjunction : std::true_type {};

	template<typename head, typename... tail>
	struct conjunction<head, tail...> : 
		std::integral_constant<bool, head::value && conjunction<tail...>::value> {};

	struct Unit {};

}

#endif // META_HPP_
