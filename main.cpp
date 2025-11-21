#include <iostream>
#include <memory_resource>

#include "CustomMemoryResource.hpp"
#include "ForwardList.hpp"

int main() {
    std::cout << "Создание пользовательского memory_resource:\n" << std::endl;
    CustomMemoryResource custom_memory_resource;
    std::pmr::polymorphic_allocator<int> int_allocator(&custom_memory_resource);
    std::cout << "Полиморфный аллокатор для int создан\n" << std::endl;

    std::cout << "Создание списка с пользовательской структурой...\n" << std::endl;

    struct TestStructure {
        int first_value;
        int second_value;
    };

    std::pmr::polymorphic_allocator<TestStructure> struct_allocator(&custom_memory_resource);
    ForwardList<TestStructure, std::pmr::polymorphic_allocator<TestStructure>>
        struct_list(struct_allocator);

    struct_list.push_back({1, 10});
    struct_list.push_back({2, 20});
    struct_list.push_back({3, 30});

    std::cout << "Список структур создан и заполнен:" << std::endl;
    int index = 0;
    for (auto it = struct_list.begin(); it != struct_list.end(); ++it) {
        std::cout << "     [" << index++ << "]: { "
                  << it->first_value << ", " << it->second_value << " }" << std::endl;
    }

    std::cout << "\nСоздание нескольких списков:" << std::endl;

    ForwardList<int, std::pmr::polymorphic_allocator<int>> *list1, *list2, *list3, *list4;

    list1 = new ForwardList<int, std::pmr::polymorphic_allocator<int>>(int_allocator);
    list2 = new ForwardList<int, std::pmr::polymorphic_allocator<int>>(int_allocator);
    list3 = new ForwardList<int, std::pmr::polymorphic_allocator<int>>(int_allocator);
    list4 = new ForwardList<int, std::pmr::polymorphic_allocator<int>>(int_allocator);

    for (int i = 0; i < 5; ++i) {
        list1->push_back(i);
        list2->push_back(i * 10);
    }

    auto it = list1->begin();
    std::advance(it, 1);
    *it = 91293;

    std::cout << "Четыре списка созданы" << std::endl;

    std::cout << "Содержимое list1: ";
    for (auto it = list1->begin(); it != list1->end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Содержимое list2: ";
    for (auto it = list2->begin(); it != list2->end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Освобождение части списков:" << std::endl;
    delete list2;
    delete list3;
    std::cout << "Два списка освобождены" << std::endl;

    std::cout << "Циклическое создание и использование списков:" << std::endl;

    for (int iteration = 0; iteration < 10; ++iteration) {
        std::cout << "   Итерация " << iteration << ":" << std::endl;

        ForwardList<int, std::pmr::polymorphic_allocator<int>> temp_list(int_allocator);
        ForwardList<int, std::pmr::polymorphic_allocator<int>> large_list(int_allocator);

        for (int value = 0; value < 10; ++value) {
            temp_list.push_back(value);
        }

        for (int value = 0; value < 40; ++value) {
            large_list.push_back(value * 2);
        }

        std::cout << "     Содержимое temp_list: ";
        for (auto it = temp_list.begin(); it != temp_list.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
        std::cout << "     Содержимое large_list: ";
        for (auto it = large_list.begin(); it != large_list.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;

    }

    std::cout << "\n6. Освобождение оставшихся списков:" << std::endl;
    delete list1;
    delete list4;
    std::cout << "   Все списки освобождены" << std::endl;

    return 0;
}
