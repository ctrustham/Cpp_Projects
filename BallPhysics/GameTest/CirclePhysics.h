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

	Ball() {
		px = 0;
		py = 0;
		vx = 0;
		vy = 0;
		ax = 0;
		ay = 0;
		prevX = 0;
		prevY = 0;
		r = 1;
		mass = 1;
		id = -1;
		timeSliceRemaining = 0.0f;
	}
};

// creates a 2d 'line' that starts and ends with a ball and has a top and a bottom ( effectively a round-end rectangle )
struct Line {
	Ball sBall = Ball(); // Start coord
	Ball eBall = Ball(); // end coord
	Ball collisionBall = Ball();

	Line(float sx, float sy, float ex, float ey, float r) {
		sBall.px = sx;
		sBall.py = sy;
		sBall.r = r;
		eBall.px = ex;
		eBall.py = ey;
		eBall.r = r;
	}
};

class CirclePhysics
{
private:
	vector<pair<float, float>> unitCircle; // holds the points that form a circle. Used as a base for drawing a circle on screen
	vector<Ball> balls;
	vector<Line> lines;
	Ball* pSelectedBall = nullptr; // the ball the user has clicked on
	Line* pSelectedLine = nullptr; // the line the user has clicked on

	void AddBall(float x, float y, float r = 15.0f) {
		// create a ball and initialize values
		Ball b = Ball();
		b.px = x;
		b.py = y;
		b.vx = 0;
		b.vy = 0;
		b.ax = 0;
		b.ay = 0;
		b.r = r;
		b.mass = 1;

		b.id = balls.size(); // give the ball a unique id

		balls.push_back(b); // add ball to vector of balls
	}

public:
	CirclePhysics() {
		CreateCircle();

		srand((int)time(0)); // set the random seed for rand() -> based off system time to be sudo-random
		for(int i =0; i < 10; i++)
			AddBall(rand() % (APP_VIRTUAL_WIDTH /2) + (APP_VIRTUAL_WIDTH / 4), rand() % (APP_VIRTUAL_HEIGHT /2) + (APP_VIRTUAL_HEIGHT / 4));
		
		float lineRad = 5.0f; // radius of line
		float lineX = rand() % (APP_VIRTUAL_WIDTH / 2) + (APP_VIRTUAL_WIDTH / 4);
		float lineY = rand() % (APP_VIRTUAL_HEIGHT / 2) + (APP_VIRTUAL_HEIGHT / 4);
		Line l = { lineX, lineY, lineX + 100.0f, lineY + 40.0f, lineRad};
		lines.emplace_back(l);
	}
	~CirclePhysics() {};

	void CreateCircle() {
		unitCircle.emplace_back(0.0f, 0.0f); // emplace: new element [a 'pair' here] is constructed in place using args as the arguments for its constructor.
		// using push_back() to insert: 
		//unitCircle.push_back(make_pair(0.0f, 0.0f));

		int segments = 12;

		// create the locus of points around the center
		for (int i = 0; i < segments; i++)
		{
			pair<float, float> pt = { cosf(i / (float)(segments) * 2.0f * PI) , sinf(i / (float)(segments) * 2.0f * PI) };
			unitCircle.push_back(pt);
		}
	}

	void DrawCircles() {
		for (auto &ball : balls) {
			if (pSelectedBall == &ball) {
				if (App::IsKeyPressed(VK_RBUTTON)) {
					//DrawCircle(ball.px, ball.py, ball.r, unitCircle.size(), 0.0f, 0.0f, 1.0f);
					DrawBall(ball, 0.0f, 0.0f, 1.0f);
				}
				else {
					//DrawCircle(ball.px, ball.py, ball.r, unitCircle.size(), 0.0f, 1.0f, 0.0f);
					DrawBall(ball, 0.0f, 1.0f, 0.0f);
				}
			}
			else {
				//DrawCircle(ball.px, ball.py, ball.r, unitCircle.size() -1, 1.0f, 1.0f, 1.0f);
				DrawBall(ball);
			}
		}
	}

