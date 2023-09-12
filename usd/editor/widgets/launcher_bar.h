//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
class Editor;

//
// This is a simple launcher bar like a shelf, which allows to run an external command, like triggering
// renders or publishing files from a button.

// It is an editor specific UI
void draw_launcher_bar(Editor *editor);

}// namespace vox
