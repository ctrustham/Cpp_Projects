#pragma once
#include "table.h"
#include "App/AppSettings.h"
#include "App/app.h"
#include "Vectors.h"

class Flipper
{
	public:
		Flipper(CLineSegment* line);
		void ActivateRight();
		void ActivateLeft();
		Vector2d GetForce();
		void Draw();
	public:
		CLineSegment* line; // the underlying line representing the flipper
		bool isFlipped;
		float activateAngle; // degrees flipper actuates/rotates to
		float currentAngle; // the current rotation of the flipper
		int xOld;
		int yOld;
		
	private:
		void Rotate(float degrees);
};

class Ball
{
public:
	Ball(float rad = 8.0f, CPoint = { 0,0 }); //{ 1024/2, 768/2 });
	void Draw();
	void AddForce(float x, float y);
	void Update(float dt);
	void Bounce(CLineSegment line);
public:
	CPoint center;
	Vector2d velocity; // velocity vector
	float radius;
};

Vector2d FindCollisionNormal(CLineSegment line, Ball ball);

