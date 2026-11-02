#pragma once

#include <cstddef>
#include <string>
#include <utility>

#include "DynamicArray.hpp"
#include "TreeNode.hpp"
#include "TreeText.hpp"

using TextGrid = DynamicArray<DynamicArray<std::string>>;

template <typename T>
size_t WidestValue(const TreeNode<T>* node) {
    if (node == nullptr) {
        return 0;
    }
    size_t own = ValueToText(node->value).size();
    size_t left = WidestValue(node->left.get());
    size_t right = WidestValue(node->right.get());
    size_t widest = own > left ? own : left;
    return widest > right ? widest : right;
}

template <typename T>
size_t PlaceNode(const TreeNode<T>* node, size_t depth, size_t cell, size_t& column,
                 TextGrid& grid) {
    bool hasLeft = node->left != nullptr;
    bool hasRight = node->right != nullptr;
    size_t left = hasLeft ? PlaceNode(node->left.get(), depth + 1, cell, column, grid) : 0;
    size_t x = column * cell + cell / 2;
    ++column;
    size_t right = hasRight ? PlaceNode(node->right.get(), depth + 1, cell, column, grid) : 0;

    std::string text = ValueToText(node->value);
    size_t start = x - (text.size() - 1) / 2;
    for (size_t i = 0; i < text.size(); ++i) {
        grid[2 * depth][start + i] = std::string(1, text[i]);
    }
    if (!hasLeft && !hasRight) {
        return x;
    }
    DynamicArray<std::string>& links = grid[2 * depth + 1];
    if (hasLeft) {
        links[left] = "┌";
        for (size_t i = left + 1; i < x; ++i) {
            links[i] = "─";
        }
    }
    if (hasRight) {
        links[right] = "┐";
        for (size_t i = x + 1; i < right; ++i) {
            links[i] = "─";
        }
    }
    links[x] = hasLeft && hasRight ? "┴" : hasLeft ? "┘" : "└";
    return x;
}

template <typename T>
std::string DrawTree(const TreeNode<T>* root, size_t maxWidth) {
    if (root == nullptr) {
        return "";
    }
    size_t cell = WidestValue(root) + 1;
    size_t width = SizeOf(root) * cell;
    if (width > maxWidth) {
        return "";
    }
    TextGrid grid;
    for (int row = 0; row < 2 * HeightOf(root) - 1; ++row) {
        DynamicArray<std::string> line;
        for (size_t i = 0; i < width; ++i) {
            line.Append(" ");
        }
        grid.Append(std::move(line));
    }
    size_t column = 0;
    PlaceNode(root, 0, cell, column, grid);
    std::string result;
    for (size_t row = 0; row < grid.GetLength(); ++row) {
        size_t end = width;
        while (end > 0 && grid[row][end - 1] == " ") {
            --end;
        }
        for (size_t i = 0; i < end; ++i) {
            result += grid[row][i];
        }
        result += "\n";
    }
    return result;
}
