#include "TraversalFormat.hpp"

#include <cctype>
#include <stdexcept>

namespace {

const std::string kOpenBrackets = "([{<";
const std::string kCloseBrackets = ")]}>";

void SkipSpaces(const std::string& text, size_t& pos) {
    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
        ++pos;
    }
}

[[noreturn]] void Fail(const std::string& what, const std::string& format) {
    throw std::invalid_argument(what + ": «" + format + "»");
}

bool ReadLetter(const std::string& text, size_t& pos, Part& part) {
    const std::string letters[] = {"К", "Л", "П"};
    const Part parts[] = {Part::Root, Part::Left, Part::Right};
    for (int i = 0; i < 3; ++i) {
        if (text.compare(pos, letters[i].size(), letters[i]) == 0) {
            pos += letters[i].size();
            part = parts[i];
            return true;
        }
    }
    return false;
}

std::string Letter(Part part) {
    if (part == Part::Root) {
        return "К";
    }
    return part == Part::Left ? "Л" : "П";
}

}

TraversalFormat::TraversalFormat(const std::string& format) {
    bool used[3] = {false, false, false};
    size_t pos = 0;
    for (int i = 0; i < 3; ++i) {
        SkipSpaces(format, pos);
        if (pos >= format.size() || kOpenBrackets.find(format[pos]) == std::string::npos) {
            Fail("ожидалась открывающая скобка ( [ { <", format);
        }
        char open = format[pos++];
        Part part;
        if (!ReadLetter(format, pos, part)) {
            Fail("ожидалась буква К, Л или П", format);
        }
        if (pos >= format.size() || kCloseBrackets.find(format[pos]) == std::string::npos) {
            Fail("ожидалась закрывающая скобка ) ] } >", format);
        }
        char close = format[pos++];
        int index = static_cast<int>(part);
        if (used[index]) {
            Fail("буква повторяется", format);
        }
        used[index] = true;
        parts_[i] = FormatPart{part, open, close};
    }
    SkipSpaces(format, pos);
    if (pos != format.size()) {
        Fail("лишние символы в формате", format);
    }
}

const FormatPart& TraversalFormat::operator[](size_t index) const {
    if (index >= 3) {
        throw std::out_of_range("в формате три части");
    }
    return parts_[index];
}

std::string TraversalFormat::Order() const {
    return Letter(parts_[0].part) + Letter(parts_[1].part) + Letter(parts_[2].part);
}
