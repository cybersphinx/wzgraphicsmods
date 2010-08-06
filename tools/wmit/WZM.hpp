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
#ifndef WZM_HPP
#define WZM_HPP

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <GL/gl.h>
#include "VectorTypes.hpp"
#include "Polygon.hpp"

#ifdef __GNUC__
# ifdef WARNMORE
#  pragma  GCC diagnostic warning "-Weffc++"
#  pragma  GCC diagnostic warning "-Wconversion"
# endif
#endif

typedef Vertex<GLfloat> WZMVertex;
typedef UV<GLclampf> WZMUV;
typedef std::vector<UV<GLclampf> > TexArray;

class WZMConnector
{
public:
	WZMConnector(){}
	virtual ~WZMConnector(){}
private:
	Vertex<GLfloat> m_pos;
	///TODO: Types for wzm connectors
#pragma message "TODO"
};

struct Frame
{
	GLfloat time;
	GLfloat xTrans, yTrans, zTrans;
	GLfloat xRot, yRot, zRot;
};

typedef Vertex<GLfloat> OBJVertex;
typedef UV<GLclampf> OBJUV;

void writeOBJVertex(const OBJVertex& vert, std::ostream& out);
void writeOBJUV(const OBJUV& uv, std::ostream& out);

class Pie3Level;
class Lib3dsMesh;
struct Mesh_exportToOBJ_InOutParams;
class Mesh
{
public:
	Mesh();
	Mesh(const Pie3Level& p3, double uvEps = 0.0001, double vertEps = -1);
	Mesh(const Lib3dsMesh& mesh3ds);
	virtual ~Mesh();

	static Pie3Level backConvert(const Mesh& wzmMesh);
	virtual operator Pie3Level() const;

	bool read(std::istream& in);
	void write(std::ostream& out) const;

	bool importFromOBJ(const std::vector<OBJTri>&	faces,
					   const std::vector<OBJVertex>& verts,
					   const std::vector<OBJUV>&	uvArray);

	std::stringstream* exportToOBJ(const Mesh_exportToOBJ_InOutParams& params) const;

	std::string getName() const;
	void setName(const std::string& name);

	bool teamColours() const;
	void setTeamColours(bool tc);

	WZMConnector& getConnector(int index);
	void addConnector (const WZMConnector& conn);
	void rmConnector (int index);
	int connectors() const;


	int textureArrays() const;
	const TexArray& getTexArray (int index) const;
//	void addTexArray (const TexArray& tex, int index);
//	void rmTexArray(int index);
//	void replaceTexArray(const TexArray& tex, int index);

	unsigned vertices() const;
	unsigned faces() const;
	unsigned triangles() const;
	unsigned indices() const;
	unsigned frames() const;

	bool isValid() const;
protected:
	void clear();
	std::string m_name;
	std::vector<Frame> m_frameArray;
	std::vector<WZMVertex > m_vertexArray;
	std::vector<TexArray> m_textureArrays;
	std::vector<IndexedTri> m_indexArray;
	std::list<WZMConnector> m_connectors;
	bool m_teamColours;
};

class Pie3Model;

class WZM
{
public:
	WZM();
	WZM(const Pie3Model& p3);
	virtual ~WZM(){}

	virtual operator Pie3Model() const;

	bool read(std::istream& in);
	void write(std::ostream& in) const;

	bool importFromOBJ(std::istream& in);
	void exportToOBJ(std::ostream& out) const;

	bool importFrom3DS(std::string fileName);

	int version() const;
	int meshes() const;

	/// might throw out_of_range exception? not decided yet
	Mesh& getMesh(int index);
	void addMesh (const Mesh& mesh);
	void rmMesh (int index);

	bool isValid() const;

protected:
	void clear();
	std::vector<Mesh> m_meshes;
	std::string m_texName;
};

#endif // WZM_HPP
