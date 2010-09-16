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

#include <QPixmap>
#include <QImage>
#include <QApplication>

#include <QGLViewer/vec.h>

#include "QtGLView.hpp"

QtGLView::QtGLView(QWidget *parent) :
		QGLViewer(parent)
{
	connect(&textureUpdater, SIGNAL(fileChanged(QString)), this, SLOT(textureChanged(QString)));

	setShortcut(DISPLAY_FPS, 0); // Disable stuff that won't work.
	setShortcut(DRAW_AXIS, 0);
	setShortcut(DRAW_GRID, 0);
}

QtGLView::~QtGLView()
{
	foreach (ManagedGLTexture texture, m_textures)
	{
		const GLuint id = texture.id();
		QGLWidget::deleteTexture(id);
	}
}

void QtGLView::init()
{
	glDisable(GL_LIGHTING); // QGLViewer likes enabling this stuff
	glDisable(GL_COLOR_MATERIAL);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.05f);

	glEnable(GL_CULL_FACE);

	setSceneRadius(2);

	camera()->setPosition(qglviewer::Vec(0.5 * 2, 2.12 * 2, -2.12 * 2));
	camera()->setViewDirection(qglviewer::Vec(-0.5, -2.12, 2.12));
}

void QtGLView::draw()
{
	foreach(IGLRenderable* obj, renderList)
	{
		obj->render();
	}
}

void QtGLView::postDraw()
{
	glDisable(GL_TEXTURE_2D);

	/* Grid begin - Copied from QGLViewer source then modified */
	glColor3f(.7f, 1.f, .7f);
	glPushMatrix();

	glRotatef(90.0f, 1.f, 0.f, 0.f);
	const int subdivisions = 3;
	const float halfSize = subdivisions/2.f;
	glBegin(GL_LINES);
	for (int i=0; i <= subdivisions; ++i)
	{
		const float pos = i - halfSize;
		glVertex2f(pos, -halfSize); // vertical
		glVertex2f(pos, +halfSize);

		// horizontal
		glVertex2f(-halfSize, pos); // |   |    | |  |_|_
		glVertex2f( halfSize, pos); // |   |___ |_|_ |_|_
	}
	glEnd();
	glColor3f(1.f, 1.f, 1.f);
	glPopMatrix();

	/* Grid end
	 * Axis begin  - Copied from QGLViewer source then modified */
	const float length =camera()->sceneRadius();
	const float charWidth = length / 40.0;
	const float charHeight = length / 30.0;
	const float charShift = 1.04 * length;

	GLboolean lighting;
	glGetBooleanv(GL_LIGHTING, &lighting);

	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2);

#define swap(a,b) a,b // Swap the qglviewer code so that it fits the WZ axis
	glBegin(GL_LINES);
	// The X
	glVertex3f(-charShift,  charWidth, -charHeight);
	glVertex3f(-charShift, -charWidth,  charHeight);
	glVertex3f(-charShift, -charWidth, -charHeight);
	glVertex3f(-charShift,  charWidth,  charHeight);
	// The Y
	glVertex3f( charWidth, swap(charShift, charHeight));
	glVertex3f(0.0,        swap(charShift, 0.0));
	glVertex3f(-charWidth, swap(charShift, charHeight));
	glVertex3f(0.0,        swap(charShift, 0.0));
	glVertex3f(0.0,        swap(charShift, 0.0));
	glVertex3f(0.0,        swap(charShift, -charHeight));
	// The Z
	glVertex3f(-charWidth,  swap(charHeight, charShift));
	glVertex3f( charWidth,  swap(charHeight, charShift));
	glVertex3f( charWidth,  swap(charHeight, charShift));
	glVertex3f(-charWidth, swap(-charHeight, charShift));
	glVertex3f(-charWidth, swap(-charHeight, charShift));
	glVertex3f( charWidth, swap(-charHeight, charShift));
	glEnd();
#undef swap

	glEnable(GL_LIGHTING);

	float color[4];
	color[0] = 0.7f;  color[1] = 0.7f;  color[2] = 1.0f;  color[3] = 1.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	QGLViewer::drawArrow(length, 0.01*length);

	color[0] = 1.0f;  color[1] = 0.7f;  color[2] = 0.7f;  color[3] = 1.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	glPushMatrix();
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	QGLViewer::drawArrow(length, 0.01*length);
	glPopMatrix();

	color[0] = 0.7f;  color[1] = 1.0f;  color[2] = 0.7f;  color[3] = 1.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	QGLViewer::drawArrow(length, 0.01*length);
	glPopMatrix();

	if (!lighting)
		glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

