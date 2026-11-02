#pragma once

#include <stdexcept>

#include "DynamicArray.hpp"
#include "Interfaces.hpp"
#include "TreeNode.hpp"

template <typename T>
class TreeEnumerator : public IEnumerator<T> {
public:
    explicit TreeEnumerator(const TreeNode<T>* root) : root_(root), current_(nullptr) {
        Reset();
    }

    bool MoveNext() override {
        if (stack_.IsEmpty()) {
            current_ = nullptr;
            return false;
        }
        current_ = stack_.Last();
        stack_.RemoveLast();
        PushLeft(current_->right.get());
        return true;
    }

    const T& GetCurrent() const override {
        if (current_ == nullptr) {
            throw std::out_of_range("перечислитель не указывает на элемент");
        }
        return current_->value;
    }

    void Reset() override {
        while (!stack_.IsEmpty()) {
            stack_.RemoveLast();
        }
        current_ = nullptr;
        PushLeft(root_);
    }

private:
    void PushLeft(const TreeNode<T>* node) {
        while (node != nullptr) {
            stack_.Append(node);
            node = node->left.get();
        }
    }

    const TreeNode<T>* root_;
    const TreeNode<T>* current_;
    DynamicArray<const TreeNode<T>*> stack_;
};
