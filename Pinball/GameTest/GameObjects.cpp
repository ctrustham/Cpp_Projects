/*
Created by Craig Trustham
Last Modified March 17, 2020
*/

#include "stdafx.h"
#include "GameObjects.h"
#include "App/app.h"

#define APP_NATIVE_TO_VIRTUAL_COORDS(_x_,_y_)			_x_ = ((_x_ + 1.0f) * APP_VIRTUAL_WIDTH) / 2.0f; _y_ = ((_y_ + 1.0f) * APP_VIRTUAL_HEIGHT) / 2.0f;

Flipper::Flipper(CLineSegment* line)
{
	this->line = line;
	isFlipped = false;
	activateAngle = 55;
	currentAngle = 0;
	xOld = line->m_end.m_x;
	yOld = line->m_end.m_y;
}

void Flipper::ActivateLeft()
{
	xOld = line->m_end.m_x;
	yOld = line->m_end.m_y;
	if(currentAngle <= activateAngle && isFlipped)
	{
		Rotate(5.0f);
	}
	else if(currentAngle > 0 && !isFlipped) {
		Rotate(-5.0f);
	}
}

void Flipper::ActivateRight()
{
	xOld = line->m_end.m_x;
	yOld = line->m_end.m_y;
	if (currentAngle >= activateAngle*-1 && isFlipped)
	{
		Rotate(-5.0f);
	}
	else if (currentAngle < 0 && !isFlipped) {
		Rotate(5.0f);
	}
}

Vector2d Flipper::GetForce()
{
	float dx = line->m_end.m_x - xOld;
	float dy = line->m_end.m_y - yOld;
	return Vector2d(dx,dy);
}

void Flipper::Rotate(float degrees) {
	CPoint pivot = line->m_start;
	float sinAngle = sin(degrees * PI / 180); // angle in radians
	float cosAngle = cos(degrees * PI / 180);
	float xnew = 0;
	float ynew = 0;

	// translate point to rotate around origin
	line->m_end.m_x -= pivot.m_x;
	line->m_end.m_y -= pivot.m_y;

	// rotate point
	xnew = line->m_end.m_x * cosAngle - line->m_end.m_y * sinAngle;
	ynew = line->m_end.m_x * sinAngle + line->m_end.m_y * cosAngle;

	// translate point back
	line->m_end.m_x = xnew + pivot.m_x;
	line->m_end.m_y = ynew + pivot.m_y;

	currentAngle+= degrees;
}

void Flipper::Draw()
{
	App::DrawLine(line->m_start.m_x, line->m_start.m_y, line->m_end.m_x, line->m_end.m_y);
}

Ball::Ball(float rad, CPoint cent)
{
	radius = rad;
	center.m_x = cent.m_x;
	center.m_y = cent.m_y;
	velocity.x = 0;
	velocity.y = 0; 

#if APP_USE_VIRTUAL_RES
	APP_NATIVE_TO_VIRTUAL_COORDS(center.m_x, center.m_y);
#endif
}

void Ball::Draw()
{
	App::DrawCircle(center.m_x, center.m_y, radius);
}

void Ball::AddForce(float x, float y)
{
//#if APP_USE_VIRTUAL_RES		
//	APP_NATIVE_TO_VIRTUAL_COORDS(x, y);
//#endif
	velocity.x += x;
	velocity.y += y;
}

void Ball::Update(float dt)
{
	if (velocity.y > -5.0f)
	{
		AddForce(0, -0.5f * dt / 1000); // constant acceleration down -> gravity
	}
	center.m_x += velocity.x;
	center.m_y += velocity.y;
}

//void Ball::Bounce(CLineSegment line)
//{
//	center.m_x -= velocity.x;
//	center.m_y -= velocity.y;
//	Vector2d surface = { line.m_start.m_x - line.m_end.m_x, line.m_start.m_y - line.m_end.m_y };
//	if (surface.x < 0) {
//		surface.x *= -1;
//		surface.y *= -1;
//	}
//
//	Vector2d xAxis = { 1, 0 };
//	Vector2d vel = { -1 * velocity.x, -1 * velocity.y };
//
//	float theta = AngleBetween(velocity, surface);
//	float thetaDeg = RadToDeg(theta);
//	//if (theta < PI) {
//	//	theta = PI - theta;
//	//}
//	//else {
//	//	theta = PI + (2 * PI - theta);
//	//}
//	thetaDeg = RadToDeg(theta);
//	//if (velocity.y > 0) {
//	//	theta *= -1;
//	//}
//
//	float alpha = AngleBetween(surface, xAxis);
//	float alphaDeg = RadToDeg(alpha);
//
//	float beta = theta + alpha;
//
//	//if (velocity.x < 0) {
//	//	beta = alpha - theta;
//	//}
//
//	float vMag = velocity.Magnitude();
//	float rx = vMag * cosf(beta);
//	float ry = vMag * sinf(beta);
//
//	Vector2d reflection = { rx, ry };
//
//		this->velocity = reflection;
//}

