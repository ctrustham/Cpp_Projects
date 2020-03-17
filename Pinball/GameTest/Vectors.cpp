/*
Created by Craig Trustham
Last Modified March 17, 2020
*/

#include "stdafx.h"
#include "Vectors.h"


Vector2d::Vector2d(float _x, float _y)
{
	x = _x;
	y = _y;
}

Vector2d::Vector2d()
{
	x = 0.0f;
	y = 0.0f;
}

float Vector2d::Dot(Vector2d _v)
{
	return x * _v.x + y * _v.y;
}

float Vector2d::Magnitude()
{
	return sqrt((x * x) + (y * y));
}

Vector2d Vector2d::UnitForm()
{
	return Vector2d(x/Magnitude(), y/Magnitude());
}

Vector2d Vector2d::Normal()
{
	
	return Vector2d(-y, x);
}

Vector2d Vector2d::operator*(Vector2d rhs) {
	return Vector2d(x * rhs.x, y * rhs.y);
}

Vector2d Vector2d::operator*(float rhs) {
	return Vector2d(x * rhs, y * rhs);
}

Vector2d Vector2d::operator-(Vector2d rhs) {
	return Vector2d(x - rhs.x, y - rhs.y);
}

Vector2d Vector2d::operator+(Vector2d rhs) {
	return Vector2d(x + rhs.x, y + rhs.y);
}

// radians between vectors
float AngleBetween(Vector2d Va, Vector2d Vb)
{
	float magVa = Va.Magnitude();
	float magVb = Vb.Magnitude();

	float calc = (Va.Dot(Vb) / (magVa * magVb));
	float radians = acosf(calc);

	return radians;
}

// returns a scalar representing how much of U is in the V direction
float compUonV(Vector2d u, Vector2d v)
{
	float dot = u.Dot(v);
	float magV = v.Magnitude();
	return dot/magV;
}

float DegToRad(float deg)
{
	return deg * PI / 180;
}

float RadToDeg(float rad)
{
	return rad * 180 / PI;
}
