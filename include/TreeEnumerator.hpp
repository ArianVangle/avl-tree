#pragma once

#include <cstddef>
#include <stdexcept>

#include "DynamicArray.hpp"
#include "Interfaces.hpp"
#include "TraversalFormat.hpp"
#include "TreeNode.hpp"

template <typename TKey, typename TValue = Empty>
class TreeEnumerator : public IEnumerator<TKey> {
    struct Frame {
        const TreeNode<TKey, TValue>* node = nullptr;
        size_t part = 0;
    };

public:
    TreeEnumerator(const TreeNode<TKey, TValue>* root, const TraversalFormat& format)
        : root_(root), format_(format), current_(nullptr) {
        Reset();
    }

    bool MoveNext() override {
        while (!stack_.IsEmpty()) {
            Frame& top = stack_.Last();
            if (top.part == 3) {
                stack_.RemoveLast();
                continue;
            }
            const TreeNode<TKey, TValue>* node = top.node;
            Part part = format_[top.part++].part;
            if (part == Part::Root) {
                current_ = node;
                return true;
            }
            const TreeNode<TKey, TValue>* child = part == Part::Left ? node->left.get() : node->right.get();
            if (child != nullptr) {
                stack_.Append(Frame{child, 0});
            }
        }
        current_ = nullptr;
        return false;
    }

    const TKey& GetCurrent() const override { return Current().key; }

    const TValue& GetValue() const { return Current().value; }

    void Reset() override {
        while (!stack_.IsEmpty()) {
            stack_.RemoveLast();
        }
        current_ = nullptr;
        if (root_ != nullptr) {
            stack_.Append(Frame{root_, 0});
        }
    }

private:
    const TreeNode<TKey, TValue>& Current() const {
        if (current_ == nullptr) {
            throw std::out_of_range("перечислитель не указывает на элемент");
        }
        return *current_;
    }

    const TreeNode<TKey, TValue>* root_;
    TraversalFormat format_;
    const TreeNode<TKey, TValue>* current_;
    DynamicArray<Frame> stack_;
};
