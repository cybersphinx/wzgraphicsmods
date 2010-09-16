/*
	Copyright 2010 Warzone 2100 Project

	This file is part of WMIT.

	WMIT is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WMIT is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with WMIT.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QWZM_HPP
#define QWZM_HPP

#include "WZM.hpp"

#include <GL/gl.h>

#include "IGLRenderable.hpp"
#include "TextureAccess.hpp"
#include "IAnimatable.hpp"

class IGLTextureManager;

class QWZM : public WZM, public IGLRenderable, public IAnimatable, protected TextureAccess
{
public:
    QWZM();
	QWZM(const WZM& wzm);
	QWZM(const Pie3Model& p3);

	void render();
	void animate();

	void setRenderTexture(std::string fileName);
	void setTextureManager(IGLTextureManager * manager);

	void setScaleXYZ(GLfloat xyz);
	void setScaleX(GLfloat x);
	void setScaleY(GLfloat y);
	void setScaleZ(GLfloat z);
	void reverseWindings();

private:
	void defaultConstructor();

	GLuint m_texture;

	GLfloat scale_all, scale_xyz[3];
	GLint winding;

	bool forsythInitialized;
};

#endif // QWZM_HPP
