#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

template <typename T>
class DynamicArray {
public:
    DynamicArray() : data_(), size_(0), capacity_(0) {}

    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;

    DynamicArray(DynamicArray&& other) noexcept
        : data_(std::move(other.data_)), size_(other.size_), capacity_(other.capacity_) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void Append(T value) {
        if (size_ == capacity_) {
            Grow();
        }
        data_[size_] = std::move(value);
        ++size_;
    }

    void RemoveLast() {
        if (size_ == 0) {
            throw std::out_of_range("массив пуст");
        }
        --size_;
        data_[size_] = T();
    }

    T& Last() {
        if (size_ == 0) {
            throw std::out_of_range("массив пуст");
        }
        return data_[size_ - 1];
    }

    T& Get(size_t index) {
        CheckIndex(index);
        return data_[index];
    }

    const T& Get(size_t index) const {
        CheckIndex(index);
        return data_[index];
    }

    T& operator[](size_t index) { return Get(index); }
    const T& operator[](size_t index) const { return Get(index); }

    size_t GetLength() const { return size_; }
    bool IsEmpty() const { return size_ == 0; }

private:
    void CheckIndex(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("индекс вне диапазона");
        }
    }

    void Grow() {
        size_t newCapacity = capacity_ == 0 ? 4 : capacity_ * 2;
        std::unique_ptr<T[]> fresh(new T[newCapacity]);
        for (size_t i = 0; i < size_; ++i) {
            fresh[i] = std::move(data_[i]);
        }
        data_ = std::move(fresh);
        capacity_ = newCapacity;
    }

    std::unique_ptr<T[]> data_;
    size_t size_;
    size_t capacity_;
};
