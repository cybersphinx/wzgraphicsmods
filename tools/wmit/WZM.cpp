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

#include "WZM.hpp"
#include "Generic.hpp"
#include "Pie.hpp"
#include "Vector.hpp"

#include <algorithm>
#include <iterator>
#include <map>
#include <set>

#include <cmath>

#include <sstream>
#include <cctype> // isalnum

#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>
#include <lib3ds/material.h>

Mesh::Mesh()
{
}

Mesh::Mesh(const Pie3Level& p3, double uvEps, double vertEps)
{
	std::vector<Pie3Polygon>::const_iterator itL;

	WZMVertex::less_wEps compare(vertEps);
	std::multimap<WZMVertex, unsigned, WZMVertex::less_wEps> tmpMMap(compare);

	std::multimap<WZMVertex, unsigned>::iterator itMM;
	std::pair<std::multimap<WZMVertex, unsigned>::iterator,
				std::multimap<WZMVertex, unsigned>::iterator> ret;

	unsigned vIdx;
	unsigned int vert;
	unsigned int frame;
	bool foundMatch;

	/*
	 *	Try to prevent duplicate vertices
	 *	(remember, different UV's, or animations,
	 *	 will cause unavoidable duplication)
	 *	so that our transformed vertex cache isn't
	 *	completely useless.
	 */

	if (m_textureArrays.size() == 0)
	{
		m_textureArrays.push_back(TexArray());
	}

	// For each pie3 polygon
	for (itL = p3.m_polygons.begin(); itL != p3.m_polygons.end(); ++itL)
	{
		IndexedTri iTri;

		// For all 3 vertices of the triangle
		for (vert = 0; vert < 3; ++vert)
		{
			WZMVertex wzmVert = p3.m_points[itL->getIndex(vert)];
			ret = tmpMMap.equal_range(wzmVert);
			foundMatch = false;

			// Only check if match is possible
			if (itL->m_frames <= m_textureArrays.size())
			{
				// For each potential match
				for (itMM = ret.first; itMM != ret.second; ++itMM)
				{
					vIdx = itMM->second;

					// Compare each animation frame
					for (frame = 0; frame < itL->m_frames; ++frame)
					{
						// Approximate comparison, helps kill duplicates
						const WZMUV::equal_wEps compare(uvEps);
						if (!compare(m_textureArrays[frame][vIdx], itL->getUV(vert, frame)))
						{
							break; // Not equal
						}
					}

					// if all frames were equal
					if (!(frame < itL->m_frames))
					{
						foundMatch = true;
						break; // Stop looking
					}
				}
			}

			if (!foundMatch)
			{
				unsigned frames2Do = std::max(itL->m_frames, m_textureArrays.size());
				vIdx = m_vertexArray.size();
				// add the vertex to both the multimap and the vertex array
				m_vertexArray.push_back(wzmVert);
				tmpMMap.insert(std::make_pair(wzmVert, vIdx));

				m_textureArrays.reserve(frames2Do);

				// add the uv's to the texture arrays
				for (frame = 0; frame < frames2Do; ++frame)
				{
					if (m_textureArrays.size() < frame + 1)
					{
						// Expand the texture array arrays
						m_textureArrays.push_back(m_textureArrays.back());
						m_textureArrays[frame][vIdx] = itL->getUV(vert, frame % itL->m_frames);
					}
					else
					{
						m_textureArrays[frame].push_back(itL->getUV(vert, frame % itL->m_frames));
					}
				}
			}

			// Set the index
			iTri[vert] = vIdx;
		}

		m_indexArray.push_back(iTri);
	}
}