	void drawLines() {
		for (auto& line : lines) {
			DrawBall(line.sBall);
			DrawBall(line.eBall);

			// get normal of line
			float dx, dy;
			dx = line.sBall.px - line.eBall.px;
			dy = line.sBall.py - line.eBall.py;
			float dist = PythagThrm(dx, dy);
			float normX =  -dy / dist;
			float normY = dx / dist;

			//line from top of start to top of end
			App::DrawLine(line.sBall.px + normX * line.eBall.r, line.sBall.py + normY * line.sBall.r, line.eBall.px + normX * line.eBall.r, line.eBall.py + normY * line.eBall.r);
			//line from tbottom of start to bottom of end
			App::DrawLine(line.sBall.px - normX * line.sBall.r, line.sBall.py - normY * line.sBall.r, line.eBall.px - normX * line.eBall.r, line.eBall.py - normY * line.eBall.r);
		}
	}

	void DrawBall(Ball ball, float r = 1.0f, float g = 1.0f, float b = 1.0f) {
		float x1, y1, x2, y2;
		// draw lines between the vertexes of the unit circle multiplied by the ball radius, and translated to the ball's position
		for (int vertex = 2; vertex < unitCircle.size(); vertex++) {
			x1= unitCircle[vertex-1].first * ball.r + ball.px;
			y1= unitCircle[vertex-1].second * ball.r + ball.py;
			x2= unitCircle[vertex].first * ball.r + ball.px;
			y2= unitCircle[vertex].second * ball.r + ball.py;

			App::DrawLine(x1, y1, x2, y2, r, g, b);
		}

		x1 = unitCircle[1].first * ball.r + ball.px;
		y1 = unitCircle[1].second * ball.r + ball.py;
		App::DrawLine(x2, y2, x1, y1, r, g, b);
	}

	bool CirclesOverlap(Ball b1, Ball b2) {
		float dx = b1.px - b2.px;
		float dy = b1.py - b2.py;
		float distSqrd = dx * dx + dy * dy; // could use DistBetween, but using sqrtf is comparatively resource expensive, and not needed here

		if (distSqrd <= (b1.r + b2.r)*(b1.r + b2.r)) { // square both sides of the equation; see above
			return true;
		}
		else {
			return false;
		}
	}

	float DistBetween(Ball b1, Ball b2) {
		float dx = b1.px - b2.px;
		float dy = b1.py - b2.py;
		return sqrtf(dx * dx + dy * dy);
	}

	float PythagThrm(float x, float y) {
		return sqrt(x * x + y * y);
	}

	bool PointInCircle(Ball ball, float x, float y) {
		//return ((x <= ball.px + ball.r || x >= ball.px - ball.r) // check x pos
		//	&& (y <= ball.py + ball.r || y >= ball.py - ball.r)); // check y pos
		
		// OR
		  // use local coords instead of world coords (as above)
		return (x - ball.px) * (x - ball.px) + (y - ball.py) * (y - ball.py) < (ball.r * ball.r);
	}

