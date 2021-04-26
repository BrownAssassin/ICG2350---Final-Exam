#version 420

layout (location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;

uniform float width;
uniform float height;

void main() {
    vec2 pixelOffset = vec2(1.0f / width, 1.0f / height);

    vec3 downScaledFrag = vec3(0.0f);
    downScaledFrag += texture(s_screenTex, vec2(inUV.x - 16.0f * pixelOffset.x, inUV.y)).xyz;
    downScaledFrag += texture(s_screenTex, vec2(inUV.x + 16.0f * pixelOffset.x, inUV.y)).xyz;
    downScaledFrag += texture(s_screenTex, vec2(inUV.x, inUV.y - 16.0f * pixelOffset.y)).xyz;
    downScaledFrag += texture(s_screenTex, vec2(inUV.x, inUV.y + 16.0f * pixelOffset.y)).xyz;
    downScaledFrag *= 0.25f;

    frag_color = vec4(downScaledFrag, 1.0f);
}