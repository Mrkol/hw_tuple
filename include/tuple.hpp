#pragma once


#ifndef TUPLE_HPP_
#define TUPLE_HPP_

#include <type_traits>
#include "meta.hpp"



template<typename... Ts>
class Tuple
{

};

namespace TuplePrivate
{
	template<typename... Tuples>
	struct tuple_type_concat { using type = Tuple<>; };

	template<typename First>
	struct tuple_type_concat<First> { using type = First; };

	template<
		typename... FirstArgs,
		typename... SecondArgs,
		typename... Others
		>
	struct tuple_type_concat<Tuple<FirstArgs...>, Tuple<SecondArgs...>, Others...>
		: tuple_type_concat<Tuple<FirstArgs..., SecondArgs...>, Others...>
	{

	};

	template<typename... Tuples>
	using tuple_type_concat_t = typename tuple_type_concat<Tuples...>::type;


	//This ugly hack is needed cuz we have no constexpr if.
	template<
		typename WAssignee,
		typename... WOthers
		>
	struct ConcatAssignWrapper
	{
		template<
			typename... Others,
			typename = std::enable_if_t<
				Meta::conjunction<
					Meta::is_instantiation_of<Tuple, WAssignee>,
					Meta::is_instantiation_of<Tuple, WOthers>...,
					std::is_same<std::decay_t<Others>, WOthers>...
					>::value
				>
			>
		static void call(WAssignee& to, Others&&... others);
	};

	template<
		typename WAssignee,
		typename WFirst,
		typename... WOthers
		>
	struct ConcatAssignWrapper<WAssignee, WFirst, WOthers...>
	{
		template<
			typename First,
			typename... Others,
			typename = std::enable_if_t<
				Meta::conjunction<
					Meta::is_instantiation_of<Tuple, WAssignee>,
					Meta::is_instantiation_of<Tuple, WFirst>,
					Meta::is_instantiation_of<Tuple, WOthers>...,
					std::is_same<std::decay_t<First>, WFirst>,
					std::is_same<std::decay_t<Others>, WOthers>...
					>::value
				>
			>
		static void call(WAssignee& to, First&& first, Others&&... others);
	};

	template<
		typename WAssignee,
		typename... WOthers
		>
	struct ConcatAssignWrapper<WAssignee, Tuple<>, WOthers...>
	{
		template<
			typename First,
			typename... Others,
			typename = std::enable_if_t<
				Meta::conjunction<
					Meta::is_instantiation_of<Tuple, WAssignee>,
					Meta::is_instantiation_of<Tuple, WOthers>...,
					std::is_same<std::decay_t<First>, Tuple<>>,
					std::is_same<std::decay_t<Others>, WOthers>...
					>::value
				>
			>
		static void call(WAssignee& to, First&& first, Others&&... others);
	};
}

template<
	typename... Tuples,
	typename = std::enable_if_t<
		Meta::conjunction<
			Meta::is_instantiation_of<Tuple, std::decay_t<Tuples>>...
			>::value
		>
	>
TuplePrivate::tuple_type_concat_t<std::decay_t<Tuples>...> tupleCat(Tuples&&... others);


template<
	std::size_t index,
	typename T,
	typename = std::enable_if_t<Meta::is_instantiation_of<Tuple, std::decay_t<T>>::value>
	>
auto get(T&& tuple)
	-> typename std::decay_t<T>::template
		IthParent<index>::template
			QualifiedType<decltype(tuple)>;

template<
	typename U,
	typename T,
	typename = std::enable_if_t<Meta::is_instantiation_of<Tuple, std::decay_t<T>>::value>
	>
auto get(T&& tuple)
	-> typename std::decay_t<T>::template
		ParentWithHead<U>::template
			QualifiedType<decltype(tuple)>;

template<typename... Args>
Tuple<std::decay_t<Args>...> makeTuple(Args&&... args);

template<
	typename First,
	typename Second,
	typename = std::enable_if_t<Meta::is_instantiation_of<Tuple, First>::value>,
	typename = std::enable_if_t<Meta::is_instantiation_of<Tuple, Second>::value>
	>
