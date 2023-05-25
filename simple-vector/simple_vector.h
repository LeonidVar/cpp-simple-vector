#pragma once

#include <cassert>
#include <initializer_list>
#include <array>
#include "array_ptr.h"
#include <stdexcept>
#include <iostream>

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve) :
        capacity_(capacity_to_reserve) {}

    size_t GetCapacity() {
        return capacity_;
    }
private:
    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) :
        items_(size), size_(size), capacity_(size) {
        ArrayPtr<Type> temp_arr(size);

        //Отсутствие цикла вызывает множество ошибок тестов 12, 16, 17... вида { return item == type_t{}; }) is false,
        for (auto it = temp_arr.Get(); it != temp_arr.Get() + size; ++it) {
            *it = std::move(Type{});
        }

        temp_arr.swap(items_);

        //Эти функции не скомпилируются
        //std::fill(temp_arr.Get(), temp_arr.Get() + size, Type{});
        //std::fill(this->begin(), this->end(), Type{});
    }


    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, Type value) :
    items_(size), size_(size), capacity_(size){
        std::fill(this->begin(), this->end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) :
        items_(init.size()) {       
        std::copy(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), this->begin());// temp_arr.Get());
        size_ = init.size();
        capacity_ = init.size();
    }

    SimpleVector(ReserveProxyObj rpo) {
        this->Reserve(rpo.GetCapacity());
    }

    SimpleVector(const SimpleVector& other) {
        ArrayPtr<Type> temp_arr(other.size_);
        std::copy(other.begin(), other.end(), temp_arr.Get());
        temp_arr.swap(items_);
        size_ = other.size_;
        capacity_ = other.size_;
    }

    SimpleVector(SimpleVector&& other) {
        items_ = std::move(other.items_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index < size_) {
            return items_[index];
        }
        else {
            throw std::out_of_range("out_of_range");
        }
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index < size_) {
            return items_[index];
        }
        else {
            throw std::out_of_range("out_of_range");
        }
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= capacity_) {
            if (new_size <= size_) {
                size_ = new_size;
            }
            else {
                for (size_t i = size_; i <= new_size; ++i) {
                    items_[i - 1] = Type{};
                }
            }
        }
        else {
            size_t n = std::max(new_size, capacity_ * 2);
            SimpleVector<Type> temp_vector(n);
            std::copy(std::make_move_iterator(this->begin()), std::make_move_iterator(this->end()), temp_vector.begin());
            temp_vector.swap(*this);
            size_ = n;
            capacity_ = n;
        }
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> temp_arr(new_capacity);
            std::copy(this->begin(), this->end(), temp_arr.Get());
            items_.swap(temp_arr);
            capacity_ = new_capacity;
        }
    }
    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (rhs.begin() != this->begin())
        {
            SimpleVector temp_vector(rhs);
            std::copy(rhs.cbegin(), rhs.cend(), temp_vector.begin());
            this->swap(temp_vector);
        }
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type item) {
        if (capacity_ == size_) {
            size_t old_size = size_;
            capacity_ == 0 ? this->Resize(1) : this->Resize(capacity_ * 2);
            size_ = old_size;
        }
        items_[size_] = std::move(item);
        ++size_;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type value) {
        assert(this->begin() <= pos && this->end() >= pos);
        Iterator it = const_cast<Iterator>(pos);
        if (capacity_ == size_) {
            size_t n;
            capacity_ == 0 ? n = 1 : n = capacity_ * 2;
            SimpleVector<Type> new_simple_vector(n);
            auto last_temp_it = std::move(this->begin(), it, new_simple_vector.begin());
            *last_temp_it = std::move(value);
            std::move(it, this->end(), last_temp_it + 1);
            new_simple_vector.items_.swap(items_);
            capacity_ = n;
            ++size_;
            return last_temp_it;
        }
        else
        {
            std::copy_backward(std::make_move_iterator(it), std::make_move_iterator(this->end()), this->end() + 1);
            *it = std::move(value);
            ++size_;
            return it;
        }
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        Iterator it = const_cast<Iterator>(pos);
        std::copy(std::make_move_iterator(it + 1), std::make_move_iterator(this->end()), it);
        --size_;
        return it;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    ArrayPtr<Type> items_;

    size_t size_{ 0 };
    size_t capacity_{ 0 };

};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() &&
        std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);;
}

