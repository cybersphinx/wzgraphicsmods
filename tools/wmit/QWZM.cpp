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

#include "QWZM.hpp"

#include <QString>

#include "IGLTextureManager.hpp"

QWZM::QWZM()
{
	defaultConstructor();
}

QWZM::QWZM(const WZM& wzm)
	: WZM(wzm)
{
	defaultConstructor();
}

QWZM::QWZM(const Pie3Model& p3)
	: WZM(p3)
{
	defaultConstructor();
}

void QWZM::render()
{
	GLint frontFace;
	glGetIntegerv(GL_FRONT_FACE, &frontFace);

	std::vector<Mesh>::iterator it;

	// Basic rendering
	glEnable(GL_TEXTURE_2D);
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glPushMatrix();

	if (frontFace != winding)
	{
		glFrontFace(winding);
	}
	glScalef(-1/128.f, 1/128.f, 1/128.f); // Scale from warzone to fit in our scene. possibly a FIXME

	glScalef(scale_all * scale_xyz[0], scale_all * scale_xyz[1], scale_all * scale_xyz[2]);

	for (it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{

		glTexCoordPointer(2, GL_FLOAT, 0, &it->m_textureArrays[0][0]);
		glVertexPointer(3, GL_FLOAT, 0, &it->m_vertexArray[0]);
		glDrawElements(GL_TRIANGLES, it->m_indexArray.size() * 3, GL_UNSIGNED_SHORT, &it->m_indexArray[0]);
	}
	if (frontFace != winding)
	{
		glFrontFace(GL_CCW);
	}
	glPopMatrix();
	glPopClientAttrib();
}

void QWZM::animate()
{

}

void QWZM::setRenderTexture(std::string fileName)
{
	if (m_texture != 0)
	{
		deleteTexture(m_texture);
	}
	m_texture = createTexture(fileName).id();
}

void QWZM::setTextureManager(IGLTextureManager * manager)
{
	std::string fileName;
	if (m_texture != 0)
	{
		fileName = idToFilePath(m_texture);
		deleteTexture(m_texture);
	}
	TextureAccess::setTextureManager(manager);
	if (m_texture != 0)
	{
		m_texture = createTexture(fileName).id();
	}
}

inline void QWZM::defaultConstructor()
{
	m_texture = 0;
	scale_all = 1.f;
	scale_xyz[0] = 1.f;
	scale_xyz[1] = 1.f;
	scale_xyz[2] = 1.f;
	winding = GL_CW;
	forsythInitialized = false;
}

void QWZM::setScaleXYZ(GLfloat xyz)
{
	scale_all = xyz;
}

void QWZM::setScaleX(GLfloat x)
{
	scale_xyz[0] = x;
}

void QWZM::setScaleY(GLfloat y)
{
	scale_xyz[1] = y;
}

void QWZM::setScaleZ(GLfloat z)
{
	scale_xyz[2] = z;
}

void QWZM::reverseWindings()
{
	if (winding == GL_CCW)
	{
		winding = GL_CW;
	}
	else
	{
		winding = GL_CCW;
	}
}