bool operator==(const First& first, const Second& second);

template<
	typename First,
	typename Second,
	typename = std::enable_if_t<Meta::is_instantiation_of<Tuple, First>::value>,
	typename = std::enable_if_t<Meta::is_instantiation_of<Tuple, Second>::value>
	>
bool operator!=(const First& first, const Second& second);

template<>
class Tuple<>
{
	using Me = Tuple<>;
	using Parent = Tuple<>;
	using ValueType = Meta::Unit;

	template<typename U>
	using QualifiedType = Meta::Unit;

	template<std::size_t index, typename = void>
	struct IthParent_ 
	{
		static_assert(index < 0, "Tuple index out of range!");
		using type = Me;
	};

	template<typename T, typename = void>
	struct ParentWithHead_
	{
		static_assert(Meta::dependent_false<T>, "No element of such type in this tuple!");
		using type = Me;
	};

	template<typename...>
	friend class Tuple;

	template<typename, typename...>
	friend struct TuplePrivate::ConcatAssignWrapper;

	template<std::size_t index, typename T, typename>
	friend auto get(T&& tuple)
		-> typename std::decay_t<T>::template
			IthParent<index>::template
				QualifiedType<decltype(tuple)>;

	template<typename U, typename T, typename>
	friend auto get(T&& tuple)
		-> typename std::decay_t<T>::template
			ParentWithHead<U>::template
				QualifiedType<decltype(tuple)>;

public:
	Tuple() = default;
	Tuple(const Me& other) = default;
	Tuple(Me&& other) = default;
	Me& operator=(const Me& other) = default;
	Me& operator=(Me&& other) = default;
};


template<
	typename Head,
	typename... Tail
	>
class Tuple<Head, Tail...> : Tuple<Tail...>
{
	using ValueType = Head;
	template<typename U>
	using QualifiedType = Meta::transfer_qualifiers_t<U, ValueType>;
	using Parent = Tuple<Tail...>;
	using Me = Tuple<Head, Tail...>;

	template<std::size_t index, typename = void>
	struct IthParent_
	{
		using type = typename Parent::template IthParent_<index - 1>::type;
	};

	// full specialization is not allowed
	// hence the dummy variable to make it a partial one
	template<typename DUMMY>
	struct IthParent_<0, DUMMY>
	{
		using type = Me;
	};

	template<typename T, typename = void>
	struct ParentWithHead_
	{
		using type = typename Parent::template ParentWithHead_<T>::type;
	};

	template<typename DUMMY>
	struct ParentWithHead_<Head, DUMMY>
	{
		using type = Me;
	};

	template<std::size_t index>
	using IthParent = typename IthParent_<index>::type;

	template<typename T>
	using ParentWithHead = typename ParentWithHead_<T>::type;

	template<typename...>
	friend class Tuple;

	template<typename, typename...>
	friend struct TuplePrivate::ConcatAssignWrapper;

	template<std::size_t index, typename T, typename>
	friend auto get(T&& tuple)
		-> typename std::decay_t<T>::template
			IthParent<index>::template
				QualifiedType<decltype(tuple)>;

	template<typename U, typename T, typename>
	friend auto get(T&& tuple)
		-> typename std::decay_t<T>::template
			ParentWithHead<U>::template
				QualifiedType<decltype(tuple)>;

	template<typename First, typename Second, typename, typename>
	friend bool operator==(const First& first, const Second& second);


public:
	Tuple() = default;

	template<
		typename T,
		typename... Ts,
		typename = std::enable_if_t<std::is_convertible<T, ValueType>::value>
		>
	explicit Tuple(T&& head, Ts&&... tail);

	Tuple(const Me& other) = default;
	Tuple(Me&& other) = default;
	Me& operator=(const Me& other) = default;
	Me& operator=(Me&& other) = default;

	void swap(Me& other);

private:
	Head value_;
};

#include "tuple.tpp"
#endif //TUPLE_HPP_
