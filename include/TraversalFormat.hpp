#pragma once

#include <cstddef>
#include <string>

enum class Part { Root, Left, Right };

struct FormatPart {
    Part part;
    char open;
    char close;
};

inline const char* const kDefaultFormat = "{К}(Л)[П]";

inline const char* const kAllFormats[] = {"{К}(Л)[П]", "{К}[П](Л)", "(Л){К}[П]",
                                          "(Л)[П]{К}", "[П]{К}(Л)", "[П](Л){К}"};

class TraversalFormat {
public:
    explicit TraversalFormat(const std::string& format);

    const FormatPart& operator[](size_t index) const;
    std::string Order() const;

private:
    FormatPart parts_[3];
};
