//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shader_common_h
#define shader_common_h

#import <simd/simd.h>

typedef enum {
    Position = 0,
    Normal = 1,
    UV_0 = 2,
    Tangent = 3,
    Bitangent = 4,
    Color_0 = 5,
    Weights_0 = 6,
    Joints_0 = 7,
    UV_1 = 8,
    UV_2 = 9,
    UV_3 = 10,
    UV_4 = 11,
    UV_5 = 12,
    UV_6 = 13,
    UV_7 = 14,
} Attributes;

struct EnvMapLight {
    vector_float3 diffuse;
    float diffuseIntensity;
    float specularIntensity;
    int mipMapLevel;
    matrix_float4x4 transformMatrix;
};

struct PointLightData {
    vector_float3 color;
    vector_float3 position;
    float distance;
};

struct SpotLightData {
    vector_float3 color;
    vector_float3 position;
    vector_float3 direction;
    float distance;
    float angleCos;
    float penumbraCos;
};

struct DirectLightData {
    vector_float3 color;
    vector_float3 direction;
};

struct ShadowData {
    /**
     * Shadow bias.
     */
    float bias = 0.005;
    
    /**
     * Shadow intensity, the larger the value, the clearer and darker the shadow.
     */
    float intensity = 0.2;
    
    /**
     * Pixel range used for shadow PCF interpolation.
     */
    float radius = 1;
    
    /** Alignment */
    float dump = 0;
    
    /**
     * Light view projection matrix.(cascade)
     */
    matrix_float4x4 vp[4];
    
    /**
     * Light cascade depth.
     */
    float cascadeSplits[4];
};

struct CubeShadowData {
    /**
     * Shadow bias.
     */
    float bias = 0.005;
    
    /**
     * Shadow intensity, the larger the value, the clearer and darker the shadow.
     */
    float intensity = 0.2;
    
    /**
     * Pixel range used for shadow PCF interpolation.
     */
    float radius = 1;
    
    /** Alignment */
    float dump = 0;
    
    /**
     * Light view projection matrix.(cascade)
     */
    matrix_float4x4 vp[6];
    
    vector_float3 lightPos;
};

#endif /* shader_common_h */
