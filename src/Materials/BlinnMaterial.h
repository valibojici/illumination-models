#pragma once
#include "PhongMaterial.h"

/// <summary>
/// Class for a Blinn-Phong material, same as Phong
/// </summary>
class BlinnMaterial : public PhongMaterial
{
public:
	BlinnMaterial() : PhongMaterial() { m_alpha *= 4; } // set default alpha to 4 times the original
};

