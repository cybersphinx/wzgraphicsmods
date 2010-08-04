#ifndef POLYGON_HPP
#define POLYGON_HPP
#include "Vector.hpp"
#include "VectorTypes.hpp"
#include <iostream>
#include <GL/gl.h>

///
struct IndexedTri : public Vector<GLushort,3>
{
	GLushort& a() {
		return component[0];
	}
	GLushort a() const {
		return component[0];
	}

	GLushort& b() {
		return component[1];
	}
	GLushort b() const {
		return component[1];
	}

	GLushort& c() {
		return component[2];
	}
	GLushort c() const {
		return component[2];
	}
};

/*
  Pie polygons are triangle fans of with
  texture and texture animation data.
  */
template<typename U, typename S, size_t MAX>
class PiePolygon
{
	friend class Mesh;
public:
	PiePolygon();
	virtual ~PiePolygon(){}

	bool read(std::istream& in);
	void write(std::ostream& out) const;

	unsigned getFrames() const;
	unsigned getIndex(unsigned int n) const;
	U getUV(int index) const;
	unsigned short vertices() const;
	unsigned short triangles() const;
protected:
	void clear();
	static const size_t MAX_VERTICES = MAX;
	unsigned short m_vertices;
	short m_indices[MAX];
	U m_texCoords[MAX];
	unsigned long m_flags;

	/// Texture Animation Data
	unsigned m_frames;
	unsigned m_playbackRate;
	S m_width, m_height;
};

struct OBJTri
{
	 IndexedTri tri;
	 // ignoring normals
	 Vector<short, 3> uvs; // signed short: -1 means not specified
	bool operator == (const OBJTri& rhs)
	{
		return (tri == rhs.tri) && (uvs == rhs.uvs);
	}
	bool operator < (const OBJTri& rhs)
	{
		if (tri == rhs.tri)
		{
			return uvs < rhs.uvs;
		}
		return tri < rhs.tri;
	}
};

// Include template implementations
#include "Polygon_t.cpp"

#endif // POLYGON_HPP
