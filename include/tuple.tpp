#include "tuple.hpp"


template<typename... Args>
Tuple<std::decay_t<Args>...> makeTuple(Args&&... args)
{
	return Tuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
}

template<typename Head, typename... Tail>
template<typename T, typename... Ts, typename>
Tuple<Head, Tail...>::Tuple(T&& head, Ts&&... tail)
	: Parent(std::forward<Ts>(tail)...),
	value_(std::forward<Head>(head))
{
	
}

template<typename Head, typename... Tail>
void Tuple<Head, Tail...>::swap(Me& other)
{
	Me tmp(std::move(other));
	other = std::move(*this);
	*this = std::move(tmp);
}

template<std::size_t index, typename T, typename>
auto get(T&& tuple)
	-> typename std::decay_t<T>::template
			IthParent<index>::template
				QualifiedType<decltype(tuple)>
{
	return
		static_cast<
			Meta::transfer_qualifiers_t<
				decltype(tuple),
				typename std::decay_t<T>::template IthParent<index>
				>
			>
			(tuple).value_;
}

template<typename U, typename T, typename>
auto get(T&& tuple)
	-> typename std::decay_t<T>::template
		ParentWithHead<U>::template
			QualifiedType<decltype(tuple)>
{
	return
		static_cast<
			Meta::transfer_qualifiers_t<
				decltype(tuple),
				typename std::decay_t<T>::template ParentWithHead<U>
				>
			>
			(tuple).value_;
}

template<typename... Tuples, typename>
TuplePrivate::tuple_type_concat_t<std::decay_t<Tuples>...> tupleCat(Tuples&&... tuples)
{
	decltype(tupleCat(tuples...)) result;
	TuplePrivate::ConcatAssignWrapper<decltype(tupleCat(tuples...)), std::decay_t<Tuples>...>::
		call(result, std::forward<Tuples>(tuples)...);
	return result;
}

namespace TuplePrivate
{
	template<typename WAssignee, typename... WOthers>
	template<typename... Others, typename>
	void ConcatAssignWrapper<WAssignee, WOthers...>::
		call(WAssignee& to, Others&&... others)
	{

	}


	template<typename WAssignee, typename WFirst, typename... WOthers>
	template<typename First, typename... Others, typename>
	void ConcatAssignWrapper<WAssignee, WFirst, WOthers...>::
		call(WAssignee& to, First&& first, Others&&... others)
	{
		if (std::is_lvalue_reference<decltype(first)>::value)
			to.value_ = first.value_;
		else
			to.value_ = std::move(first.value_);

		ConcatAssignWrapper<typename WAssignee::Parent, typename WFirst::Parent, WOthers...>::
			call(
			static_cast<typename WAssignee::Parent&>(to),
			static_cast<Meta::transfer_qualifiers_t<decltype(first), typename WFirst::Parent>>(first), 
			std::forward<Others>(others)...
			);
	}

	template<typename WAssignee, typename... WOthers>
	template<typename First, typename... Others, typename>
	void ConcatAssignWrapper<WAssignee, Tuple<>, WOthers...>::
		call(WAssignee& to, First&& first, Others&&... others)
	{
		ConcatAssignWrapper<WAssignee, WOthers...>::
			call(to, std::forward<Others>(others)...);
	}
}

template<typename First, typename Second, typename, typename>
bool operator==(const First& first, const Second& second)
{
	return first.value_ == second.value_ && 
		(static_cast<const typename First::Parent&>(first)
			==
		static_cast<const typename Second::Parent&>(second));
}

template<>
bool operator==<Tuple<>, Tuple<>>(const Tuple<>& first, const Tuple<>& second)
{
	return true;
}

template<typename First, typename Second, typename, typename>
bool operator!=(const First& first, const Second& second)
{
	return !(first == second);
}
