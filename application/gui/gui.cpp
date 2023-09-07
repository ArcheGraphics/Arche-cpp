//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui.h"
#define IMGUI_IMPL_METAL_CPP
#include "gui/imgui_impl_metal.h"

namespace vox {
GUI::GUI(MTL::Device &device) {
    ImGui_ImplMetal_Init(&device);
}

GUI::~GUI() {
    ImGui_ImplMetal_Shutdown();
}

void GUI::newFrame(MTL::RenderPassDescriptor &renderPassDescriptor) {
    ImGui_ImplMetal_NewFrame(&renderPassDescriptor);
}

void GUI::draw(ImDrawData *drawData,
               MTL::CommandBuffer &commandBuffer,
               MTL::RenderCommandEncoder &commandEncoder) {
    ImGui_ImplMetal_RenderDrawData(drawData, &commandBuffer, &commandEncoder);
}

}// namespace vox
