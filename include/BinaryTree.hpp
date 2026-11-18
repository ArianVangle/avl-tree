#pragma once

#include <cstddef>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "DynamicArray.hpp"
#include "Interfaces.hpp"
#include "TraversalFormat.hpp"
#include "TreeDrawing.hpp"
#include "TreeEnumerator.hpp"
#include "TreeNode.hpp"
#include "TreeText.hpp"

template <typename T>
class BinaryTree : public ICollection<T>, public IEnumerable<T> {
    using Node = TreeNode<T>;

public:
    BinaryTree() = default;

    BinaryTree(const BinaryTree& other) : root_(Clone(other.root_.get())) {}

    BinaryTree(BinaryTree&& other) noexcept = default;

    BinaryTree& operator=(const BinaryTree& other) {
        if (this != &other) {
            root_ = Clone(other.root_.get());
        }
        return *this;
    }

    BinaryTree& operator=(BinaryTree&& other) noexcept = default;

    bool Insert(const T& value) { return InsertInto(root_, value); }

    bool Remove(const T& value) { return RemoveFrom(root_, value); }

    bool Contains(const T& value) const override { return FindNode(value) != nullptr; }

    const T* Find(const T& value) const {
        const Node* node = FindNode(value);
        return node == nullptr ? nullptr : &node->value;
    }

    size_t GetCount() const override { return SizeOf(root_.get()); }

    const T& Get(size_t index) const override {
        if (index >= GetCount()) {
            throw std::out_of_range("индекс вне диапазона: " + std::to_string(index));
        }
        const Node* node = root_.get();
        while (true) {
            size_t leftSize = SizeOf(node->left.get());
            if (index < leftSize) {
                node = node->left.get();
            } else if (index == leftSize) {
                return node->value;
            } else {
                index -= leftSize + 1;
                node = node->right.get();
            }
        }
    }

    std::unique_ptr<IEnumerator<T>> GetEnumerator() const override {
        return std::make_unique<TreeEnumerator<T>>(root_.get());
    }

    bool IsEmpty() const { return root_ == nullptr; }
    int GetHeight() const { return HeightOf(root_.get()); }
    void Clear() { root_.reset(); }

    template <typename F>
    BinaryTree Map(F function) const {
        BinaryTree result;
        ForEach(root_.get(), [&](const T& value) { result.Insert(function(value)); });
        return result;
    }

    template <typename P>
    BinaryTree Where(P predicate) const {
        BinaryTree result;
        ForEach(root_.get(), [&](const T& value) {
            if (predicate(value)) {
                result.Insert(value);
            }
        });
        return result;
    }

    template <typename A, typename F>
    A Reduce(F function, A start) const {
        A accumulator = std::move(start);
        ForEach(root_.get(), [&](const T& value) { accumulator = function(value, accumulator); });
        return accumulator;
    }

    BinaryTree Merge(const BinaryTree& other) const {
        BinaryTree result(*this);
        ForEach(other.root_.get(), [&](const T& value) { result.Insert(value); });
        return result;
    }

    BinaryTree Subtree(const T& value) const {
        const Node* node = FindNode(value);
        if (node == nullptr) {
            throw std::invalid_argument("элемента «" + ValueToText(value) + "» нет в дереве");
        }
        BinaryTree result;
        result.root_ = Clone(node);
        return result;
    }

    bool ContainsSubtree(const BinaryTree& other) const {
        if (other.IsEmpty()) {
            return true;
        }
        return SameShape(FindNode(other.root_->value), other.root_.get());
    }

    DynamicArray<T> Traverse(const std::string& format = "(Л){К}[П]") const {
        DynamicArray<T> result;
        VisitTree(root_.get(), TraversalFormat(format), result);
        return result;
    }

    std::string ToString(const std::string& format = kDefaultFormat) const {
        TraversalFormat parsed(format);
        std::string result;
        if (root_ != nullptr) {
            WriteTree(root_.get(), parsed, result);
        }
        return result;
    }

    static BinaryTree FromString(const std::string& text,
                                 const std::string& format = kDefaultFormat) {
        TraversalFormat parsed(format);
        BinaryTree result;
        result.root_ = TreeParser<T>(text, parsed).Parse();
        if (!result.CheckInvariants()) {
            throw std::invalid_argument("строка не задаёт сбалансированное дерево поиска");
        }
        return result;
    }

    bool CheckInvariants() const { return IsOrdered() && IsValidAvl(root_.get()); }

    std::string Draw(size_t maxWidth) const { return DrawTree(root_.get(), maxWidth); }

