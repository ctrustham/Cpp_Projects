#pragma once
#include "stdafx.h"
#include <cmath>
#include "App/AppSettings.h"

class Vector2d {
public:
	float x, y;

public:
	Vector2d(float _x, float _y);
	Vector2d();

	float Dot(Vector2d _v);
	float Magnitude();
	Vector2d UnitForm();
	Vector2d Normal();

	Vector2d operator*(Vector2d rhs);
	Vector2d operator*(float rhs);
	Vector2d operator-(Vector2d);
	Vector2d operator+(Vector2d);
};


float DegToRad(float deg);
float RadToDeg(float rad);
float AngleBetween(Vector2d Va, Vector2d Vb); // in radians
float compUonV(Vector2d u, Vector2d v);


