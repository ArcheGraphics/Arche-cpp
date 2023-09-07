//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_pass.h"
#include "framework/common/metal_helpers.h"
#include "framework/common/logging.h"

namespace vox {
RenderPass::RenderPass(MTL::Library &library,
                       MTL::RenderPassDescriptor &desc)
    : _library(library),
      _desc(desc),
      _resourceCache(library.device()) {
}

const MTL::RenderPassDescriptor &RenderPass::renderPassDescriptor() const {
    return _desc;
}

MTL::Library &RenderPass::library() {
    return _library;
}

void RenderPass::setGUI(GUI *gui) {
    _gui = gui;
}

//MARK: - Subpass
void RenderPass::draw(MTL::CommandBuffer &commandBuffer,
                      std::optional<std::string> label) {
    assert(!_subpasses.empty() && "Render pipeline should contain at least one sub-pass");

    std::shared_ptr<MTL::RenderCommandEncoder> encoder = CLONE_METAL_CUSTOM_DELETER(MTL::RenderCommandEncoder,
                                                                                    commandBuffer.renderCommandEncoder(&_desc));
    if (label) {
        encoder->setLabel(NS::String::string(label.value().c_str(), NS::StringEncoding::UTF8StringEncoding));
    }
    for (size_t i = 0; i < _subpasses.size(); ++i) {
        _activeSubpassIndex = i;
        _subpasses[i]->draw(*encoder);
    }
    _activeSubpassIndex = 0;

    if (_gui) {
        ImDrawData *drawData = ImGui::GetDrawData();
        if (drawData) {
            encoder->pushDebugGroup(NS::String::string("GUI Rendering", NS::StringEncoding::UTF8StringEncoding));
            _gui->newFrame(_desc);
            _gui->draw(drawData, commandBuffer, *encoder);
            encoder->popDebugGroup();
        }
    }
    encoder->endEncoding();
}

void RenderPass::addSubpass(std::unique_ptr<Subpass> &&subpass) {
    subpass->setRenderPass(this);
    _subpasses.emplace_back(std::move(subpass));
}

std::vector<std::unique_ptr<Subpass>> &RenderPass::subpasses() {
    return _subpasses;
}

std::unique_ptr<Subpass> &RenderPass::activeSubpass() {
    return _subpasses[_activeSubpassIndex];
}

//MARK: - ParentPass
void RenderPass::addParentPass(const std::string &name, RenderPass *pass) {
    auto iter = _parentPass.find(name);
    if (iter == _parentPass.end()) {
        _parentPass[name] = pass;
    } else {
        LOGW("already exists")
    }
}

RenderPass *RenderPass::removeParentPass(const std::string &name) {
    auto iter = _parentPass.find(name);
    if (iter != _parentPass.end()) {
        auto pass = iter->second;
        _parentPass.erase(iter);
        return pass;
    } else {
        LOGW("can't find pass")
        return nullptr;
    }
}

RenderPass *RenderPass::findPass(const std::string &name) {
    auto iter = _parentPass.find(name);
    if (iter != _parentPass.end()) {
        return iter->second;
    } else {
        return nullptr;
    }
}

void RenderPass::clearParentPass() {
    _parentPass.clear();
}

ResourceCache &RenderPass::resourceCache() {
    return _resourceCache;
}

}// namespace vox
