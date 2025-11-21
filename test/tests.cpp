#include <gtest/gtest.h>
#include <memory_resource>

#include "../include/CustomMemoryResource.hpp"
#include "../include/ForwardList.hpp"

using IntAllocator = std::pmr::polymorphic_allocator<int>;
using IntList = ForwardList<int, IntAllocator>;

static bool EqualLists(IntList& a, IntList& b) {
    if (a.size() != b.size()) return false;

    auto it1 = a.begin();
    auto it2 = b.begin();
    for (; it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
        if (*it1 != *it2) return false;
    }
    return it1 == a.end() && it2 == b.end();
}

TEST(test_01, EmptyConstructorAndSize) {
    CustomMemoryResource resource;
    IntAllocator alloc(&resource);

    IntList list(alloc);

    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), static_cast<std::size_t>(0));
}

TEST(test_02, SizedConstructorDefaultValues) {
    CustomMemoryResource resource;
    IntAllocator alloc(&resource);

    IntList list(5, alloc);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), static_cast<std::size_t>(5));

    for (auto it = list.begin(); it != list.end(); ++it) {
        EXPECT_EQ(*it, 0);
    }
}

TEST(test_03, PushFrontAndFront) {
    CustomMemoryResource resource;
    IntAllocator alloc(&resource);

    IntList list(alloc);

    list.push_front(3);
    list.push_front(2);
    list.push_front(1);

    EXPECT_EQ(list.size(), static_cast<std::size_t>(3));
    EXPECT_EQ(list.front(), 1);

    auto it = list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, list.end());
}

TEST(test_04, PushBackAndIterationOrder) {
    CustomMemoryResource resource;
    IntAllocator alloc(&resource);

    IntList list(alloc);

    list.push_back(10);
    list.push_back(20);
    list.push_back(30);

    EXPECT_EQ(list.size(), static_cast<std::size_t>(3));
    EXPECT_EQ(list.front(), 10);

    int expected[] = {10, 20, 30};
    int index = 0;
    for (auto it = list.begin(); it != list.end(); ++it, ++index) {
        EXPECT_EQ(*it, expected[index]);
    }
    EXPECT_EQ(index, 3);
}

TEST(test_05, PopFrontAndExceptions) {
    CustomMemoryResource resource;
    IntAllocator alloc(&resource);

    IntList list(alloc);

    list.push_back(1);
    list.push_back(2);

    EXPECT_EQ(list.size(), static_cast<std::size_t>(2));
    EXPECT_EQ(list.front(), 1);

    list.pop_front();
    EXPECT_EQ(list.size(), static_cast<std::size_t>(1));
    EXPECT_EQ(list.front(), 2);

    list.pop_front();
    EXPECT_TRUE(list.empty());

    EXPECT_THROW(list.pop_front(), std::out_of_range);
    EXPECT_THROW(list.front(), std::out_of_range);
}

TEST(test_06, CopyConstructorDeepCopy) {
    CustomMemoryResource resource;
    IntAllocator alloc(&resource);

    IntList original(alloc);
    original.push_back(5);
    original.push_back(6);
    original.push_back(7);

    IntList copy(original);

    EXPECT_TRUE(EqualLists(original, copy));

    original.pop_front();
    original.push_back(100);

    EXPECT_FALSE(EqualLists(original, copy));
}

TEST(test_07, CopyAssignmentDeepCopy) {
    CustomMemoryResource resource;
    IntAllocator alloc(&resource);

    IntList a(alloc);
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);

    IntList b(alloc);
    b.push_back(10);
    b.push_back(20);

    b = a;

    EXPECT_TRUE(EqualLists(a, b));

    b.pop_front();
    b.push_back(42);

    EXPECT_FALSE(EqualLists(a, b));
}

TEST(test_08, MoveConstructorAndMoveAssignment) {
    CustomMemoryResource resource;
    IntAllocator alloc(&resource);

    IntList a(alloc);
    a.push_back(1);
    a.push_back(2);

    IntList b(std::move(a));
    EXPECT_EQ(b.size(), static_cast<std::size_t>(2));
    EXPECT_TRUE(a.empty());

    IntList c(alloc);
    c.push_back(100);
    c = std::move(b);

    EXPECT_EQ(c.size(), static_cast<std::size_t>(2));
    EXPECT_TRUE(b.empty());
}

TEST(test_09, ManyListsWithSameResource) {
    CustomMemoryResource resource;
    IntAllocator alloc(&resource);

    IntList list1(alloc);
    IntList list2(alloc);
    IntList list3(alloc);

    for (int i = 0; i < 10; ++i) {
        list1.push_back(i);
        list2.push_back(i * 2);
        list3.push_back(i * 3);
    }

    EXPECT_EQ(list1.size(), static_cast<std::size_t>(10));
    EXPECT_EQ(list2.size(), static_cast<std::size_t>(10));
    EXPECT_EQ(list3.size(), static_cast<std::size_t>(10));

    int sum1 = 0, sum2 = 0, sum3 = 0;
    for (auto it = list1.begin(); it != list1.end(); ++it) sum1 += *it;
    for (auto it = list2.begin(); it != list2.end(); ++it) sum2 += *it;
    for (auto it = list3.begin(); it != list3.end(); ++it) sum3 += *it;

    EXPECT_GT(sum1, 0);
    EXPECT_GT(sum2, sum1);
    EXPECT_GT(sum3, sum2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
