#pragma once

#include <memory_resource>
#include <memory>
#include <concepts>
#include <stdexcept>
#include <iterator>

template <class T, class allocator_type>
requires std::is_default_constructible_v<T> &&
             std::is_same_v<allocator_type, std::pmr::polymorphic_allocator<T>>
class ForwardList {
private:
    struct Node {
        T value;
        Node* next;

        Node(const T& v, Node* n = nullptr): value(v), next(n) {}

        Node(): value(), next(nullptr) {}
    };

    using node_allocator_type = std::pmr::polymorphic_allocator<Node>;

    node_allocator_type node_allocator;
    Node* head;
    std::size_t list_size;

public:
    class ForwardListIterator {
    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference_type = T&;
        using pointer_type = T*;
        using iterator_category = std::forward_iterator_tag;

        ForwardListIterator() : current(nullptr) {}
        
        ForwardListIterator& operator++() {
            current = current->next;
            return *this;
        };

        ForwardListIterator operator++(int) {
            ForwardListIterator tmp = *this;
            current = current->next;
            return tmp;
        };
    
        inline reference_type operator*() const {
            return current->value;
        };

        inline pointer_type operator->() const {
            return &current->value;;
        };

        inline bool operator==(const ForwardListIterator& other) const {
            return current == other.current;
        }
    
        inline bool operator!=(const ForwardListIterator& other) const {
            return current != other.current;
        }
    private:
        explicit ForwardListIterator(Node* node) {
            current = node;
        }

        Node* current;

        friend class ForwardList;
    };

public:
    explicit ForwardList(allocator_type alloc = {}): node_allocator(alloc.resource()), head(nullptr), list_size(0) {}

    ForwardList(std::size_t count, allocator_type alloc = {}): node_allocator(alloc.resource()), head(nullptr), list_size(0) {
        for (std::size_t i = 0; i < count; ++i) {
            push_back(T{});
        }
    }

    ForwardList(const ForwardList& other): node_allocator(other.node_allocator), head(nullptr), list_size(0) {
        if (!other.head) return;
        Node* ptr = other.head;
        while (ptr) {
            push_back(ptr->value);
            ptr = ptr->next;
        }
    }

    ForwardList& operator=(const ForwardList& other) {
        if (this != &other) {
            clear();
            ForwardList tmp(other);
            std::swap(head, tmp.head);
            std::swap(list_size, tmp.list_size);
        }
        return *this;
    }

    ForwardList(ForwardList&& other) noexcept: node_allocator(std::move(other.node_allocator)), head(other.head), list_size(other.list_size) {
        other.head = nullptr;
        other.list_size = 0;
    }

    ForwardList& operator=(ForwardList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            list_size = other.list_size;
            other.head = nullptr;
            other.list_size = 0;
        }
        return *this;
    }

    /**
     * Вставка элемента в начало списка
     *
     */
    void push_front(const T& value) {
        Node* new_node = node_allocator.allocate(1);
        std::allocator_traits<node_allocator_type>::construct(node_allocator, new_node, value, head);
        head = new_node;
        ++list_size;
    }

    /**
     * Вставка элемента в конец списка
     */
    void push_back(const T& value) {
        Node* new_node = node_allocator.allocate(1);
        std::allocator_traits<node_allocator_type>::construct(node_allocator, new_node, value, nullptr);

        if (!head) {
            head = new_node;
        } else {
            Node* current = head;
            while (current->next) current = current->next;
            current->next = new_node;
        }
        ++list_size;
    }

    /**
     * Удаление элемента из начала списка
     */
    void pop_front() {
        if (!head) throw std::out_of_range("List is empty");

        Node* tmp = head;
        head = head->next;

        std::allocator_traits<node_allocator_type>::destroy(node_allocator, tmp);
        node_allocator.deallocate(tmp, 1);
        --list_size;
    }

    /**
     * Доступ к первому элементу
     */
    T& front() {
        if (!head) throw std::out_of_range("List is empty");
        return head->value;
    }

    const T& front() const {
        if (!head) throw std::out_of_range("List is empty");
        return head->value;
    }

    /**
     * Проверка на пустоту
     */
    bool empty() const noexcept {
        return list_size == 0;
    }

    /**
     * Получение количества элементов
     */
    std::size_t size() const noexcept {
        return list_size;
    }

    /**
     * Итератор начала списка
     */
    ForwardListIterator begin() {
        return ForwardListIterator(head);
    }

    /**
     * Итератор конца списка
     */
    ForwardListIterator end() {
        return ForwardListIterator(nullptr);
    }

    /**
     * Очистка списка
     */
    void clear() noexcept {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            std::allocator_traits<node_allocator_type>::destroy(node_allocator, current);
            node_allocator.deallocate(current, 1);
            current = next;
        }
        head = nullptr;
        list_size = 0;
    }

    /**
     * Деструктор списка
     * Разрушает все элементы и освобождает память через полиморфный аллокатор
     */
    ~ForwardList() {
        clear();
    }
};