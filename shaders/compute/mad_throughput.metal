//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;

constant int kLoopSize [[function_constant(0)]];

kernel void mad_throughput(float4* inputA [[buffer(0)]],
                           float4* inputB [[buffer(1)]],
                           float4* output [[buffer(2)]],
                           uint3 tpig [[ thread_position_in_grid ]]) {
    float4 a = inputA.x[tpig.x];
    float4 b = inputB.x[tpig.x];
    float4 c = float4(1.f, 1.f, 1.f, 1.f);
    for(int i = 0; i < kLoopSize; i++) {
      c = a * c + b;
      c = a * c + b;
      c = a * c + b;
      c = a * c + b;
      c = a * c + b;
      c = a * c + b;
      c = a * c + b;
      c = a * c + b;
      c = a * c + b;
      c = a * c + b;
    }
    output.x[tpig.x] = c;
}
