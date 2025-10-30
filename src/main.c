// main_raylib.c
#include "raylib.h"
#include "flight_logic.h"
#include <stdio.h>
#include <string.h>

typedef enum
{
	NONE,
	ADDING_FLIGHT_ID,
	ADDING_FLIGHT_PRIORITY,
	ADDING_LUGGAGE_FLIGHT,
	ADDING_LUGGAGE_ID
} InputState;

int main()
{
	InitWindow(900, 700, "✈️  Flight Runway Management Visual");
	SetTargetFPS(60);

	struct PriorityQueue pq;
	pq.size = 0;

	char statusMsg[100] = "System Ready. Press 1 to add flight, 2 to add luggage, 3 to clear runway.";

	// Input state management
	InputState inputState = NONE;
	char inputBuffer[32] = "";
	int inputLength = 0;

	// Temporary storage for adding flights/luggage
	char tempFlightID[10] = "";
	char tempLuggageFlightID[10] = "";
	int tempPriority = 0;

	while (!WindowShouldClose())
	{
		// ---------------- KEYBOARD INPUT ----------------
		if (inputState == NONE)
		{
			if (IsKeyPressed(KEY_ONE))
			{
				inputState = ADDING_FLIGHT_ID;
				inputLength = 0;
				inputBuffer[0] = '\0';
				sprintf(statusMsg, "Enter Flight ID (press ENTER when done):");
			}
			else if (IsKeyPressed(KEY_TWO))
			{
				if (pq.size > 0)
				{
					inputState = ADDING_LUGGAGE_FLIGHT;
					inputLength = 0;
					inputBuffer[0] = '\0';
					sprintf(statusMsg, "Enter Flight ID to add luggage:");
				}
				else
				{
					sprintf(statusMsg, "No flights in queue to add luggage.");
				}
			}
			else if (IsKeyPressed(KEY_THREE))
			{
				if (pq.size > 0)
				{
					sprintf(statusMsg, "Flight %s cleared for takeoff!", pq.planes[0].flightID);
					dequeue(&pq);
				}
				else
				{
					sprintf(statusMsg, "No flights in queue.");
				}
			}
		}
		else
		{
			// Handle text input for various states
			int key = GetCharPressed();

			// Add characters to input buffer
			while (key > 0)
			{
				if ((key >= 32) && (key <= 125) && (inputLength < 31))
				{
					inputBuffer[inputLength] = (char)key;
					inputBuffer[inputLength + 1] = '\0';
					inputLength++;
				}
				key = GetCharPressed();
			}

			// Handle backspace
			if (IsKeyPressed(KEY_BACKSPACE))
			{
				if (inputLength > 0)
				{
					inputLength--;
					inputBuffer[inputLength] = '\0';
				}
			}

			// Handle enter key
			if (IsKeyPressed(KEY_ENTER) && inputLength > 0)
			{
				if (inputState == ADDING_FLIGHT_ID)
				{
					strcpy(tempFlightID, inputBuffer);
					inputState = ADDING_FLIGHT_PRIORITY;
					inputLength = 0;
					inputBuffer[0] = '\0';
					sprintf(statusMsg, "Enter Priority (1=Emergency, 2=VIP, 3=Normal):");
				}
				else if (inputState == ADDING_FLIGHT_PRIORITY)
				{
					tempPriority = atoi(inputBuffer);
					if (tempPriority >= 1 && tempPriority <= 3)
					{
						enqueue(&pq, tempFlightID, tempPriority);
						sprintf(statusMsg, "Flight %s added with priority %d!", tempFlightID, tempPriority);
						inputState = NONE;
					}
					else
					{
						sprintf(statusMsg, "Invalid priority! Enter 1, 2, or 3:");
						inputLength = 0;
						inputBuffer[0] = '\0';
					}
				}
				else if (inputState == ADDING_LUGGAGE_FLIGHT)
				{
					strcpy(tempLuggageFlightID, inputBuffer);

					// Check if flight exists
					int found = -1;
					for (int i = 0; i < pq.size; i++)
					{
						if (strcmp(pq.planes[i].flightID, tempLuggageFlightID) == 0)
						{
							found = i;
							break;
						}
					}

					if (found >= 0)
					{
						if (pq.planes[found].luggageCount >= MAX_LUGGAGE)
						{
							sprintf(statusMsg, "Luggage limit reached for flight %s!", tempLuggageFlightID);
							inputState = NONE;
						}
						else
						{
							inputState = ADDING_LUGGAGE_ID;
							inputLength = 0;
							inputBuffer[0] = '\0';
							sprintf(statusMsg, "Enter Luggage ID:");
						}
					}
					else
					{
						sprintf(statusMsg, "Flight %s not found in queue!", tempLuggageFlightID);
						inputState = NONE;
					}
				}
				else if (inputState == ADDING_LUGGAGE_ID)
				{
					// Find the flight and add luggage
					for (int i = 0; i < pq.size; i++)
					{
						if (strcmp(pq.planes[i].flightID, tempLuggageFlightID) == 0)
						{
							strcpy(pq.planes[i].luggage[pq.planes[i].luggageCount], inputBuffer);
							pq.planes[i].luggageCount++;
							sprintf(statusMsg, "Luggage %s added to flight %s!", inputBuffer, tempLuggageFlightID);
							break;
						}
					}
					inputState = NONE;
				}
			}

			// Handle ESC to cancel input
			if (IsKeyPressed(KEY_ESCAPE) && inputState != NONE)
			{
				inputState = NONE;
				sprintf(statusMsg, "Input cancelled. System ready.");
			}
		}

		// ---------------- DRAWING ----------------
		BeginDrawing();
		ClearBackground(DARKBLUE);

		DrawText("✈️  Flight Runway Management System", 180, 20, 28, RAYWHITE);
		DrawText("Controls: [1] Add Flight | [2] Add Luggage | [3] Clear Runway | [ESC] Exit",
				 80, 60, 18, LIGHTGRAY);

		// Status message
		DrawRectangle(50, 90, 800, 40, DARKGRAY);
		DrawText(statusMsg, 60, 100, 18, YELLOW);

		// Input box (when active)
		if (inputState != NONE)
		{
			DrawRectangle(50, 140, 800, 50, GRAY);
			DrawRectangleLines(50, 140, 800, 50, YELLOW);
			DrawText(inputBuffer, 60, 155, 24, WHITE);

			// Blinking cursor
			if ((int)(GetTime() * 2) % 2 == 0)
			{
				int textWidth = MeasureText(inputBuffer, 24);
				DrawText("_", 60 + textWidth, 155, 24, WHITE);
			}
		}

		// Draw runway
		int runwayY = 600;
		DrawRectangle(100, runwayY, 700, 15, GRAY);
		DrawText("RUNWAY", 380, runwayY - 25, 20, LIGHTGRAY);

		// Draw queued planes
		int startY = runwayY - 80;
		for (int i = 0; i < pq.size && i < 6; i++)
		{
			int y = startY - i * 70;

			// Priority color
			Color color = (pq.planes[i].priority == 1) ? RED : (pq.planes[i].priority == 2) ? ORANGE
																							: GREEN;

			// Plane box
			DrawRectangle(150, y, 250, 50, color);
			DrawRectangleLines(150, y, 250, 50, BLACK);

			// Flight info
			DrawText(pq.planes[i].flightID, 170, y + 8, 22, BLACK);

			char priorityText[20];
			sprintf(priorityText, "P%d", pq.planes[i].priority);
			DrawText(priorityText, 170, y + 28, 16, BLACK);

			// Luggage count
			char luggageText[30];
			sprintf(luggageText, "🧳 %d", pq.planes[i].luggageCount);
			DrawText(luggageText, 320, y + 18, 20, DARKGRAY);

			// Position indicator
			char posText[10];
			sprintf(posText, "#%d", i + 1);
			DrawText(posText, 110, y + 18, 20, LIGHTGRAY);
		}

		// Queue info
		DrawText(TextFormat("Flights in Queue: %d", pq.size), 550, 220, 22, RAYWHITE);

		if (pq.size > 0)
		{
			DrawText("Next Flight:", 550, 260, 20, LIGHTGRAY);
			DrawText(pq.planes[0].flightID, 550, 285, 24, YELLOW);

			char nextPriorityText[50];
			sprintf(nextPriorityText, "Priority: %d (%s)",
					pq.planes[0].priority,
					pq.planes[0].priority == 1 ? "EMERGENCY" : pq.planes[0].priority == 2 ? "VIP"
																						  : "NORMAL");
			DrawText(nextPriorityText, 550, 315, 18, LIGHTGRAY);
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}