Mesh::Mesh(const Lib3dsMesh& mesh3ds)
{
	const bool swapYZ = true;
	const bool reverseWinding = true;
	const bool invertV = true;

	TexArray texArr;

	clear();

	m_vertexArray.reserve(mesh3ds.points);
	texArr.reserve(mesh3ds.points);

	m_indexArray.reserve(mesh3ds.faces);
	m_textureArrays.reserve(1); // only one supported from 3DS

	if (isValidWzName(mesh3ds.name))
	{
		m_name = mesh3ds.name;
	}

	for (unsigned i = 0; i < mesh3ds.points; i++)
	{
		Lib3dsVector pos;
		WZMVertex vert;

		lib3ds_vector_copy(pos, mesh3ds.pointL[i].pos);

		/* TODO: Check if mesh3ds->matrix is identity,
		 *	if not we probably should transform the vertices.
		 */
		if (swapYZ)
		{
			vert.x() = pos[0];
			vert.y() = pos[2];
			vert.z() = pos[1];
		}
		else
		{
			vert.x() = pos[0];
			vert.y() = pos[1];
			vert.z() = pos[2];
		}
		m_vertexArray.push_back(vert);

		if (i < mesh3ds.texels)
		{
			WZMUV uv;
			uv.u() = mesh3ds.texelL[i][0];
			if (invertV)
			{
				uv.v() = 1.0f - mesh3ds.texelL[i][1];
			}
			else
			{
				uv.v() = mesh3ds.texelL[i][1];
			}
			texArr.push_back(uv);
		}
		else
		{
			texArr.push_back(WZMUV());
		}
	}

	m_textureArrays.push_back(texArr);

	for (unsigned i = 0; i < mesh3ds.faces; ++i)
	{
		Lib3dsFace *face = &mesh3ds.faceL[i];

		IndexedTri idx;

		if (reverseWinding)
		{
			idx.a() = face->points[2];
			idx.b() = face->points[1];
			idx.c() = face->points[0];
		}
		else
		{
			idx.a() = face->points[0];
			idx.b() = face->points[1];
			idx.c() = face->points[2];
		}
		m_indexArray.push_back(idx);
	}
	// TODO: Check if 3DS animation data can be used with our Frames
}

Mesh::~Mesh()
{
}

Pie3Level Mesh::backConvert(const Mesh& wzmMesh)
{
	return wzmMesh;
}

Mesh::operator Pie3Level() const
{
	Pie3Level p3;

	std::vector<Pie3Vertex>::iterator itPV;

	std::vector<IndexedTri>::const_iterator itTri;

	unsigned i;

	/* Note:
	 * WZM will have duplicates due to uv map differences
	 * so we remove those when converting
	 */

	for (itTri = m_indexArray.begin(); itTri != m_indexArray.end(); ++itTri)
	{
		Pie3Polygon p3Poly;

		for (i = 0; i < 3; ++i)
		{
			typedef Pie3Vertex::equal_wEps equals;
			mybinder1st<equals> compare(equals(), m_vertexArray[(*itTri)[i]]);

			itPV = std::find_if(p3.m_points.begin(), p3.m_points.end(), compare);

			if (itPV == p3.m_points.end())
			{
				// add it now
				p3Poly.m_indices[i] = p3.m_points.size();
				p3.m_points.push_back(m_vertexArray[(*itTri)[i]]);
			}
			else
			{
				p3Poly.m_indices[i] = std::distance(p3.m_points.begin(), itPV);
			}
#pragma message "unfinished"
			// TODO: deal with UV's now (incase you didn't know)

		}
		p3.m_polygons.push_back(p3Poly);
	}

	return p3;
}

