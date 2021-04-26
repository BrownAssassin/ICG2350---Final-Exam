#version 420

layout (location = 0) in vec2 inUV;

out vec4 frag_color;

layout (binding = 0) uniform sampler2D s_screenTex;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main() {
    vec2 textureOffset = 1.0f / textureSize(s_screenTex, 0);
    vec3 result = texture(s_screenTex, inUV).rgb * weight[0];

    if (horizontal) {
        for (int i = 1; i < 5; ++i) {
            result += texture(s_screenTex, inUV + vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(s_screenTex, inUV - vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else {
        for (int i = 1; i < 5; ++i) {
            result += texture(s_screenTex, inUV + vec2(0.0, textureOffset.y * i)).rgb * weight[i];
            result += texture(s_screenTex, inUV - vec2(0.0, textureOffset.y * i)).rgb * weight[i];
        }
    }

    frag_color = vec4(result, 1.0);
}