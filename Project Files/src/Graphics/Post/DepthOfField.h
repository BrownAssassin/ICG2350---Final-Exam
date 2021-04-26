#pragma once

#include "Graphics/Post/PostEffect.h"

class DepthOfFieldEffect : public PostEffect
{
public:
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;

	//Applies the effect to this buffer
	//passes the previous framebuffer with the texture to apply as parameter
	void ApplyEffect(PostEffect* buffer) override;

	//Getters
	int GetIntensity() const;
	float GetPlaneInFocus() const;
	float GetFocalLength() const;
	float GetAperture() const;

	//Setters
	void SetIntensity(bool intensity);
	void SetPlaneInFocus(float planeInFocus);
	void SetFocalLength(float focalLength);
	void SetAperture(float aperture);

private:
	bool _intensity = true;
	float _planeInFocus = 0.9675f;
	float _focalLength = 85.0f;
	float _aperture = 1.4f;
	float _width = 0.0f, _height = 0.0f;
};