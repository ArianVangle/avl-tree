#pragma once

#include <cctype>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "DynamicArray.hpp"
#include "TraversalFormat.hpp"
#include "TreeNode.hpp"

template <typename T>
std::string ValueToText(const T& value) {
    std::ostringstream out;
    out.precision(15);
    out << value;
    return out.str();
}

template <typename T>
T TextToValue(const std::string& text) {
    if constexpr (std::is_same_v<T, std::string>) {
        return text;
    } else {
        std::istringstream in(text);
        T value;
        if (!(in >> value)) {
            throw std::invalid_argument("не удалось прочитать значение «" + text + "»");
        }
        in >> std::ws;
        if (!in.eof()) {
            throw std::invalid_argument("лишние символы в значении «" + text + "»");
        }
        return value;
    }
}

template <typename TKey, typename TValue>
void VisitTree(const TreeNode<TKey, TValue>* node, const TraversalFormat& format,
               DynamicArray<TKey>& out) {
    if (node == nullptr) {
        return;
    }
    for (size_t i = 0; i < 3; ++i) {
        switch (format[i].part) {
            case Part::Root:
                out.Append(node->key);
                break;
            case Part::Left:
                VisitTree(node->left.get(), format, out);
                break;
            case Part::Right:
                VisitTree(node->right.get(), format, out);
                break;
        }
    }
}

template <typename T>
std::string CheckedText(const T& value, const FormatPart& part) {
    std::string text = ValueToText(value);
    bool badEdges = text.empty() || std::isspace(static_cast<unsigned char>(text.front())) ||
                    std::isspace(static_cast<unsigned char>(text.back()));
    if (badEdges || text.find(part.close) != std::string::npos) {
        throw std::invalid_argument("значение «" + text + "» нельзя записать в формате: " +
                                    "оно пустое, с пробелами по краям или содержит «" +
                                    part.close + "»");
    }
    return text;
}

template <typename TKey, typename TValue>
void WriteTree(const TreeNode<TKey, TValue>* node, const TraversalFormat& format,
               std::string& out) {
    for (size_t i = 0; i < 3; ++i) {
        const FormatPart& part = format[i];
        out += part.open;
        if (part.part == Part::Root) {
            out += CheckedText(node->key, part);
        } else {
            const TreeNode<TKey, TValue>* child =
                part.part == Part::Left ? node->left.get() : node->right.get();
            if (child != nullptr) {
                WriteTree(child, format, out);
            }
        }
        out += part.close;
    }
}

template <typename TKey, typename TValue>
class TreeParser {
public:
    TreeParser(const std::string& text, const TraversalFormat& format)
        : text_(text), format_(format), pos_(0) {}

    std::unique_ptr<TreeNode<TKey, TValue>> Parse() {
        SkipSpaces();
        if (pos_ == text_.size()) {
            return nullptr;
        }
        std::unique_ptr<TreeNode<TKey, TValue>> root = ParseNode();
        SkipSpaces();
        if (pos_ != text_.size()) {
            Fail("лишние символы");
        }
        return root;
    }

private:
    std::unique_ptr<TreeNode<TKey, TValue>> ParseNode() {
        std::unique_ptr<TKey> key;
        std::unique_ptr<TreeNode<TKey, TValue>> left;
        std::unique_ptr<TreeNode<TKey, TValue>> right;
        for (size_t i = 0; i < 3; ++i) {
            const FormatPart& part = format_[i];
            SkipSpaces();
            Expect(part.open);
            if (part.part == Part::Root) {
                key = std::make_unique<TKey>(ReadKey(part.close));
            } else {
                std::unique_ptr<TreeNode<TKey, TValue>>& child = part.part == Part::Left ? left : right;
                SkipSpaces();
                if (pos_ < text_.size() && text_[pos_] != part.close) {
                    child = ParseNode();
                    SkipSpaces();
                }
            }
            Expect(part.close);
        }
        auto node = std::make_unique<TreeNode<TKey, TValue>>(std::move(*key));
        node->left = std::move(left);
        node->right = std::move(right);
        UpdateNode(*node);
        return node;
    }

    TKey ReadKey(char close) {
        size_t end = text_.find(close, pos_);
        if (end == std::string::npos) {
            Fail("не закрыто значение");
        }
        std::string raw = Trim(text_.substr(pos_, end - pos_));
        if (raw.empty()) {
            Fail("пустое значение");
        }
        pos_ = end;
        return TextToValue<TKey>(raw);
    }

    void Expect(char symbol) {
        if (pos_ >= text_.size() || text_[pos_] != symbol) {
            Fail(std::string("ожидался символ «") + symbol + "»");
        }
        ++pos_;
    }

    void SkipSpaces() {
        while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
    }

    [[noreturn]] void Fail(const std::string& what) const {
        throw std::invalid_argument("ошибка разбора дерева: " + what + " (позиция " +
                                    std::to_string(pos_) + ")");
    }

    static std::string Trim(const std::string& text) {
        size_t first = 0;
        while (first < text.size() && std::isspace(static_cast<unsigned char>(text[first]))) {
            ++first;
        }
        size_t last = text.size();
        while (last > first && std::isspace(static_cast<unsigned char>(text[last - 1]))) {
            --last;
        }
        return text.substr(first, last - first);
    }

    const std::string& text_;
    const TraversalFormat& format_;
    size_t pos_;
};
