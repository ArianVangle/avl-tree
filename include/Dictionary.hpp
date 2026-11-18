#pragma once

#include <cstddef>
#include <stdexcept>

#include "BinaryTree.hpp"
#include "Interfaces.hpp"

template <typename TKey, typename TValue>
struct Pair {
    TKey key;
    TValue value;
};

template <typename TKey, typename TValue>
bool operator<(const Pair<TKey, TValue>& a, const Pair<TKey, TValue>& b) {
    return a.key < b.key;
}

template <typename TKey, typename TValue>
class Dictionary : public IDictionary<TKey, TValue> {
public:
    size_t GetCount() const override { return tree_.GetCount(); }

    bool ContainsKey(const TKey& key) const override { return tree_.Contains(Probe(key)); }

    const TValue& Get(const TKey& key) const override {
        const Pair<TKey, TValue>* pair = tree_.Find(Probe(key));
        if (pair == nullptr) {
            throw std::out_of_range("ключа нет в словаре");
        }
        return pair->value;
    }

    void Add(const TKey& key, const TValue& value) override {
        tree_.Remove(Probe(key));
        tree_.Insert(Pair<TKey, TValue>{key, value});
    }

    void Remove(const TKey& key) override {
        if (!tree_.Remove(Probe(key))) {
            throw std::out_of_range("ключа нет в словаре");
        }
    }

private:
    static Pair<TKey, TValue> Probe(const TKey& key) { return Pair<TKey, TValue>{key, TValue()}; }

    BinaryTree<Pair<TKey, TValue>> tree_;
};
