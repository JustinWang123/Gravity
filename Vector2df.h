
#ifndef VECTOR2DF_H
#define VECTOR2DF_H

class Vector2df {
public:
	Vector2df();
	Vector2df(float setX, float setY);
	Vector2df(float setDeg);

	// Accessors:
	Vector2df Normal() const;
	float Deg() const;
	float Length() const;
	bool IsNormal() const;

	// Modifiers:
	void Set(float setX, float setY);
	void SetDeg(float setDeg);

	bool operator==(const Vector2df& rhs) {
		return this->x == rhs.x && this->y == rhs.y;
	}
	bool operator!=(const Vector2df& rhs) {
		return !(*this == rhs);
	}

	Vector2df& operator=(const Vector2df& other) {
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	Vector2df& operator+=(const Vector2df& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	Vector2df operator+(const Vector2df& rhs) const {
		return Vector2df(this->x + rhs.x, this->y + rhs.y);
	}

	Vector2df& operator-=(const Vector2df& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

	Vector2df operator-(const Vector2df& rhs) const{
		return Vector2df(this->x - rhs.x, this->y - rhs.y);
	}

	Vector2df& operator/=(const float& rhs) {
		this->x /= rhs;
		this->y /= rhs;
		return *this;
	}

	Vector2df operator/( const float& rhs) const{
		Vector2df temp(this->x / rhs, this->y / rhs);
		return temp;
	}

	Vector2df& operator*=(const float& rhs) {
		this->x *= rhs;
		this->y *= rhs;
		return *this;
	}

	Vector2df operator*(const float& rhs) const{
		return Vector2df(this->x * rhs, this->y * rhs);
	}

	float x;
	float y;
};

#endif // VECTOR2DF_H