bool Mesh::read(std::istream& in)
{
	std::string str;
	unsigned i,j,vertices,indices;
	GLfloat f;

	clear();

	in >> str >> m_name;
	if (in.fail() || str.compare("MESH") != 0)
	{
		std::cerr << "Mesh::read - Expected MESH directive found " << str;
		clear();
		return false;
	}

	if (!isValidWzName(m_name))
	{
		std::cerr << "Mesh::read - Invalid Mesh name.";
		m_name = std::string();
	}

	in >> str >> m_teamColours;
	if (in.fail() || str.compare("TEAMCOLOURS") != 0)
	{
		std::cerr << "Mesh::read - Expected TEAMCOLOURS directive found " << str;
		clear();
		return false;
	}

	in >> str >> vertices;
	if (in.fail() || str.compare("VERTICES") != 0)
	{
		std::cerr << "Mesh::read - Expected VERTICES directive found " << str;
		clear();
		return false;
	}

	in >> str >> indices;
	if (in.fail() || str.compare("FACES") != 0)
	{
		std::cerr << "Mesh::read - Expected FACES directive found " << str;
		clear();
		return false;
	}

	in >> str;
	if (str.compare("VERTEXARRAY") !=0)
	{
		std::cerr << "Mesh::read - Expected VERTEXARRAY directive found " << str;
		clear();
		return false;
	}

	m_vertexArray.reserve(vertices);
	for (; vertices > 0; --vertices)
	{
		WZMVertex vert;
		in >> vert.x() >> vert.y() >> vert.z();
		if (in.fail())
		{
			std::cerr << "Mesh::read - Error reading number of faces";
			clear();
			return false;
		}
		m_vertexArray.push_back(vert);
	}

	in >> str >> i;
	if (in.fail() || str.compare("TEXTUREARRAYS") != 0)
	{
		std::cerr << "Mesh::read - Expected TEXTUREARRAYS directive found " << str;
		clear();
		return false;
	}

	m_vertexArray.reserve(i);
	for (; i > 0; --i)
	{
		std::vector<WZMUV> tmp;
		tmp.clear();

		// j is currently ignored
		in >> str >> j;
		if ( in.fail() || str.compare("TEXTUREARRAY") != 0)
		{
			std::cerr << "Mesh::read - Expected TEXTUREARRAY directive found " << str;
			clear();
			return false;
		}

		for (j = 0; j < m_vertexArray.size(); ++j)
		{
			WZMUV uv;
			in >> uv.u() >> uv.v();
			if (in.fail())
			{
				std::cerr << "Mesh::read - Error reading uv coords.";
				clear();
				return false;
			}
			else if(uv.u()>1||uv.v()>1)
			{
				std::cerr << "Mesh::read - Error uv coords out of range";
				clear();
				return false;
			}
			tmp.push_back(uv);
		}
		m_textureArrays.push_back(tmp);
	}

	in >> str;
	if (str.compare("INDEXARRAY") != 0)
	{
		std::cerr << "Mesh::read - Expected INDEXARRAY directive found " << str;
		clear();
		return false;
	}

	m_indexArray.reserve(indices);
	for(;indices>0;indices--)
	{
		IndexedTri tri;

		in >> tri.a() >> tri.b() >> tri.c();

		if (in.fail())
		{
			std::cerr << "Mesh::read - Error reading indices";
			clear();
			return false;
		}
		m_indexArray.push_back(tri);
	}

	in >> str >> i;
	if (in.fail() || str.compare("FRAMES") != 0)
	{
		std::cerr << "Mesh::read - Expected FRAMES directive found " << str;
		clear();
		return false;
	}

	if (i > 0)
	{
		// TODO: animation frames
#pragma message "unfinished"
		for(; i > 0; --i)
		{
			in >> f >> f >> f >> f >> f >> f >> f >> f;
		}
		if (in.fail())
		{
			std::cerr << "Mesh::read - Error reading frames";
			clear();
			return false;
		}
	}

	in >> str >> i;
	if (in.fail() || str.compare("CONNECTORS") != 0)
	{
		std::cerr << "Mesh::read - Expected CONNECTORS directive found " << str;
		clear();
		return false;
	}

	if ( i > 0)
	{
		// TODO: Connectors
#pragma message "unfinished"
		for(; i > 0; --i)
		{
			in >> str >> f >> f >> f >> f >> f >> f ;
		}
		if(in.fail())
		{
			std::cerr << "Mesh::read - Error reading connectors";
			clear();
			return false;
		}
	}

	return true;
}

void Mesh::write(std::ostream &out) const
{
	out << "MESH ";
	if (m_name.empty())
	{
		out << "_noname_\n";
	}
	else
	{
		out << m_name << '\n';
	}

	// noboolalpha is default
	out << "TEAMCOLOURS " << teamColours() << '\n';

	out << "VERTICES " << vertices() << '\n';
	out << "FACES " << faces() << '\n';
	out << "VERTEXARRAY\n" ;

	std::vector<WZMVertex>::const_iterator vertIt;
	for (vertIt=m_vertexArray.begin(); vertIt < m_vertexArray.end(); vertIt++ )
	{
		out << '\t';
		out		<< vertIt->x() << ' '
				<< vertIt->y() << ' '
				<< vertIt->z() << '\n';
	}

	out << "TEXTUREARRAYS " << textureArrays() << '\n';

	std::vector< std::vector<WZMUV> >::const_iterator texArrIt;
	for (texArrIt=m_textureArrays.begin(); texArrIt < m_textureArrays.end(); texArrIt++ )
	{
		out << "TEXTUREARRAY " << std::distance(m_textureArrays.begin(),texArrIt) << '\n';
		std::vector<WZMUV>::const_iterator texIt;
		for (texIt=texArrIt->begin(); texIt < texArrIt->end(); texIt++ )
		{
			out << '\t';
			out	<< texIt->u() << ' '
					<< texIt->v() << '\n';
		}
	}

	out << "INDEXARRAY\n";

	std::vector<IndexedTri>::const_iterator indIt;
	for (indIt=m_indexArray.begin(); indIt < m_indexArray.end(); indIt++ )
	{
		out << '\t';
		out		<< indIt->a() << ' '
				<< indIt->b() << ' '
				<< indIt->c() << '\n';
	}

	// TODO: Frames and connectors
#pragma message "unfinished"
	out <<"FRAMES 0\nCONNECTORS 0\n";
}

