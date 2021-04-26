#include "DepthOfField.h"

void DepthOfFieldEffect::Init(unsigned width, unsigned height)
{
    _width = width;
    _height = height;

    int index = int(_buffers.size());
    _buffers.push_back(new Framebuffer());
    _buffers[index]->AddColorTarget(GL_RGBA8);
    _buffers[index]->AddDepthTarget();
    _buffers[index]->Init(width, height);

    //Loads the shaders
    index = int(_shaders.size());
    _shaders.push_back(Shader::Create());
    _shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
    _shaders[index]->LoadShaderPartFromFile("shaders/Post/downscale_frag.glsl", GL_FRAGMENT_SHADER);
    _shaders[index]->Link();

    index++;
    _shaders.push_back(Shader::Create());
    _shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
    _shaders[index]->LoadShaderPartFromFile("shaders/Post/gaussian_blur_frag.glsl", GL_FRAGMENT_SHADER);
    _shaders[index]->Link();

    index++;
    _shaders.push_back(Shader::Create());
    _shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
    _shaders[index]->LoadShaderPartFromFile("shaders/Post/dof_composite_frag.glsl", GL_FRAGMENT_SHADER);
    _shaders[index]->Link();

    _shaders.push_back(Shader::Create());
    _shaders[_shaders.size() - 1]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
    _shaders[_shaders.size() - 1]->LoadShaderPartFromFile("shaders/passthrough_frag.glsl", GL_FRAGMENT_SHADER);
    _shaders[_shaders.size() - 1]->Link();
}

void DepthOfFieldEffect::ApplyEffect(PostEffect* buffer)
{
    // Downscale
    BindShader(0);

    _shaders[0]->SetUniform("width", _width);
    _shaders[0]->SetUniform("height", _height);

    buffer->BindColorAsTexture(0, 0, 0);

    _buffers[0]->RenderToFSQ();

    buffer->UnbindTexture(0);

    UnbindShader();

    // 3-Pass Gaussian Blur
    bool horizontal = true;

    BindShader(1);

    for (unsigned int i = 0; i < 15; i++) {
        _shaders[1]->SetUniform("horizontal", horizontal);

        BindColorAsTexture(0, 0, 0);

        _buffers[0]->RenderToFSQ();

        UnbindTexture(0);

        horizontal != horizontal;
    }
    
    UnbindShader();

    // Composite
    BindShader(2);

    _shaders[2]->SetUniform("intensity", _intensity);
    _shaders[2]->SetUniform("planeInFocus", _planeInFocus);
    _shaders[2]->SetUniform("focalLength", _focalLength);
    _shaders[2]->SetUniform("aperture", _aperture);

    buffer->BindColorAsTexture(0, 0, 0);
    BindColorAsTexture(0, 0, 1);

    _buffers[0]->RenderToFSQ();

    UnbindTexture(0);
    UnbindTexture(1);

    UnbindShader();
}

int DepthOfFieldEffect::GetIntensity() const
{
	return _intensity;
}

float DepthOfFieldEffect::GetPlaneInFocus() const
{
    return _planeInFocus;
}

float DepthOfFieldEffect::GetFocalLength() const
{
    return _focalLength;
}

float DepthOfFieldEffect::GetAperture() const
{
    return _aperture;
}

void DepthOfFieldEffect::SetIntensity(bool intensity)
{
    _intensity = intensity;
}

void DepthOfFieldEffect::SetPlaneInFocus(float planeInFocus)
{
    _planeInFocus = planeInFocus;
}

void DepthOfFieldEffect::SetFocalLength(float focalLength)
{
    _focalLength = focalLength;
}

void DepthOfFieldEffect::SetAperture(float aperture)
{
    _aperture = aperture;
}
