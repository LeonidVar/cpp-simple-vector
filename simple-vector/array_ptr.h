#pragma once
#include <cstdlib> 
template <typename Type>
class ArrayPtr {
public:
    // �������������� ArrayPtr ������� ����������
    ArrayPtr() = default;

    // ������ � ���� ������ �� size ��������� ���� Type.
    // ���� size == 0, ���� raw_ptr_ ������ ���� ����� nullptr
    explicit ArrayPtr(size_t size) {
        // ���������� ����������� ��������������
        if (size) {
            raw_ptr_ = new Type[size];
        }
    }

    // ����������� �� ������ ���������, ��������� ����� ������� � ���� ���� nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        // ���������� ����������� ��������������
        raw_ptr_ = raw_ptr;
    }

    // ��������� �����������
    ArrayPtr(const ArrayPtr&) = delete;

    ~ArrayPtr() {
        // �������� ���������� ��������������
        delete[] raw_ptr_;
    }

    // ��������� ������������
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }
    ArrayPtr& operator=(ArrayPtr&& other) {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
        return *this;
    }


    // ���������� ��������� �������� � ������, ���������� �������� ������ �������
    // ����� ������ ������ ��������� �� ������ ������ ����������
    [[nodiscard]] Type* Release() noexcept {
        // ��������. ���������� ����� ��������������
        auto temp_ptr = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp_ptr;
    }

    // ���������� ������ �� ������� ������� � �������� index
    Type& operator[](size_t index) noexcept {
        // ���������� �������� ��������������
        return *(raw_ptr_ + index);
    }

    // ���������� ����������� ������ �� ������� ������� � �������� index
    const Type& operator[](size_t index) const noexcept {
        // ���������� �������� ��������������
        return *(raw_ptr_ + index);
    }

    // ���������� true, ���� ��������� ���������, � false � ��������� ������
    explicit operator bool() const {
        // ��������. ���������� �������� ��������������
        return static_cast<bool>(raw_ptr_);
    }

    // ���������� �������� ������ ���������, ��������� ����� ������ �������
    Type* Get() const noexcept {
        // ��������. ���������� ����� ��������������
        return raw_ptr_;
    }

    // ������������ ��������� ��������� �� ������ � �������� other
    void swap(ArrayPtr& other) noexcept {
        // ���������� ����� ��������������
        auto temp_ptr = raw_ptr_;
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = temp_ptr;
    }

private:
    Type* raw_ptr_ = nullptr;
};