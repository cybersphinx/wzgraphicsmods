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

/* The following 2 lines are for obj conversion
 */
typedef UV<GLfloat> OBJUV;
typedef Vertex<GLfloat> OBJVertex;

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

typedef Vertex<GLfloat> WZMVertex;
typedef UV<GLclampf> WZMUV;
typedef std::vector<UV<GLclampf> > TexArray;

class Pie3Level;
class Lib3dsMesh;
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

	std::string getName() const;
	void setName(const std::string& name);

	bool teamColours() const;
	void setTeamColours(bool tc);

	WZMConnector& getConnector(int index);
	void addConnector (const WZMConnector& conn);
	void rmConnector (int index);
	int connectors() const;


	int textureArrays() const;
	TexArray& getTexArray (int index);
	void addTexArray (const TexArray& tex, int index);
	void rmTexArray(int index);
	void replaceTexArray(const TexArray& tex, int index);

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
