/*
Original Code provided by Ubisoft Toronto for NXT challenge 2018
Modified by Craig Trustham

Last modified March 17, 2020
*/

//------------------------------------------------------------------------
// GameTest.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
//------------------------------------------------------------------------
#include <fstream>
#include "table.h"
#include "GameObjects.h"
#include "Vectors.h"

CTable* playTable;
const char* defaultFileName = "table.txt";
const char* statusMessage = "All Good";
std::vector<Flipper> flippers;
std::vector<Ball> balls;

void Load(const char* fileName)
{
	std::fstream file;
	file.open(fileName, std::fstream::in);
	if (file.is_open())
	{
		int version = 0;
		file >> version;
		if (version <= 1)
		{
			playTable->m_lines.clear();
			while (!file.eof())
			{
				CLineSegment line;
				int type;
				file >> type;
				line.m_type = static_cast<LineType>(type);
				file >> line.m_start.m_x;
				file >> line.m_start.m_y;
				file >> line.m_end.m_x;
				file >> line.m_end.m_y;
				playTable->m_lines.push_back(line);
			}

			statusMessage = "Game LOADED OK";
		}
		else
		{
			statusMessage = "LOAD FAILED: UNHANDLED VERSION";
		}

		file.close();
	}
	else
	{
		// error message!
		statusMessage = "LOAD FAILED";
	}
}

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	playTable = new CTable;
	Load(defaultFileName); // load the level/game field

	// create all the flippers for the game
	for (int i = 0 ; i < playTable->m_lines.size() ; i++)
	{
		if (playTable->m_lines[i].m_type == eLine_Flipper) {
			flippers.push_back(Flipper(&playTable->m_lines[i]));
		}
	}

	// create the first ball
	balls.push_back(Ball());

	balls[0].AddForce(0, -1.0f);
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(float deltaTime)
{
	flippers[0].isFlipped = false;
	flippers[1].isFlipped = false;
	if (App::IsKeyPressed(VK_SPACE))
	{
		flippers[0].isFlipped = true;
		flippers[1].isFlipped = true;
	}
	for (auto& flipper : flippers)
	{
		if (flipper.line->IsOnLine(balls[0].center.m_x, balls[0].center.m_y, balls[0].radius+1)) { //if the ball hits the flipper
			balls[0].Bounce(*flipper.line);
			if (flipper.isFlipped) {
				Vector2d force = flipper.GetForce();
				balls[0].velocity = balls[0].velocity + force;
			}
		}
	}

	flippers[0].ActivateLeft();
	flippers[1].ActivateRight();

	//else if(balls[0].velocity.y < -1.5f){
	//	balls[0].velocity.y = -1.5f;
	//}

	for (auto& line : playTable->m_lines)
	{
		if (line.m_type != eLine_Flipper) {
			if (line.IsOnLine(balls[0].center.m_x, balls[0].center.m_y, balls[0].radius)) {
				balls[0].Bounce(line);
			}
		}
	}

	balls[0].Update(deltaTime);
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine or Print) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{	
	// display debug info
	float y = 740.0f;
	float dy = -12.0f;
	char buffer[256];
	char* displayString = buffer;
	char* test = buffer;
	strcpy_s(buffer, 255, statusMessage);
	int length = static_cast<int>(strlen(statusMessage));
	while (test - buffer <= length)
	{
		if (*test == '\n' || *test == 0)
		{
			*test = 0;
			App::Print(10.0f, y, displayString, 1.0f, 1.0f, 1.0f, GLUT_BITMAP_8_BY_13);
			displayString = test + 1;
			y += dy;
		}
		++test;
	}

	// draw the level
	for (auto& line : playTable->m_lines)
	{
		CLineDefinition& def = playTable->m_lineDefs[line.m_type];
		App::DrawLine(line.m_start.m_x, line.m_start.m_y, line.m_end.m_x, line.m_end.m_y, def.m_Red, def.m_Green, def.m_Blue);

		y += dy;
	}

	for each (auto flipper in flippers)
	{
		flipper.Draw();
	}
	for each (auto ball in balls)
	{
		ball.Draw();
	}
}

//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{
}

