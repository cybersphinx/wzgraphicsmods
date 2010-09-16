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

#ifndef QTGLVIEW_HPP
#define QTGLVIEW_HPP

#include <QString>
#include <QList>
#include <QHash>
#include <QBasicTimer>
#include <QFileSystemWatcher>

#include <QGLViewer/qglviewer.h>

#include "GLTexture.hpp"
#include "IGLTextureManager.hpp"
#include "IGLRenderable.hpp"

class QtGLView : public QGLViewer , public IGLTextureManager
{
	Q_OBJECT
public:
	explicit QtGLView(QWidget *parent = 0);
	~QtGLView();

	void draw();
	void postDraw();

	void addToRenderList(IGLRenderable* object);
	void removeFromRenderList(IGLRenderable* object);
	void clearRenderList();

	/// GLTextureManager components
	GLTexture createTexture(const QString& fileName);
	GLTexture bindTexture(const QString& fileName); // We're hiding a few QGLWidget functions on purpose
	virtual QString idToFilePath(GLuint id);
	void deleteTexture(GLuint id);
	void deleteTexture(const QString& fileName);
	void deleteAllTextures();

protected:

	void init();

	void timerEvent(QTimerEvent* event);

	struct ManagedGLTexture : public GLTexture
	{
		int users;
		bool update;
		ManagedGLTexture(GLuint id, GLsizei w, GLsizei h):
				GLTexture(id, w, h), users(1), update(false){}

		virtual ~ManagedGLTexture(){}
	};

private:

	QList<IGLRenderable*> renderList;

	/// GLTextureManager components
	mutable QHash<QString, ManagedGLTexture> m_textures;
	typedef QHash<QString, ManagedGLTexture>::iterator t_texIt;
	typedef QHash<QString, ManagedGLTexture>::const_iterator t_cTexIt;

	void updateTextures() const;
	void _deleteTexture(t_texIt& texIt);

	QFileSystemWatcher textureUpdater;
	QBasicTimer updateTimer;

private slots:
	void textureChanged(const QString& fileName);
};

#endif // QTGLVIEW_HPP
