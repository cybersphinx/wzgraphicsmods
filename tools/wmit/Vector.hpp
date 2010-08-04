#ifndef VERTEX_HPP
#define VERTEX_HPP
#include <cstddef>
#include <limits>
#include <cmath>
#include <functional>

template <typename T, size_t C>
struct Vector
{
	Vector(){}
	virtual ~Vector(){}
	static const size_t COMPONENTS = C;

	inline T  operator [](unsigned int i) const {
		return component[i];
	}
	inline T& operator [](unsigned int i)       {
		return component[i];
	}
	inline operator const T*() const {
		return component;
	}

	struct equal_wEps : public std::binary_function<const Vector&,const Vector&,bool>
	{
		equal_wEps(double eps = std::numeric_limits<T>::epsilon()):m_eps(eps)
		{
			if (m_eps < 0)
			{
				m_eps = std::numeric_limits<T>::epsilon();
			}
		}
		bool operator() (const Vector& lhs, const Vector& rhs)const
		{
			unsigned i;
			for (i = 0; i < COMPONENTS; ++i)
			{
				if (std::abs(lhs[i] - rhs[i]) > m_eps)
				{
					return false;
				}
			}
			return true;
		}
		void setEps(double eps = std::numeric_limits<T>::epsilon())
		{
			m_eps = eps;
		}
	private:
		double m_eps;
	};

	bool operator == (const Vector& rhs) const
	{
		unsigned i;
		for (i = 0; i < COMPONENTS; ++i)
		{
			if (component[i] != rhs[i])
			{
				return false;
			}
		}
		return true;
	}

	virtual bool operator < (const Vector& rhs) const
	{
		unsigned i;
		for (i = 0; i < COMPONENTS-1; ++i)
		{
			if (component[i] != rhs.component[i])
			{
				return component[i] < rhs.component[i];
			}
		}
		return component[i] < rhs.component[i];
	}

	struct less_wEps : public std::binary_function<const Vector&,const Vector&,bool>
	{
		less_wEps(double eps = -1): compare(eps) {}
		bool operator() (const Vector& lhs, const Vector& rhs)const
		{
			bool dbgtmp = (lhs < rhs) && !compare(lhs, rhs);
			return dbgtmp;
		}
	private:
		equal_wEps compare;
	};

protected:
	T component[C];
};

#endif // VERTEX_HPP