    friend bool operator==(const BinaryTree& a, const BinaryTree& b) {
        if (a.GetCount() != b.GetCount()) {
            return false;
        }
        DynamicArray<T> left = a.Traverse();
        DynamicArray<T> right = b.Traverse();
        for (size_t i = 0; i < left.GetLength(); ++i) {
            if (!a.Same(left[i], right[i])) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const BinaryTree& a, const BinaryTree& b) { return !(a == b); }

    friend BinaryTree operator+(const BinaryTree& a, const BinaryTree& b) { return a.Merge(b); }

    friend std::ostream& operator<<(std::ostream& out, const BinaryTree& tree) {
        return out << tree.ToString();
    }

private:
    template <typename F>
    static void ForEach(const Node* node, F&& action) {
        if (node == nullptr) {
            return;
        }
        ForEach(node->left.get(), action);
        action(node->value);
        ForEach(node->right.get(), action);
    }

    static int BalanceOf(const Node& node) {
        return HeightOf(node.left.get()) - HeightOf(node.right.get());
    }

    static void RotateRight(std::unique_ptr<Node>& node) {
        std::unique_ptr<Node> pivot = std::move(node->left);
        node->left = std::move(pivot->right);
        UpdateNode(*node);
        pivot->right = std::move(node);
        node = std::move(pivot);
        UpdateNode(*node);
    }

    static void RotateLeft(std::unique_ptr<Node>& node) {
        std::unique_ptr<Node> pivot = std::move(node->right);
        node->right = std::move(pivot->left);
        UpdateNode(*node);
        pivot->left = std::move(node);
        node = std::move(pivot);
        UpdateNode(*node);
    }

    static void Rebalance(std::unique_ptr<Node>& node) {
        UpdateNode(*node);
        int balance = BalanceOf(*node);
        if (balance > 1) {
            if (BalanceOf(*node->left) < 0) {
                RotateLeft(node->left);
            }
            RotateRight(node);
        } else if (balance < -1) {
            if (BalanceOf(*node->right) > 0) {
                RotateRight(node->right);
            }
            RotateLeft(node);
        }
    }

    bool InsertInto(std::unique_ptr<Node>& node, const T& value) {
        if (node == nullptr) {
            node = std::make_unique<Node>(value);
            return true;
        }
        bool inserted;
        if (value < node->value) {
            inserted = InsertInto(node->left, value);
        } else if (node->value < value) {
            inserted = InsertInto(node->right, value);
        } else {
            return false;
        }
        Rebalance(node);
        return inserted;
    }

    bool RemoveFrom(std::unique_ptr<Node>& node, const T& value) {
        if (node == nullptr) {
            return false;
        }
        bool removed = true;
        if (value < node->value) {
            removed = RemoveFrom(node->left, value);
        } else if (node->value < value) {
            removed = RemoveFrom(node->right, value);
        } else if (node->left == nullptr) {
            node = std::move(node->right);
        } else if (node->right == nullptr) {
            node = std::move(node->left);
        } else {
            node->value = TakeMin(node->right);
        }
        if (node != nullptr) {
            Rebalance(node);
        }
        return removed;
    }

    static T TakeMin(std::unique_ptr<Node>& node) {
        if (node->left == nullptr) {
            T value = std::move(node->value);
            node = std::move(node->right);
            return value;
        }
        T value = TakeMin(node->left);
        Rebalance(node);
        return value;
    }

    const Node* FindNode(const T& value) const {
        const Node* node = root_.get();
        while (node != nullptr) {
            if (value < node->value) {
                node = node->left.get();
            } else if (node->value < value) {
                node = node->right.get();
            } else {
                return node;
            }
        }
        return nullptr;
    }

    static bool Same(const T& a, const T& b) { return !(a < b) && !(b < a); }

    bool SameShape(const Node* a, const Node* b) const {
        if (a == nullptr || b == nullptr) {
            return a == b;
        }
        return Same(a->value, b->value) && SameShape(a->left.get(), b->left.get()) &&
               SameShape(a->right.get(), b->right.get());
    }

    static std::unique_ptr<Node> Clone(const Node* node) {
        if (node == nullptr) {
            return nullptr;
        }
        auto copy = std::make_unique<Node>(node->value);
        copy->left = Clone(node->left.get());
        copy->right = Clone(node->right.get());
        UpdateNode(*copy);
        return copy;
    }

    bool IsOrdered() const {
        DynamicArray<T> values = Traverse();
        for (size_t i = 1; i < values.GetLength(); ++i) {
            if (!(values[i - 1] < values[i])) {
                return false;
            }
        }
        return true;
    }

    static bool IsValidAvl(const Node* node) {
        if (node == nullptr) {
            return true;
        }
        int left = HeightOf(node->left.get());
        int right = HeightOf(node->right.get());
        int height = 1 + (left > right ? left : right);
        size_t size = 1 + SizeOf(node->left.get()) + SizeOf(node->right.get());
        int balance = left - right;
        return node->height == height && node->size == size && balance >= -1 && balance <= 1 &&
               IsValidAvl(node->left.get()) && IsValidAvl(node->right.get());
    }

    std::unique_ptr<Node> root_;
};
