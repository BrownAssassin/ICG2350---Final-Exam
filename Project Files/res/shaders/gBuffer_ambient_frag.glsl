#version 420

layout(location = 0) in vec2 inUV;

struct DirectionalLight
{
	//Light direction (defaults to down, to the left, and a little forward)
	vec4 _lightDirection;

	//Generic Light controls
	vec4 _lightCol;

	//Ambience controls
	vec4 _ambientCol;
	float _ambientPow;
	
	//Power controls
	float _lightAmbientPow;
	float _lightSpecularPow;
	
	float _shadowBias;
};

layout (std140, binding = 0) uniform u_Light
{
	DirectionalLight light;
};

layout (binding = 0) uniform sampler2D s_albedoTex;
layout (binding = 4) uniform sampler2D s_lightAccumTex;
layout (binding = 5) uniform sampler2D s_skybox;

uniform float lighting;
uniform float ambientLight;
uniform float t;

out vec4 frag_color;

void main() 
{
	//Albedo
	vec4 textureColor = texture(s_albedoTex, inUV);
	//Lights
	vec4 lightAccum = texture(s_lightAccumTex, inUV);
	//Skybox
	vec4 skybox = texture(s_skybox, inUV);

	// Lecture 5
	vec3 ambient = light._lightAmbientPow * light._ambientCol.rgb;
	vec3 result;
	if(t == 1)
	{
		//The result of all the lighting
		result = (ambient + lightAccum.rgb) * textureColor.rgb;
	}
	else
	{
		result = (ambient + lightAccum.rgb);
	}

	result = result * skybox.rgb * lighting;

	//The light accumulation
	frag_color = vec4(result, 1.0);
}