void QtGLView::addToRenderList(IGLRenderable* object)
{
	renderList.append(object);
}

void QtGLView::removeFromRenderList(IGLRenderable* object)
{
	int index = renderList.indexOf(object, 0);
	if (index != -1)
	{
		renderList.removeAt(index);
	}
}

void QtGLView::clearRenderList()
{
	renderList.clear();
}

GLTexture QtGLView::createTexture(const QString& fileName)
{
	if (!fileName.isEmpty())
	{
		t_texIt texIt = m_textures.find(fileName);
		if (texIt == m_textures.end())
		{
			QPixmap pixmap(fileName);
			ManagedGLTexture texture(QGLWidget::bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, QGLContext::LinearFilteringBindOption),
									 pixmap.width(),
									 pixmap.height());

			m_textures.insert(fileName, texture);

			textureUpdater.addPath(fileName);

			// Remove some old textures now
			for (texIt = m_textures.begin(); texIt != m_textures.end(); ++texIt)
			{
				if (m_textures.size() > 2 && texIt->users == 0)
				{
					textureUpdater.removePath(texIt.key());
					QGLWidget::deleteTexture(texIt.value().id());
					m_textures.remove(texIt.key());
				}
			}

			return texture;
		}
		else
		{
			texIt.value().users++;
			return texIt.value();
		}
	}
	return GLTexture(0, 0, 0);
}

GLTexture QtGLView::bindTexture(const QString &fileName)
{
	return createTexture(fileName);
}

QString QtGLView::idToFilePath(GLuint id)
{
	t_texIt texIt;
	for (texIt = m_textures.begin(); texIt != m_textures.end(); ++texIt)
	{
		if (texIt->id() == id)
		{
			return texIt.key();
		}
	}
	return QString();
}

void QtGLView::deleteTexture(GLuint id)
{
	t_texIt texIt;
	for (texIt = m_textures.begin(); texIt != m_textures.end(); ++texIt)
	{
		if (texIt->id() == id)
		{
			texIt->users = std::min(texIt->users - 1, 0);
			if ( m_textures.size() > 2 && texIt->users == 0)
			{
				textureUpdater.removePath(texIt.key());
				QGLWidget::deleteTexture(id);
				texIt = m_textures.erase(texIt);
			}
			break;
		}
	}
}

void QtGLView::deleteTexture(const QString& fileName)
{
	t_texIt texIt = m_textures.find(fileName);
	if (texIt != m_textures.end())
	{
		texIt->users = std::min(texIt->users - 1, 0);
		if (m_textures.size() > 2 && texIt->users == 0)
		{
			textureUpdater.removePath(texIt.key());
			QGLWidget::deleteTexture(texIt.value().id());
			texIt = m_textures.erase(texIt);
		}
	}
}

void QtGLView::deleteAllTextures()
{
	t_texIt texIt;
	for (texIt = m_textures.begin(); texIt != m_textures.end(); ++texIt)
	{
		textureUpdater.removePath(texIt.key());
		QGLWidget::deleteTexture(texIt.value().id());
		texIt = m_textures.erase(texIt);
	}
	m_textures.clear();
}

void QtGLView::textureChanged(const QString& fileName)
{
	t_texIt texIt = m_textures.find(fileName);
	if (texIt != m_textures.constEnd())
	{
		texIt.value().update = true;
		updateTimer.start(800, this);;
	}
}

void QtGLView::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == updateTimer.timerId())
	{
		updateTimer.stop();
		updateTextures();
	}
	else
	{
		QGLViewer::timerEvent(event);
	}
}

void QtGLView::updateTextures() const
{
	t_texIt texIt;
	for (texIt = m_textures.begin(); texIt != m_textures.end(); ++texIt)
	{
		if (texIt.value().update)
		{
			QImage image(texIt.key());
			texIt.value().update = false;
			if (!image.isNull())
			{
				image = convertToGLFormat(image);
				glBindTexture(GL_TEXTURE_2D, texIt.value().id());
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
			}
		}
	}
}

