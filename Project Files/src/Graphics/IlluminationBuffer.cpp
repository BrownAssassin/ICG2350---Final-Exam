#include "IlluminationBuffer.h"

void IlluminationBuffer::Init(unsigned width, unsigned height)
{
	//Ambient + Illum Buffer
	int index = int(_buffers.size());
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->AddDepthTarget();
	_buffers[index]->Init(width, height);

	//IllumBuf 1
	index = int(_buffers.size());
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->AddDepthTarget();
	_buffers[index]->Init(width, height);

	//Loads the directional shader
	index = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/gBuffer_directional_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	//Loads the point light shader
	index = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/gBuffer_ambient_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	//Allocate the memory for the buffers
	_sunBuffer.AllocateMemory(sizeof(DirectionalLight));

	if (_sunEnabled)
	{
		//We are just assuming there's a sun
		_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));
	}

	PostEffect::Init(width, height);
}

void IlluminationBuffer::ApplyEffect(GBuffer* gBuffer)
{
	glDisable(GL_DEPTH_TEST);
	
	//If the sun is enabled
	if (_sunEnabled)
	{
		//Binds the directional light shader
		_shaders[Lights::DIRECTIONAL]->Bind();
		_shaders[Lights::DIRECTIONAL]->SetUniformMatrix("u_LightSpaceMatrix", _lightSpaceViewProj);
		_shaders[Lights::DIRECTIONAL]->SetUniform("u_CamPos", _camPos);
		_shaders[Lights::DIRECTIONAL]->SetUniform("lighting", lighting); 
		_shaders[Lights::DIRECTIONAL]->SetUniform("ambientLight", ambientLight);
		_shaders[Lights::DIRECTIONAL]->SetUniform("diffuseLight", diffuseLight);
		_shaders[Lights::DIRECTIONAL]->SetUniform("specularLight", specularLight);
		

		//Send the directional light data to the uniform buffer
		_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));
		_sunBuffer.Bind(0);

		gBuffer->BindLighting();

		//Binds and draws to the illumination buffer
		_buffers[1]->RenderToFSQ();

		gBuffer->UnbindLighting();

		//Unbinds the uniform buffer
		_sunBuffer.Unbind(0);

		//Unbind directional light shader
		_shaders[Lights::DIRECTIONAL]->UnBind();
	}

	//By the end whatever is readBuf is our final buffer
	_shaders[Lights::AMBIENT]->Bind();
	_shaders[Lights::AMBIENT]->SetUniform("lighting", lighting);
	_shaders[Lights::AMBIENT]->SetUniform("ambientLight", ambientLight);
	_shaders[Lights::AMBIENT]->SetUniform("t", tex);


	//Send the directional light data to the uniform buffer
	_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(DirectionalLight));
	_sunBuffer.Bind(0);

	gBuffer->BindLighting();
	_buffers[1]->BindColorAsTexture(0, 4);
	_buffers[0]->BindColorAsTexture(0, 5);

	_buffers[0]->RenderToFSQ();

	_buffers[0]->UnbindTexture(5);
	_buffers[1]->UnbindTexture(4);
	gBuffer->UnbindLighting();

	//Unbinds the uniform buffer
	_sunBuffer.Unbind(0);

	_shaders[Lights::AMBIENT]->UnBind();

	glEnable(GL_DEPTH_TEST);
}

void IlluminationBuffer::DrawIllumBuffer()
{
	_shaders[_shaders.size() - 1]->Bind();

	_buffers[1]->BindColorAsTexture(0, 0);

	Framebuffer::DrawFullscreenQuad();

	_buffers[1]->UnbindTexture(0);

	_shaders[_shaders.size() - 1]->UnBind();
}

void IlluminationBuffer::SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj)
{
	_lightSpaceViewProj = lightSpaceViewProj;
}

void IlluminationBuffer::SetCamPos(glm::vec3 camPos)
{
	_camPos = camPos;
}

DirectionalLight& IlluminationBuffer::GetSunRef()
{
	return _sun;
}

void IlluminationBuffer::SetSun(DirectionalLight newSun)
{
	_sun = newSun;
}

void IlluminationBuffer::SetSun(glm::vec4 lightDir, glm::vec4 lightCol)
{
	_sun._lightDirection = lightDir;
	_sun._lightCol = lightCol;
}

void IlluminationBuffer::EnableSun(bool enabled)
{
	_sunEnabled = enabled;
}

void IlluminationBuffer::SetLit(bool value)
{
	lighting = value;
}

void IlluminationBuffer::SetAmb(bool value)
{
	ambientLight = value;
}

void IlluminationBuffer::SetDif(bool value)
{
	diffuseLight = value;
}

void IlluminationBuffer::SetSpec(bool value)
{
	specularLight = value;
}

void IlluminationBuffer::SetTex(bool value)
{
	tex = value;
}