	void Update(float deltaTime) {
		static bool mouseDown = false;
		static bool selectLineStart = false;
		auto somethingSelected = [=] { return pSelectedBall || pSelectedLine; }; // check if something is selected to avoid selecting both a line and a ball
		vector<pair<Ball*, Ball*>> vectCollidingPairs; // holds all pairs of colliding balls

		int timeSlices = 4; // how many times to subdivide the delta-time
		float elapsedTime = deltaTime / timeSlices;
		int maxBallUpdates = 15; // How many times in one epoch can a ball have its position updated and resolve collisions

		// loop to break up time into steps/slices
		for (int i = 0; i < timeSlices; i++)
		{
			//set the time within the balls to the max possible for the current epoch
			for (auto& ball : balls)
			{
				ball.timeSliceRemaining = elapsedTime;
			}

			for (int j = 0; j < maxBallUpdates; j++)
			{
				// Update ball positions - only if there is time remaining within the time epoch

				for (auto& ball : balls) {
					if (ball.timeSliceRemaining > 0.0f)
					{
						ball.prevX = ball.px;
						ball.prevY = ball.py;

						ball.ax = -ball.vx * 0.75; // add drag
						ball.ay = -ball.vy * 0.75;
						// update bsed on how much time is remaining in the time epoch
						ball.vx += ball.ax * ball.timeSliceRemaining; // add acceleration to the balls velocity
						ball.vy += ball.ay * ball.timeSliceRemaining;
						ball.px += ball.vx * ball.timeSliceRemaining; // move the ball a distance equal to it's velocity
						ball.py += ball.vy * ball.timeSliceRemaining;

						// limit the velocity to eventually become 0
						if (fabs(ball.vx * ball.vx + ball.vy * ball.vy) < 0.01f) {
							ball.vx = 0;
							ball.vy = 0;
						}
					}
				}

				// check each ball against each other ball - static collisions
				for (auto& ball : balls)
				{
					for (auto& target : balls)
					{
						if (ball.id != target.id)
						{
							if (CirclesOverlap(ball, target)) {
								vectCollidingPairs.push_back({ &ball, &target });

								// remove overlap between balls
								float dist = DistBetween(ball, target);
								float overlap = dist - ball.r - target.r;
								float displacement = 0.5f * overlap;
								// displace first ball
								ball.px -= displacement * (ball.px - target.px) / dist; // unit vector in direction of difference between centers
								ball.py -= displacement * (ball.py - target.py) / dist;
								// displace second ball
								target.px += displacement * (ball.px - target.px) / dist;
								target.py += displacement * (ball.py - target.py) / dist;
							}
						}
					}
					for (auto& line : lines) {
						// line segment vector -> SE
						float lineDX = line.eBall.px - line.sBall.px;
						float lineDY = line.eBall.py - line.sBall.py;
						float lineLen = lineDX * lineDX + lineDY * lineDY;

						// line segment from the ball to the start of the line -> PS vector
						float ballToLineDX = ball.px - line.sBall.px; 
						float ballToLineDY = ball.py - line.sBall.py;

						// determine how far along the line the ball collided with the line (length to closest point)
						float PSdotSE = lineDX * ballToLineDX + lineDY * ballToLineDY; // PS dot SE
						
						// t is the distance between the start of the line and the closest point from the ball to the line
						// clamp to 0 and length of line (1 after normalization). Clamping ensures ball is closest to end points if beyond the start or end positions
						float t = max(0, min(lineLen, PSdotSE)) / lineLen; // normalized to make line a unit vector

						// closest point from ball to line
						float closestX = line.sBall.px + t * lineDX;
						float closestY = line.sBall.py + t * lineDY;
		
						// distance from closest point to ball's center -> magniture of PT
						float MagPT = PythagThrm(ball.px - closestX, ball.py - closestY);

						if (MagPT <= ball.r + line.sBall.r) {
							// collision has occured

							// resolve dynamic collision
							// create 'fake'/phantom ball within the line and have the ball collide with that - allows use of already created collision code
							
							line.collisionBall.r = (t / lineLen) * (abs(line.sBall.r - line.eBall.r)) + min(line.sBall.r, line.eBall.r); // if the radii are not equal
							line.collisionBall.mass = ball.mass; // can add multiplier for different elasticity
							line.collisionBall.px = closestX;
							line.collisionBall.py = closestY;
							line.collisionBall.vx = -ball.vx; // provide negative velocity to counter the incoming velocity
							line.collisionBall.vy = -ball.vy;

							vectCollidingPairs.push_back({ &ball, &line.collisionBall });

							//resolve static collision
							// remove overlap
							float overlap = MagPT - ball.r - line.collisionBall.r;
							ball.px -= overlap * (ball.px - closestX) / MagPT; // unit vector in direction of difference between centers
							ball.py -= overlap * (ball.py - closestY) / MagPT;
						}
					}

					// time displacement
					float totalBallSpeed = PythagThrm(ball.vx, ball.vy);
					float distanceMovedX = ball.px - ball.prevX;
					float distanceMovedY = ball.vy - ball.prevY;
					float totalDistMoved = PythagThrm(distanceMovedX, distanceMovedY);

					ball.timeSliceRemaining -= totalDistMoved / totalBallSpeed; // calculate how long the ball took to actually collide with the object
				}

				//resolve dynamic collisions
				for (auto iter : vectCollidingPairs) {
					Ball* b1 = iter.first;
					Ball* b2 = iter.second;

					// distance
					float dist = DistBetween(*b1, *b2);

					// normal, unit vector
					float normX = (b2->px - b1->px) / dist;
					float normY = (b2->py - b1->py) / dist;

					// tangent between balls
					float tangX = -normY;
					float tangY = normX;

					// tangental response - bounce along tangent vector (90 degrees)
					// dot product with tangent
					float dotB1Tan = b1->vx * tangX + b1->vy * tangY; // between ball-1 and tangent vector
					float dotB2Tan = b2->vx * tangX + b2->vy * tangY;

					// dot product with normal
					float dotB1Norm = b1->vx * normX + b1->vy * normY; // between ball-1 and normal vector
					float dotB2Norm = b2->vx * normX + b2->vy * normY;

					// One-dimensional Newtonian (https://en.wikipedia.org/wiki/Elastic_collision)
					/*
					conservation of momentum -
					with masses m1, m2, and velocities u1, u2 before collision, v1, v2 after collision.
					The conservation of the total momentum before and after the collision is expressed by:
					M₁ · U₁ + M₂ · U₂ = M₁ · V₁ + M₂ · V₂

					Likewise, the conservation of the total kinetic energy is expressed by:
					(M₁)/2 · (U₁)² + (M₂)/2 · (U₂)² = (M₁)/2 · (V₁)² + ½(M₂) · (V₂)²

					solving for V:
					V₁ = ( U₁(M₁ - M₂) + U₂(2M₂) ) / (M₁ + M₂)
					V₂ = ( U₁(2M₁) + U₂(M₂ - M₁) ) / (M₁ + M₂)

					[saved with 'Unicode (UTF-8 with signature) - Codepage 65001' to have unicode in equations]
					*/
					float v1 = (dotB1Norm * (b1->mass - b2->mass) + 2.0f * dotB2Norm * b2->mass) / (b1->mass + b2->mass);
					float v2 = (dotB1Norm * 2.0f * b1->mass + dotB2Norm * (b1->mass - b2->mass)) / (b1->mass + b2->mass);

					b1->vx = tangX * dotB1Tan + normX * v1; // new velocity is equal to the x-component along the tangent plus the component of the normal -> the sum of the two responses
					b1->vy = tangY * dotB1Tan + normY * v1;
					b2->vx = tangX * dotB2Tan + normX * v2;
					b2->vy = tangY * dotB2Tan + normY * v2;

					// Two-dimensional collision with two moving objects (https://en.wikipedia.org/wiki/Elastic_collision)
				}
				vectCollidingPairs.clear();
			}
		}

		if (App::IsKeyPressed(VK_LBUTTON) || App::IsKeyPressed(VK_RBUTTON)) {
			float mouseX, mouseY;
			App::GetMousePos(mouseX, mouseY);

			// check balls
			
			if (!somethingSelected()){ // make sure no ball or line is already selected
				for (auto& ball : balls) {
					// if mouse is inside circle
					if (PointInCircle(ball, mouseX, mouseY)) {
						pSelectedBall = &ball;
						break; // dont need to keep checking
					}
				}
			}
			if (pSelectedBall != nullptr) {
				if (App::IsKeyPressed(VK_LBUTTON)) {
					pSelectedBall->px = mouseX;
					pSelectedBall->py = mouseY;

					// stop the ball moving if it was
					pSelectedBall->vx = 0;
					pSelectedBall->vy = 0;
				}
				else if (App::IsKeyPressed(VK_RBUTTON)) {
					mouseDown = true;
				}
			}

			// check lines
			
			if (!somethingSelected()){ // make sure no ball or line is already selected
				for (auto& line : lines) {
					// if mouse is on a line's ball
					if (PointInCircle(line.sBall, mouseX, mouseY)) { // mouse is on start ball
						pSelectedLine = &line;
						selectLineStart = true; // mouse is on start ball
						break;
					}
					if (PointInCircle(line.eBall, mouseX, mouseY)) { // mouse is on end ball
						pSelectedLine = &line;
						selectLineStart = false;
						break;
					}
				}
			}

			if (pSelectedLine != nullptr) {
				if (App::IsKeyPressed(VK_LBUTTON)) {
					if (selectLineStart) {
						pSelectedLine->sBall.px = mouseX;
						pSelectedLine->sBall.py = mouseY;
					}
					else {
						pSelectedLine->eBall.px = mouseX;
						pSelectedLine->eBall.py = mouseY;
					}
				}
			}
		}
		else {
			if (mouseDown) { // check for mouse release -> if the mouse is not down now, but it WAS before, then change the ball's velocity
				float mouseX, mouseY;
				App::GetMousePos(mouseX, mouseY);
				pSelectedBall->vx = 2.0f * (pSelectedBall->px - mouseX);
				pSelectedBall->vy = 2.0f * (pSelectedBall->py - mouseY);
				mouseDown = false; // record that the mouse is not down anymore
			}

			pSelectedBall = nullptr; // unselect ball if one is selected
			pSelectedLine = nullptr;
		}
	}
};