//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
// Returns a nicer label for the predefined tokens
inline const char *get_token_label(const TfToken &token) {
    if (token == SdfFieldKeys->CustomData) {
        return "Custom Data";
    } else if (token == SdfFieldKeys->AssetInfo) {
        return "Asset Info";
    }// add others as needed
    return token.GetText();
}

}// namespace vox