bool Mesh::importFromOBJ(const std::vector<OBJTri>&	faces,
						 const std::vector<OBJVertex>& verts,
						 const std::vector<OBJUV>&	uvArray)
{
	const WZMVertex::less_wEps vertLess(0);
	const WZMUV::less_wEps uvLess(0);
	std::set<mypair<WZMVertex, WZMUV,
			WZMVertex::less_wEps, WZMUV::less_wEps> > pairSet;

	std::vector<OBJTri>::const_iterator itF;

	std::pair<std::set<mypair<WZMVertex, WZMUV, WZMVertex::less_wEps,
						WZMUV::less_wEps> >::iterator, bool> inResult;

	std::vector<unsigned> mapping;
	std::vector<unsigned>::iterator itMap;

	unsigned i;

	IndexedTri tmpTri;
	WZMUV tmpUv;

	clear();

	m_textureArrays.push_back(TexArray());

	for (itF = faces.begin(); itF != faces.end(); ++itF)
	{
		for (i = 0; i < 3; ++i)
		{
			if (itF->uvs.operator [](i) < 1)
			{
				tmpUv.u() = 0;
				tmpUv.v() = 0;
			}
			else
			{
				tmpUv = uvArray[itF->uvs.operator [](i)-1];
			}
			inResult = pairSet.insert(make_mypair(verts[itF->tri[i]-1],
												  tmpUv,
												  vertLess,
												  uvLess));

			if (!inResult.second)
			{
				tmpTri[i] = mapping[std::distance(pairSet.begin(), inResult.first)];
			}
			else
			{
				itMap = mapping.begin();
				std::advance(itMap, std::distance(pairSet.begin(), inResult.first));
				mapping.insert(itMap, m_vertexArray.size());
				tmpTri[i] = m_vertexArray.size();
				m_vertexArray.push_back(verts[itF->tri[i]-1]);
				m_textureArrays[0].push_back(tmpUv);
			}
		}
		m_indexArray.push_back(tmpTri);
	}
	return true;
}


std::string Mesh::getName() const
{
	return m_name;
}

void Mesh::setName(const std::string& name)
{
	if (isValidWzName(name))
	{
		m_name = name;
	}
}

bool Mesh::teamColours() const
{
	return m_teamColours;
}

void Mesh::setTeamColours(bool tc)
{
	m_teamColours = tc;
}

WZMConnector& Mesh::getConnector(int index)
{
	int i;
	std::list<WZMConnector>::iterator pos;
	for(i=0,pos=m_connectors.begin();i<index;pos++,i++);
	return *pos;
}

void Mesh::addConnector (const WZMConnector& conn)
{
	m_connectors.push_back(conn);
}

void Mesh::rmConnector (int index)
{
	int i;
	std::list<WZMConnector>::iterator pos;
	for(i=0,pos=m_connectors.begin();i<index;i++,pos++);
	if(pos==m_connectors.end())
	{
		return;
	}
	m_connectors.erase(pos);
}

int Mesh::connectors() const
{
	return m_connectors.size();
}

int Mesh::textureArrays() const
{
	return m_textureArrays.size();
}

TexArray& Mesh::getTexArray (int index)
{
	return m_textureArrays.at(index);
}

/*void Mesh::addTexArray (const TexArray& tex, int index)
{
	if(tex.size()!=indices())
	{
		return;
	}
	m_textureArrays.insert(m_textureArrays.begin() + index,tex);
}*/

/*void Mesh::rmTexArray(int index)
{
	std::vector<TexArray>::iterator pos;
	pos=m_textureArrays.begin()+index;
	if(pos==m_textureArrays.end())
	{
		return;
	}
	m_textureArrays.erase(pos);
}*/

unsigned int Mesh::vertices() const
{
	return m_vertexArray.size();
}

unsigned int Mesh::faces() const
{
	return triangles();
}

unsigned int Mesh::triangles() const
{
	return m_indexArray.size();
}

unsigned int Mesh::frames() const
{
	return m_frameArray.size();
}

unsigned int Mesh::indices() const
{
	return m_indexArray.size();
}

