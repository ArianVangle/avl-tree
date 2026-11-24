#pragma once

#include <cstddef>
#include <stdexcept>

#include "BinaryTree.hpp"
#include "DynamicArray.hpp"
#include "Interfaces.hpp"

template <typename TKey, typename TValue>
class Dictionary : public BinaryTree<TKey, TValue>, public IDictionary<TKey, TValue> {
    using Tree = BinaryTree<TKey, TValue>;

public:
    Dictionary() = default;

    template <typename F>
    Dictionary(const Set<TKey>& keys, F valueOf) {
        DynamicArray<TKey> all = keys.Traverse();
        for (size_t i = 0; i < all.GetLength(); ++i) {
            Tree::Insert(all[i], valueOf(all[i]));
        }
    }

    Set<TKey> Keys() const {
        Set<TKey> keys;
        DynamicArray<TKey> all = Tree::Traverse();
        for (size_t i = 0; i < all.GetLength(); ++i) {
            keys.Insert(all[i]);
        }
        return keys;
    }

    size_t GetCount() const override { return Tree::GetCount(); }

    bool ContainsKey(const TKey& key) const override { return Tree::Contains(key); }

    const TValue& Get(const TKey& key) const override {
        auto node = Tree::FindNode(key);
        if (node == nullptr) {
            throw std::out_of_range("ключа нет в словаре");
        }
        return node->value;
    }

    void Add(const TKey& key, const TValue& value) override {
        Tree::Remove(key);
        Tree::Insert(key, value);
    }

    void Remove(const TKey& key) override {
        if (!Tree::Remove(key)) {
            throw std::out_of_range("ключа нет в словаре");
        }
    }
};
