#pragma once

#include <cstddef>
#include <memory>
#include <utility>

struct Empty {};

template <typename TKey, typename TValue = Empty>
struct TreeNode {
    explicit TreeNode(TKey k, TValue v = TValue())
        : key(std::move(k)), value(std::move(v)), height(1), size(1) {}

    TKey key;
    [[no_unique_address]] TValue value;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;
    int height;
    size_t size;
};

template <typename TKey, typename TValue>
int HeightOf(const TreeNode<TKey, TValue>* node) {
    return node == nullptr ? 0 : node->height;
}

template <typename TKey, typename TValue>
size_t SizeOf(const TreeNode<TKey, TValue>* node) {
    return node == nullptr ? 0 : node->size;
}

template <typename TKey, typename TValue>
void UpdateNode(TreeNode<TKey, TValue>& node) {
    int left = HeightOf(node.left.get());
    int right = HeightOf(node.right.get());
    node.height = 1 + (left > right ? left : right);
    node.size = 1 + SizeOf(node.left.get()) + SizeOf(node.right.get());
}