bool Mesh::isValid() const
{
	// TODO: check m_frameArray, m_connectors
#pragma message "unfinished"
	if (!isValidWzName(m_name))
	{
		return false;
	}

	// Check that the values of the indices are in range
	std::vector<IndexedTri>::const_iterator it;
	for (it = m_indexArray.begin(); it != m_indexArray.end(); ++it)
	{
		if ((*it).a() >= m_vertexArray.size())
		{
			return false;
		}
		if ((*it).b() >= m_vertexArray.size())
		{
			return false;
		}
		if ((*it).c() >= m_vertexArray.size())
		{
			return false;
		}
	}
	return true;
}

void Mesh::clear()
{
	m_name = std::string();
	m_frameArray.clear();
	m_vertexArray.clear();
	m_textureArrays.clear();
	m_indexArray.clear();
	m_connectors.clear();
	m_teamColours = false;
}

WZM::WZM()
{
}

WZM::WZM(const Pie3Model &p3)
{
	m_texName = p3.m_texture;
	std::vector<Pie3Level>::const_iterator it;
	for (it = p3.m_levels.begin(); it != p3.m_levels.end(); ++it)
	{
		m_meshes.push_back(*it);
	}
}

WZM::operator Pie3Model() const
{
	Pie3Model p3;
	p3.m_texture = m_texName;
	std::transform(m_meshes.begin(), m_meshes.end(),
				   back_inserter(p3.m_levels), Mesh::backConvert);
	return p3;
}

bool WZM::read(std::istream& in)
{
	std::string str;
	int i,meshes;

	clear();
	in >> str;
	if (in.fail() || str.compare("WZM") != 0)
	{
		std::cerr << "WZM::read - Missing WZM header";
		return false;
	}

	in >> i;
	if (in.fail())
	{
		std::cerr << "WZM::read - Error reading WZM version";
		return false;
	}
	else if(i!=2)
	{
		std::cerr << "WZM::read - Unsupported WZM version " << i;
		return false;
	}

	// TEXTURE %s
	in >> str;
	if (str.compare("TEXTURE") != 0)
	{
		std::cerr << "WZM::read - Expected TEXTURE directive but got" << str;
		return false;
	}
	in >> m_texName;
	if (in.fail())
	{
		std::cerr << "WZM::read - Error reading WZM version";
		m_texName = std::string();
		return false;
	}

	// MESHES %u
	in >> str >> meshes;
	if (in.fail() || str.compare("MESHES") != 0)
	{
		std::cerr << "WZM::read - Expected MESHES directive but got " << str;
		clear();
		return false;
	}

	m_meshes.reserve(meshes);
	for(; meshes>0; --meshes)
	{
		Mesh mesh;
		if (!mesh.read(in))
		{
			clear();
			return false;
		}
		m_meshes.push_back(mesh);
	}
	return true;
}

