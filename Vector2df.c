#ifndef VECTOR2DF_H
#define VECTOR2DF_H

class Vector2df {
public:
	Vector2df(float setX, float setY);
	Vector2df(float setDeg);

	// Accessors:
	float X() const;
	float Y() const;
	Vector2df Normal() const;

	// Modifiers:
	void Set(float setX, float setY);
	void SetDeg(float setDeg);

	bool operator==(const X& lhs, const X& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}
	bool operator!=(const X& lhs, const X& rhs) {
		return !(lhs == rhs);
	}

	T& operator=(const T& other) {
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	Vector2df& operator+=(const Vector2df& rhs) {
		this.x += rhs.x;
		this.y += rhs.y;
		return *this;
	}

	Vector2df operator+(Vector2df lhs, const Vector2df& rhs) {
		lhs += rhs;
		return lhs;
	}

	Vector2df& operator-=(const Vector2df& rhs) {
		this.x -= rhs.x;
		this.y -= rhs.y;
		return *this;
	}

	Vector2df operator-(Vector2df lhs, const Vector2df& rhs) {
		lhs -= rhs;
		return lhs;
	}

private:
	float x;
	float y;
};

#endif // VECTOR2DF_H
