#pragma once


template< typename T >
struct vector3
{
private:
	static constexpr T default_value = T();
public:

	T x;
	T y;
	T z;

	constexpr vector3()
		: vector3(default_value, default_value, default_value)
	{
	}

	constexpr vector3(T x, T y, T z)
		: x(x)
		, y(y)
		, z(z)
	{
	}

	template< typename U >
	constexpr vector3(vector3<U> other)
		: vector3(other.x, other.y, other.z)
	{
	}

	template< typename U >
	vector3 & operator+=(const U rhs)
	{
		x += rhs;
		y += rhs;
		z += rhs;
		return *this;
	}

	template< typename U >
	vector3 & operator+=(const vector3<U> & rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	template< typename U >
	vector3 & operator-=(const U rhs)
	{
		x -= rhs;
		y -= rhs;
		z -= rhs;
		return *this;
	}

	template< typename U >
	vector3 & operator-=(const vector3<U> & rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	template< typename U >
	vector3 & operator*=(const U rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	template< typename U >
	vector3 & operator/=(const U rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
};

using vector3d = vector3<double>;
using vector3i = vector3<int>;

template< typename T, typename U >
constexpr vector3<T> operator*(vector3<T> lhs, const U rhs)
{
	return vector3<T>(lhs.x * rhs, lhs.y * rhs, lhs.y * rhs);
}

template< typename T, typename U >
constexpr vector3<T> operator*(const U lhs, vector3<T> rhs)
{
	return vector3<U>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

template< typename T, typename U >
constexpr vector3<T> operator/(vector3<T> lhs, const U rhs)
{
	return vector3<U>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.y / rhs.z);
}

template< typename T, typename U >
constexpr vector3<T> operator/(const U lhs, vector3<T> rhs)
{
	return vector3<U>(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}

template< typename T, typename U >
constexpr vector3<T> operator+(vector3<T> lhs, const U rhs)
{
	return vector3<T>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
}

template< typename T, typename U >
constexpr vector3<T> operator+(const U lhs, vector3<T> rhs)
{
	return vector3<T>(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
}

template< typename T >
constexpr vector3<T> operator+(const vector3<T> &lhs, const vector3<T> &rhs)
{
	return vector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

template< typename T, typename U >
constexpr vector3<T> operator-(vector3<T> lhs, const U rhs)
{
	return vector3<T>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
}

template< typename T, typename U >
constexpr vector3<T> operator-(const U lhs, vector3<T> rhs)
{
	return vector3<T>(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
}

template< typename T >
constexpr vector3<T> operator-(const vector3<T> &lhs, const vector3<T> &rhs)
{
	return vector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}