void WZM::write(std::ostream& out) const
{
	std::vector<Mesh>::const_iterator it;
	out << "WZM\t" << version() << '\n';
	out << "TEXTURE " << m_texName << '\n';
	out << "MESHES " << meshes() << '\n';
	for (it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{
		it->write(out);
	}
}

/*
 * This function does the parsing,
 * we'll let class Mesh do the WZM'izing
 */
bool WZM::importFromOBJ(std::istream& in)
{
	std::vector<OBJVertex> vertArray;

	std::vector<OBJUV> uvArray;

	std::vector<OBJTri> groupedFaces;

	std::string name("Default"); //Default name of default obj group is default
	std::vector<std::string> vertices, indices;
	std::vector<std::string>::iterator itVStr;
	std::string str;
	std::stringstream ss;

	// Only give warnings once
	bool warnLine=false, warnPoint=false;

	// More temporaries
	OBJTri tri;
	Vertex<GLfloat> vert;
	UV<GLfloat> uv;
	Mesh mesh;

	unsigned i;

	clear();

	/*	Build global vertex and
	 *	texture coordinate arrays.
	 *	Also figure out the mesh/group boundaries
	 */
	/*
	 * All the directives for the vertices (inc. normals and textures)
	 * should appear before any are referenced, this method will tolerate
	 * incorrect obj files to a certain degree.
	 */

	while(!(in.eof()|| in.fail()))
	{
		std::getline(in, str);
		ss.str(str);
		ss.seekg(0);
		ss.clear();

		// We're forgiving of leading whitespaces (though we don't need/shouldn't to be)
		skipWhitespace(ss);
		if (str.empty() || ss.fail())
		{
			continue;
		}

		switch(ss.get())
		{
		case 'v':
			switch(ss.get())
			{
			case 't':
				ss >> uv.u() >> uv.v();
				if (ss.fail())
				{
					return false;
				}
				uvArray.push_back(uv);
				break;
			case 'n':	// Ignore normals
			case 'p':	// and parameter vertices
				break;
			default:
				ss >> vert.x() >> vert.y() >> vert.z();
				if (ss.fail())
				{
					return false;
				}
				vertArray.push_back(vert);
			}
			break;
		case 'f':
			vertices = split(str);
			if (vertices.size() < 2 + 1)
			{
				 /* Should never happen, faces will have >= 3+1 tokens
				  * ( more than 3 vertices + the "f" directive)
				  */
				break;
			}

			itVStr = vertices.begin();
			++itVStr; // Advance to first vertex

			for (i = 0; itVStr != vertices.end(); ++itVStr, ++i)
			{
				if ((i % 3 == 0) && i != 0)
				{
					 /* Don't overwrite the first vertex
					  * since we're converting from triangle fans
					  */
					continue;
				}

				indices = split(*itVStr, '/');

				ss.str(indices[0]), ss.clear(), ss.seekg(0);

				ss >> tri.tri.operator [](i % 3);

				if (indices.size() >= 2 && !indices[1].empty())
				{
					ss.str(indices[1]), ss.clear(), ss.seekg(0);
					ss >> tri.uvs.operator [](i % 3);
				}
				else
				{
					tri.uvs.operator [](i % 3) = -1;
				}

				if (indices.size() == 3 && !indices[2].empty())
				{} // ignoring normals

				if (i % 3 == 2)
				{
					groupedFaces.push_back(tri);
				}
			}
			break;
		case 'l':
			// Ignore lines and give one warning to the user
			if (!warnLine)
			{
				std::cout << "WZM::importFromOBJ - Warning! Lines are not supported and will be ignored!";
				warnLine = true;
			}
			break;
		case 'p':
			// Ignore points and give one warning to the user
			if (!warnPoint)
			{
				std::cout << "Model::importFromOBJ - Warning! Points are not supported and will be ignored!";
				warnPoint = true;
			}
			break;
		case 'o':
			if (!groupedFaces.empty())
			{
				mesh.importFromOBJ(groupedFaces,vertArray,uvArray);
				mesh.setTeamColours(false);
				mesh.setName(name);
				m_meshes.push_back(mesh);
				groupedFaces.clear();
			}
			ss >> name;
			if (!isValidWzName(name))
			{
				ss.str(std::string()), ss.clear(), ss.seekg(0);
				ss << m_meshes.size();
				ss >> name;
			}
			break;
		}
	}
	if (!groupedFaces.empty())
	{
		mesh.importFromOBJ(groupedFaces,vertArray,uvArray);
		mesh.setTeamColours(false);
		mesh.setName(name);
		m_meshes.push_back(mesh);
	}
	return true;
}

bool WZM::importFrom3DS(std::string fileName)
{
	Lib3dsFile *f = lib3ds_file_load(fileName.c_str());
	Lib3dsMaterial *material;
	Lib3dsMesh *ps3dsMesh;

	if (f == NULL)
	{
		std::cerr << "Loading 3DS file failed.\n";
		return false;
	}

	clear();

	material = f->materials;

	// Grab texture name
	if (material != NULL)
	{
		m_texName = material->texture1_map.name;
		if (material->next != NULL)
		{
			std::cout << "WZM::importFrom3ds - Multiple textures not supported!\n";
		}
	}

	for (ps3dsMesh = f->meshes; ps3dsMesh != NULL; ps3dsMesh = ps3dsMesh->next)
	{
		m_meshes.push_back(*ps3dsMesh);
	}

	lib3ds_file_free(f);
	return true;
}

int WZM::version() const
{
	return 2;
}

int WZM::meshes() const
{
	return m_meshes.size();
}

Mesh& WZM::getMesh(int index)
{
	return m_meshes.at(index);
}

void WZM::addMesh(const Mesh& mesh)
{
	m_meshes.push_back(mesh);
}

void WZM::rmMesh (int index)
{
	std::vector<Mesh>::iterator pos;
	pos=m_meshes.begin()+index;
	if(pos==m_meshes.end())
	{
		return;
	}
	m_meshes.erase(pos);
}

bool WZM::isValid() const
{
	std::vector<Mesh>::const_iterator it;

	if (!isValidWzName(m_texName))
	{
		return false;
	}

	for (it = m_meshes.begin(); it != m_meshes.end(); ++it)
	{
		if(!it->isValid())
		{
			return false;
		}
	}
	return true;
}

void WZM::clear()
{
	m_meshes.clear();
	m_texName = std::string();
}