// calculates the reflection vector for the ball
void Ball::Bounce(CLineSegment line)
{	
	
	Vector2d surface = { line.m_start.m_x - line.m_end.m_x, line.m_start.m_y - line.m_end.m_y };

	//ensure the surface is in the positive-x direction (for easier calculation)
	if (surface.x < 0) {
		surface.x *= -1;
		surface.y *= -1;
	}

		// reverse the vector-in so that it lines up tail to tail with the surface vector
		Vector2d v = { -1 * velocity.x, -1 * velocity.y };

		// convert the vector-in to a unit vector 
		Vector2d vHat = v.UnitForm();

		//backtrack the ball to ensure it does not get stuck
		//center.m_x -= velocity.x;
		//center.m_y -= velocity.y;
		while (line.DistanceToLine(center.m_x, center.m_y) < radius) {
			center.m_x += vHat.x;
			center.m_y += vHat.y;
		}
		float distToLine = line.DistanceToLine(center.m_x, center.m_y);

		// calculate the normal to the surface
		Vector2d sNorm = surface.Normal();

		// determine if the positive or negative normal to the surface is needed
		// this is where having the surface always in the positive-x direction makes things easier
		if (v.x < -0.0f) {
			sNorm = sNorm * -1;
		}

		// convert the surface-normal to a unit vector 
		Vector2d sNormHat = sNorm.UnitForm();

		// caculate the reflection vector using R= 2*(S-hat <dot> V-hat)*S-hat minus V-hat
		// see: https://www.fabrizioduroni.it/2017/08/25/how-to-calculate-reflection-vector.html
		Vector2d reflection = sNormHat * (2 * vHat.Dot(sNormHat)) - vHat;

		if (line.m_type == eLine_Flipper) {
			reflection = reflection * velocity.Magnitude() * 0.25; // lose some energy each bounce
		}
		else if (line.m_type == eLine_Hard) {
			reflection = reflection * velocity.Magnitude() * 0.55; // lose some energy each bounce
		}
		else if (line.m_type == eLine_Soft) {
			reflection = reflection * velocity.Magnitude() * 0.35; // lose some energy each bounce
		}
		else if (line.m_type == eLine_Power) {
			reflection = reflection * velocity.Magnitude() * 1.25; // Gain some energy each bounce
		}

		// gravity was here

	if (reflection.Magnitude() >= 0.1) {
		this->velocity = reflection;// +gravity;
	}
	else {
		Vector2d gravity = { 0,-0.5 };
		float angle = AngleBetween(sNormHat * -1, gravity);
		float angleDeg = RadToDeg(angle);
		float s = sinf(angle);
		float c = cosf(angle);
		if (velocity.y < 0) {
			gravity = Vector2d(s, c).UnitForm() * gravity.Magnitude();
		}
			this->velocity = velocity + gravity;
	}
}

//eLine_Hard] =
//eLine_Soft] =
//eLine_Power] =
//eLine_Flipper]
//eLine_Fail] =
//eLine_Score] =

Vector2d FindCollisionNormal(CLineSegment line, Ball ball) {

	CPoint p1;
	CPoint p2;
	float dx;
	float dy;

	// ensure a positive x
	if (line.m_start.m_x <= line.m_end.m_x) {
		p1 = line.m_start;
		p2 = line.m_end;
	}
	else {
		p1 = line.m_end;
		p2 = line.m_start;
	}
	dx = p2.m_x - p1.m_x;
	dy = p2.m_y - p1.m_y;

	Vector2d normal;
	normal.x = -dy;
	normal.y = dx;

	float magnitude = sqrt((dx * dx) + (dy * dy));
	Vector2d unitNormal;
	unitNormal.x = normal.x * magnitude;
	unitNormal.y = normal.y * magnitude;

	return Vector2d(unitNormal);
	//float distance = line.DistanceToLine(ball.center.m_x, ball.center.m_y);
}