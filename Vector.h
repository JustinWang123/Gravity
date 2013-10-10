#ifndef VECTOR_H
#define VECTOR_H

template <class T>

class Vector2d {
public:
	Vector2d();
	Vector2d(T setX, T setY);
	~Vector2d() {}

	void SetX(T setX);
	void SetY(T setY);
	T GetX();
	T GetY();

private
	T x;
	T y;
}
