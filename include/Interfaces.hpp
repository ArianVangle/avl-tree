#pragma once

#include <cstddef>
#include <memory>

template <typename T>
class ICollection {
public:
    virtual ~ICollection() = default;
    virtual size_t GetCount() const = 0;
    virtual bool Contains(const T& value) const = 0;
    virtual const T& Get(size_t index) const = 0;
};

template <typename T>
class IEnumerator {
public:
    virtual ~IEnumerator() = default;
    virtual bool MoveNext() = 0;
    virtual const T& GetCurrent() const = 0;
    virtual void Reset() = 0;
};

template <typename T>
class IEnumerable {
public:
    virtual ~IEnumerable() = default;
    virtual std::unique_ptr<IEnumerator<T>> GetEnumerator() const = 0;
};
