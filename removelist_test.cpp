#include <gtest/gtest.h>
#include "RemoveList.h"

using namespace J;

TEST(RemoveList, RemoveList_Basic)
{

	RemoveList<char, true> removeList(3);

	removeList.remove_if([](const char &c)
						{ return true; });
	EXPECT_EQ(removeList.size(), 0);
	EXPECT_GE(removeList.capacity(), 0);
	removeList.for_each([](const char &c) { throw "iterating over empty remove list"; });
	EXPECT_EQ(removeList.to_array(), nullptr);

	removeList.remove_if([](const char &c) { throw "iterating over empty remove list"; return false; });

	removeList.add('a');
	removeList.add('b');
	removeList.add('c');
	EXPECT_EQ(removeList.size(), 3);
	EXPECT_GE(removeList.capacity(), 3);
}

TEST(RemoveList, RemoveList_Content)
{

	constexpr std::size_t numberOfEntries = 10'000;

	RemoveList<std::size_t, true> removeList(numberOfEntries);
	EXPECT_EQ(removeList.size(), 0);
	EXPECT_GE(removeList.capacity(), numberOfEntries);
	for (std::size_t i = 0; i < numberOfEntries; i++)
	{
		removeList.add(i);
	}
	EXPECT_EQ(removeList.size(), numberOfEntries);
	EXPECT_GE(removeList.capacity(), numberOfEntries);
	std::size_t i = 0;
	removeList.for_each([&](const std::size_t &c) { 
		EXPECT_EQ(c, i++); 
		});
	removeList.remove_if([](const std::size_t &c) { 
		return c % 2; 
		});
	EXPECT_EQ(removeList.size(), numberOfEntries / 2);
	EXPECT_GE(removeList.capacity(), numberOfEntries / 2);
	i = 0;
	removeList.for_each([&](const std::size_t &c) {
				EXPECT_EQ(c, i);
				i += 2; 
			});
	removeList.remove_if([](const std::size_t &c)
						{ return !(c % 2); });
	EXPECT_EQ(removeList.size(), 0);
	EXPECT_GE(removeList.capacity(), 0);
}