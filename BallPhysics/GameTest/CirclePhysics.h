/*
Created By Craig Trustham
using https://www.youtube.com/watch?v=LPzyNOHY3A4 for reference.

Last update: March 17 2020
*/
#pragma once
#include <iostream> 
#include <utility> // for pair
#include <vector> 
#include <cmath>
#include <stdio.h>
#include <math.h>
#include "App/app.h"
#include <ctime> // for random numbers

using namespace std;

struct Ball {
	float px, py; // pos
	float vx, vy; // velo
	float ax, ay; // accel
	float prevX, prevY; // previous frame's position
	float r; // radius
	float mass;
	int id; // unique id

	float timeSliceRemaining; // keeps track of timing for physics events within the current time slice -> keeps track of how much time is left for the ball within a time epoch

	Ball();
};

// creates a 2d 'line' that starts and ends with a ball and has a top and a bottom ( effectively a round-end rectangle )
struct Line {
	Ball sBall = Ball(); // Start coord
	Ball eBall = Ball(); // end coord
	Ball collisionBall = Ball();

	Line(float sx, float sy, float ex, float ey, float r);
};

class CirclePhysics
{
private:
	vector<pair<float, float>> unitCircle; // holds the points that form a circle. Used as a base for drawing a circle on screen
	vector<Ball> balls;
	vector<Line> lines;
	Ball* pSelectedBall = nullptr; // the ball the user has clicked on
	Line* pSelectedLine = nullptr; // the line the user has clicked on

	void AddBall(float x, float y, float r = 15.0f); // creates, initializes and adds balls to the balls vector

public:
	CirclePhysics();
	~CirclePhysics() {};

	void CreateCircle();

	void DrawCircles();

	void DrawLines();

	void DrawBall(Ball ball, float r = 1.0f, float g = 1.0f, float b = 1.0f);

	bool CirclesOverlap(Ball b1, Ball b2); // returns true if two balls/circles overlap

	float DistBetween(Ball b1, Ball b2); // returns the distance between two balls

	float PythagThrm(float x, float y); // returns sqrt(x * x + y * y)

	bool PointInCircle(Ball ball, float x, float y); // checks if an x,y point is inside a ball's radius

	void Update(float deltaTime); // Update ball positions, checks and resolves collisions, checks for user interaction
};