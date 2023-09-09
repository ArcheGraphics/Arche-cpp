//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gpu_capture.h"
#include "common/metal_helpers.h"
#include "common/logging.h"

namespace vox::compute {
std::shared_ptr<MTL::CaptureScope> create_capture_scope(const std::string &name, MTL::Device &device) {
    auto scope = make_shared(MTL::CaptureManager::sharedCaptureManager()->newCaptureScope(&device));
    scope->setLabel(NS::String::string(name.c_str(), NS::UTF8StringEncoding));

    auto pCaptureDescriptor = make_shared(MTL::CaptureDescriptor::alloc()->init());
    pCaptureDescriptor->setDestination(MTL::CaptureDestinationDeveloperTools);
    pCaptureDescriptor->setCaptureObject(scope.get());

    NS::Error *error{nullptr};
    MTL::CaptureManager::sharedCaptureManager()->startCapture(pCaptureDescriptor.get(), &error);
    if (error != nullptr) {
        LOGE("Error: could not create scope: {}",
             error->description()->cString(NS::StringEncoding::UTF8StringEncoding));
    }
    return scope;
}

}// namespace vox::compute