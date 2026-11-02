#pragma once

#include <cstddef>
#include <memory>
#include <utility>

template <typename T>
struct TreeNode {
    explicit TreeNode(T v) : value(std::move(v)), height(1), size(1) {}

    T value;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;
    int height;
    size_t size;
};

template <typename T>
int HeightOf(const TreeNode<T>* node) {
    return node == nullptr ? 0 : node->height;
}

template <typename T>
size_t SizeOf(const TreeNode<T>* node) {
    return node == nullptr ? 0 : node->size;
}

template <typename T>
void UpdateNode(TreeNode<T>& node) {
    int left = HeightOf(node.left.get());
    int right = HeightOf(node.right.get());
    node.height = 1 + (left > right ? left : right);
    node.size = 1 + SizeOf(node.left.get()) + SizeOf(node.right.get());
}
