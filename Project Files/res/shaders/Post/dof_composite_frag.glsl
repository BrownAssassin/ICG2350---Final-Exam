#version 420

layout (location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_SharpImage;
layout (binding = 1) uniform sampler2D s_BlurBuffer;
layout (binding = 30) uniform sampler2D s_DepthMap;

uniform bool intensity;
uniform float planeInFocus;
uniform float focalLength;
uniform float aperture;
float CoC = -0.483613242f;

float unmix(float a, float b, float v) {
    return (v - a) / (b - a);
}

float Dn(float planeInFocus, float focalLength, float aperture, float CoC) {
    return (planeInFocus * (((focalLength * focalLength) / (aperture * CoC)) - focalLength)) / 
           (((focalLength * focalLength) / (aperture * CoC)) + planeInFocus - (2 * focalLength));
}

float Df(float planeInFocus, float focalLength, float aperture, float CoC) {
    return (planeInFocus * (((focalLength * focalLength) / (aperture * CoC)) - focalLength)) / 
           (((focalLength * focalLength) / (aperture * CoC)) - planeInFocus);
}

void main() {
    float depthValue = texture(s_DepthMap, inUV).r;
    vec4 sharpImage = texture(s_SharpImage, inUV);
    vec4 blurBuffer = texture(s_BlurBuffer, inUV);

    float farPlane = Df(planeInFocus, focalLength, aperture, CoC);
    float nearPlane = Dn(planeInFocus, focalLength, aperture, CoC);

    if (farPlane > 0.9925f) {
        farPlane = 0.9925f;
    }
    if (nearPlane < 0.0075f) {
        nearPlane = 0.0075f;
    }
    
    if (!intensity) {
        if (depthValue >= farPlane) {
            frag_color = mix(sharpImage, blurBuffer, mix(0.0f, 1.0f, unmix(farPlane, 1.0f, depthValue)));
        }
        else if (depthValue <= nearPlane) {
            frag_color = mix(sharpImage, blurBuffer, mix(0.0f, 1.0f, unmix(nearPlane, 0.0f, depthValue)));
        }
        else {
            frag_color = sharpImage;
        }
    }
    else {
        frag_color = sharpImage;
    }
}