#ifndef PIE_HPP
#define PIE_HPP

#include <iostream>
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

/**
  * Templates used to remove tedious code
  * duplication.
  *
  */
template<typename V, typename P, typename C>
class APieLevel
{
	friend class Mesh;
public:
	APieLevel();
	virtual ~APieLevel(){}

	virtual bool read(std::istream& in);
	virtual void write(std::ostream& out) const;

	int points() const;
	int polygons() const;
	int connectors() const;

	bool isValid() const;
protected:
	void clearAll();

	std::vector<V> m_points;
	std::vector<P> m_polygons;
	std::list<C> m_connectors;
};

template <typename L>
class APieModel
{
public:
	APieModel();
	virtual ~APieModel();

	virtual unsigned int version() const =0;

	virtual bool read(std::istream& in);
	virtual void write(std::ostream& out) const;

	unsigned int levels() const;
	unsigned long getType() const;

	bool isValid() const;

//	virtual bool setType(int type) = 0;
protected:

	void clearAll();

	virtual unsigned int textureHeight() const =0;
	virtual unsigned int textureWidth() const =0;

	std::string m_texture;
	std::vector<L> m_levels;
	unsigned long m_type;
};

template <typename V>
struct PieConnector
{
	virtual ~PieConnector(){}
	bool read(std::istream& in);
	void write(std::ostream& out) const;
	V pos;
};

/**********************************************
  Pie version 2
  *********************************************/
typedef UV<GLushort> Pie2UV;
typedef Vertex<GLint> Pie2Vertex;
typedef PieConnector<Pie2Vertex> Pie2Connector;

class Pie2Polygon : public PiePolygon<Pie2UV, GLushort, 16>
{
	friend class Pie3Polygon; // only for operator thisclass() and thatclass(const thisclass&)
public:
	Pie2Polygon(){}
	virtual ~Pie2Polygon(){}
};

class Pie2Level : public APieLevel<Pie2Vertex, Pie2Polygon, Pie2Connector>
{
	friend class Pie3Level; // only for operator thisclass() and thatclass(const thisclass&)
public:
	Pie2Level(){}
	virtual ~Pie2Level(){}
};

class Pie2Model : public APieModel<Pie2Level>
{
	friend class Pie3Model; // only for operator thisclass() and thatclass(const thisclass&)
public:
	Pie2Model();
	virtual ~Pie2Model();

	unsigned int version() const;

	unsigned int textureHeight() const;
	unsigned int textureWidth() const;
};
/**********************************************
  Pie version 3
  *********************************************/
class Pie3UV : public UV<GLclampf>
{
public:
	Pie3UV();
	Pie3UV(GLclampf u, GLclampf v);
	Pie3UV(const Pie2UV& p2);
	operator Pie2UV() const;
};

class Pie3Vertex : public Vertex<GLfloat>
{
public:
	Pie3Vertex(){}
	Pie3Vertex(const Vertex<GLfloat>& vert);
	Pie3Vertex(const Pie2Vertex& p2);

	static inline Pie3Vertex upConvert(const Pie2Vertex& p2);
	static Pie2Vertex backConvert(const Pie3Vertex& p3);
	operator Pie2Vertex() const;
};

class Pie3Connector : public PieConnector<Pie3Vertex>
{
public:
	Pie3Connector(){}
	Pie3Connector(const Pie2Connector& p2);

	static Pie3Connector upConvert(const Pie2Connector& p2);
	static Pie2Connector backConvert(const Pie3Connector& p3);
	operator Pie2Connector() const;
};

class Pie3Polygon : public PiePolygon<Pie3UV, GLclampf, 3>
{
public:
	Pie3Polygon();
	virtual ~Pie3Polygon();

	static int upConvert(const Pie2Polygon& pie2Poly, std::back_insert_iterator<std::vector<Pie3Polygon> > result);
	static Pie2Polygon backConvert(const Pie3Polygon& p3);
	operator Pie2Polygon() const;

	Pie3UV getUV(unsigned index, unsigned frame) const;

private:
};

class Pie3Level : public APieLevel<Pie3Vertex, Pie3Polygon, Pie3Connector>
{
public:
	Pie3Level();
	Pie3Level(const Pie2Level& p2);
	virtual ~Pie3Level();

	static Pie3Level upConvert(const Pie2Level& p2);
	static Pie2Level backConvert(const Pie3Level& p3);
	operator Pie2Level() const;
};

class Pie3Model : public APieModel<Pie3Level>
{
	friend class WZM; // only for operator thisclass() and thatclass(const thisclass&)
public:
	Pie3Model();
	Pie3Model(const Pie2Model& pie2);
	virtual ~Pie3Model();

	unsigned int version() const;

	operator Pie2Model() const;

	bool setType(int type);
private:
	unsigned int textureHeight() const;
	unsigned int textureWidth() const;
};

// Include template implementations
#include "Pie_t.cpp"

#endif // PIE_HPP