/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Vertex and fragment shaders for blitting a texture to the view.
*/

#include <metal_stdlib>
using namespace metal;

struct VertexOut
{
    float4 position [[ position ]];
    float2 texcoord;
};

vertex VertexOut vtxBlit(uint vid [[vertex_id]])
{
    // These vertices map a triangle to cover a fullscreen quad.
    const float4 vertices[] = {
        float4(-1, -1, 1, 1), // bottom left
        float4(3, -1, 1, 1),  // bottom right
        float4(-1, 3, 1, 1),  // upper left
    };
    
    const float2 texcoords[] = {
        float2(0.0, 0.0), // bottom left
        float2(2.0, 0.0), // bottom right
        float2(0.0, 2.0), // upper left
    };
    
    VertexOut out;
    out.position = vertices[vid];
    out.texcoord = texcoords[vid];
    return out;
}

fragment half4 fragBlitLinear(VertexOut in [[stage_in]], texture2d<float> tex[[texture(0)]])
{
    constexpr sampler s = sampler(address::clamp_to_edge);
    
    float4 pixel = tex.sample(s, in.texcoord);
    return half4(pixel);
}
