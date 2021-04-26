#include "GBuffer.h"

void GBuffer::Init(unsigned width, unsigned height)
{
	//Stores window width and height
	_windowWidth = width;
	_windowHeight = height;

	//Adds color targets to our gbuffer
	_gBuffer.AddColorTarget(GL_RGBA8);		//Albedo Buffer, needs GL_RGBA8
	_gBuffer.AddColorTarget(GL_RGB8);		//Normals Buffer, doesn't need Alpha channel
	_gBuffer.AddColorTarget(GL_RGB8);			//specular buffer, only needs 1 channel

	//Important note, technically you can obtain the positional data using the depth buffer (there are calculations you can do), but in this case we're
	//going to use a POSITION buffer
	_gBuffer.AddColorTarget(GL_RGB32F);		//Stores the positional data of the scene, actually NEEDS more data storage, so we use GL_RGB32F

	//Adds our depth buffer
	_gBuffer.AddDepthTarget();

	//Inits our framebuffer
	_gBuffer.Init(width, height);

	//Initializes pass through shader;
	_passThrough = Shader::Create();
	_passThrough->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_passThrough->LoadShaderPartFromFile("shaders/passthrough_frag.glsl", GL_FRAGMENT_SHADER);
	_passThrough->Link();
}

void GBuffer::Bind()
{
	_gBuffer.Bind();
}

void GBuffer::BindLighting()
{
	_gBuffer.BindColorAsTexture(Target::ALBEDO, 0);
	_gBuffer.BindColorAsTexture(Target::NORMAL, 1);
	_gBuffer.BindColorAsTexture(Target::SPECULAR, 2);
	_gBuffer.BindColorAsTexture(Target::POSITION, 3);
}

void GBuffer::BindDepthAsTexture(int textureSlot)
{
	_gBuffer.BindDepthAsTexture(textureSlot);
}

void GBuffer::Clear()
{
	_gBuffer.Clear();
}

void GBuffer::Unbind()
{
	_gBuffer.Unbind();
}

void GBuffer::UnbindLighting()
{
	_gBuffer.UnbindTexture(0);
	_gBuffer.UnbindTexture(1);
	_gBuffer.UnbindTexture(2);
	_gBuffer.UnbindTexture(3);
}

void GBuffer::UnbindTexture(int textureSlot)
{
	_gBuffer.UnbindTexture(textureSlot);
}

void GBuffer::DrawBuffersToScreen()
{
	//Binds our Passthrough shader
	_passThrough->Bind();

	//Set viewport to top left corner
	glViewport(0, _windowHeight / 2.f, _windowWidth / 2.f, _windowHeight / 2.f);
	_gBuffer.BindColorAsTexture(Target::ALBEDO, 0);
	_gBuffer.DrawFullscreenQuad();
	_gBuffer.UnbindTexture(0);

	
	//Set viewport to the top right
	glViewport(_windowWidth / 2.f, _windowHeight / 2.f, _windowWidth / 2.f, _windowHeight / 2.f);
	_gBuffer.BindColorAsTexture(Target::NORMAL, 0);
	_gBuffer.DrawFullscreenQuad();
	_gBuffer.UnbindTexture(0);

	
	//Set viewport to the bottom left
	glViewport(0, 0, _windowWidth / 2.f, _windowHeight / 2.f);
	_gBuffer.BindColorAsTexture(Target::SPECULAR, 0);
	_gBuffer.DrawFullscreenQuad();
	_gBuffer.UnbindTexture(0);

	//Set viewport to the bottom right
	glViewport(_windowWidth / 2.f, 0, _windowWidth / 2.f, _windowHeight / 2.f);
	_gBuffer.BindColorAsTexture(Target::POSITION, 0);
	_gBuffer.DrawFullscreenQuad();
	_gBuffer.UnbindTexture(0);

	//Unbinds our Passthrough shader
	_passThrough->UnBind();
}

void GBuffer::Reshape(unsigned width, unsigned height)
{
	//Updates the saved width and height values
	_windowWidth = width;
	_windowHeight = height;

	//Reshapes the framebuffer with the new width and height
	_gBuffer.Reshape(width, height);
}
