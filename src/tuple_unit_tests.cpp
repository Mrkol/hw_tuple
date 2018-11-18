#include <iostream>
#include "gtest/gtest.h"
#include "tuple.hpp"

#define ASSERT_EQ_R(A, B) ASSERT_EQ(B, A)



template<typename T>
void static_assert_type(T&&)
{
    static_assert(std::is_same<T, int>::value && !std::is_same<T, int>::value,
    	"Compilation failed because you wanted to read the type. See below");
}

enum class LastAction
{
	Constructed, CopyConstructed, MoveConstructed, CopyAssigned, MoveAssigned
};

struct MoveTester
{
	LastAction action;

	MoveTester() : action(LastAction::Constructed) { }
	MoveTester(const MoveTester&) : action(LastAction::CopyConstructed) { }
	MoveTester(MoveTester&&) : action(LastAction::MoveConstructed) { }

	MoveTester& operator=(const MoveTester&) { action = LastAction::CopyAssigned; return *this; }
	MoveTester& operator=(MoveTester&&) { action = LastAction::MoveAssigned; return *this; }
};

TEST(TupleUnitTetsts, ConstructionTest)
{
	Tuple<int, MoveTester> ctorTest;
	ASSERT_EQ_R(get<1>(ctorTest).action, LastAction::Constructed);

	Tuple<int, MoveTester> cpyTest(ctorTest);
	ASSERT_EQ_R(get<1>(cpyTest).action, LastAction::CopyConstructed);

	Tuple<int, MoveTester> movTest(std::move(ctorTest));
	ASSERT_EQ_R(get<1>(movTest).action, LastAction::MoveConstructed);


	Tuple<int, int> tuple(1, 10);

	Tuple<int, int> other(tuple);
	ASSERT_EQ_R(get<0>(tuple), get<0>(other));
	ASSERT_EQ_R(get<1>(tuple), get<1>(other));

	ASSERT_TRUE(tuple == other);

	Tuple<int, int> third;
	third = tuple;
	ASSERT_EQ_R(get<0>(third), get<0>(tuple));
	ASSERT_EQ_R(get<1>(third), get<1>(tuple));

	ASSERT_TRUE(tuple == third);

	auto fourth = makeTuple(9, 10);
	ASSERT_EQ_R(get<0>(fourth), 9);
	ASSERT_EQ_R(get<1>(fourth), 10);

	ASSERT_TRUE(tuple != fourth);

	third.swap(fourth);
	ASSERT_EQ_R(get<0>(fourth), 1);
	ASSERT_EQ_R(get<1>(fourth), 10);
	ASSERT_EQ_R(get<0>(third), 9);
	ASSERT_EQ_R(get<1>(third), 10);
}

TEST(TupleUnitTetsts, GetterTest)
{
	Tuple<int, double, bool> tuple(10, 3.3, true);

	get<0>(tuple) = 3;
	ASSERT_EQ_R(get<0>(tuple), 3);
	get<2>(tuple) = false;
	ASSERT_EQ_R(get<2>(tuple), false);

	const Tuple<int, double> tuple2(5, 1.1);
	ASSERT_EQ_R(get<1>(tuple2), 1.1);

	Tuple<int, double, bool> tuple3(69, 42.0, false);
	get<bool>(tuple3) = true;
	ASSERT_EQ_R(get<bool>(tuple3), true);
	get<double>(tuple3) = 3;
	ASSERT_EQ_R(get<double>(tuple3), 3);

	Tuple<int, double> tuple4(1, 4.32);
	ASSERT_EQ_R(get<0>(tuple4), 1);
	ASSERT_EQ_R(get<1>(tuple4), 4.32);

}

TEST(TupleUnitTetsts, SizeTests)
{
	ASSERT_EQ_R(sizeof(Tuple<int>), 4);
	ASSERT_EQ_R(sizeof(Tuple<int, int>), 8);
	ASSERT_EQ_R(sizeof(Tuple<int, int, int>), 12);
	ASSERT_EQ_R(sizeof(Tuple<int, int, int, int>), 16);
}

TEST(TupleUnitTetsts, ConcatTest)
{
	Tuple<int, float, int> first(0, 1.0f, 2);
	Tuple<char, bool, MoveTester> second('k', true);

	Tuple<int, float, int, char, bool, MoveTester> concat;

	concat = tupleCat(first, second);

	ASSERT_EQ_R(get<0>(concat), 0);
	ASSERT_EQ_R(get<1>(concat), 1.0f);
	ASSERT_EQ_R(get<2>(concat), 2);
	ASSERT_EQ_R(get<3>(concat), 'k');
	ASSERT_EQ_R(get<4>(concat), true);
	ASSERT_EQ_R(get<5>(tupleCat(first, second)).action, LastAction::CopyAssigned);
	//^^^ not copy constructed due to C++ being an awesome language -_-

	Tuple<int*> third(nullptr);
	auto moveConcat = tupleCat(std::move(first), std::move(second), std::move(third));

	ASSERT_EQ_R(get<0>(moveConcat), 0);
	ASSERT_EQ_R(get<1>(moveConcat), 1.0f);
	ASSERT_EQ_R(get<2>(moveConcat), 2);
	ASSERT_EQ_R(get<3>(moveConcat), 'k');
	ASSERT_EQ_R(get<4>(moveConcat), true);
	ASSERT_EQ_R(get<5>(moveConcat).action, LastAction::MoveAssigned);
	ASSERT_EQ_R(get<6>(moveConcat), nullptr);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
