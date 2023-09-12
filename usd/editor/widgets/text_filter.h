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
    IMGUI_API explicit TextFilter(const char *default_filter = "");
    IMGUI_API bool draw(const char *label = "###Filter", float width = 0.0f);// Helper calling InputText+Build
    IMGUI_API bool pass_filter(const char *text, const char *text_end = nullptr) const;
    IMGUI_API void build();
    void clear() {
        InputBuf[0] = 0;
        build();
    }
    [[nodiscard]] bool is_active() const { return !Filters.empty(); }
    [[nodiscard]] ImGuiID get_hash() const;

    // [Internal]
    struct TextRange {
        const char *b;
        const char *e;

        TextRange() { b = e = nullptr; }
        TextRange(const char *_b, const char *_e) {
            b = _b;
            e = _e;
        }
        [[nodiscard]] bool empty() const { return b == e; }
        void split(char separator, ImVector<TextRange> *out) const;
    };
    static constexpr size_t InputBufSize = 256;
    char InputBuf[InputBufSize]{};
    ImVector<TextRange> Filters;
    int CountGrep;

    std::function<const char *(const char *, const char *, const char *, const char *)> PatternMatchFunc;
};

}// namespace vox