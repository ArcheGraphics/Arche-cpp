//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <imgui.h>

namespace vox {
/*
    The following code was copied from imgui and modified to add wildcard search
*/
struct TextFilter {
    IMGUI_API TextFilter(const char *default_filter = "");
    IMGUI_API bool Draw(const char *label = "###Filter", float width = 0.0f);// Helper calling InputText+Build
    IMGUI_API bool PassFilter(const char *text, const char *text_end = NULL) const;
    IMGUI_API void Build();
    void Clear() {
        InputBuf[0] = 0;
        Build();
    }
    bool IsActive() const { return !Filters.empty(); }
    ImGuiID GetHash() const;

    // [Internal]
    struct TextRange {
        const char *b;
        const char *e;

        TextRange() { b = e = NULL; }
        TextRange(const char *_b, const char *_e) {
            b = _b;
            e = _e;
        }
        bool empty() const { return b == e; }
        void split(char separator, ImVector<TextRange> *out) const;
    };
    static constexpr size_t InputBufSize = 256;
    char InputBuf[InputBufSize];
    ImVector<TextRange> Filters;
    int CountGrep;

    std::function<const char *(const char *, const char *, const char *, const char *)> PatternMatchFunc;
};

}// namespace vox