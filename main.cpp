/**
 * main.cpp
 *
 * Basic two-player pong game built with the RayLib library.
 * Use W and S to move the left paddle
 * Use UP_ARROW and DOWN_ARROW to move the right paddle
 *
 * 2023 Rob Vandelinder
 */

#include <string>
#include <iostream>
#include "raylib.h"
using namespace std;

// Ball object
struct Ball
{
	float posX, posY;
	float speedX, speedY;
	float radius;

	// Call to draw the paddle on screen
	void Draw() {
		DrawCircle((int)posX, (int)posY, radius, WHITE);
	};
};

// Paddle object
struct Paddle
{
	float posX, posY;
	float height, width;
	float speed;

	// Create Rectangle object
	Rectangle GetRect() {
		return Rectangle{ posX - width, posY - (height / 2), 10, 100 };
	}

	// Call to draw the paddle on screen
	void Draw() {
		DrawRectangle(posX - width / 2, posY - (height / 2), 10, 100, WHITE);
		DrawRectangleRec(GetRect(), WHITE);
	}
};

int main()
{
	// Set screen size here
	const float WIDTH = 800.0f;
	const float HEIGHT = 600.0f;

	// Hide FPS initially
	bool show_fps = false;

	InitWindow(WIDTH, HEIGHT, "Le Pong");
	SetWindowState(FLAG_VSYNC_HINT);

	// Create our ball
	Ball ball{};
	ball.posX = GetScreenWidth() / 2.0f;
	ball.posY = GetScreenHeight() / 2.0f;
	ball.radius = 5.0f;
	ball.speedX = 300.0f;
	ball.speedY = 300.0f;

	// Create our left paddle
	Paddle left_paddle{};
	left_paddle.height = 100.0f;
	left_paddle.width = 10.0f;
	left_paddle.posX = 50.0f;
	left_paddle.posY = (GetScreenHeight() / 2.0f);
	left_paddle.speed = 500;

	// Create our right paddle
	Paddle right_paddle{};
	right_paddle.height = 100.0f;
	right_paddle.width = 10.0f;
	right_paddle.posX = (GetScreenWidth() - 50);
	right_paddle.posY = (GetScreenHeight() / 2.0f);
	right_paddle.speed = 500;

	// Create message strings
	const char* winner_text = nullptr; // this is a null pointer on purpose, we use it to check for a win condition
	const char* title_text = "LE PONG";
	const char* start_msg = "Press Space to play, Esc to quit";
	const char* controls_msg = "Use W and S to control left paddle, Up and Down to control right paddle";
	const char* restart_msg = "Press space to play again, B to go back to title screen";


	while (!WindowShouldClose())
	{
		// Begin drawing the game
		BeginDrawing();

		// Draw a black background
		ClearBackground(BLACK);

		// Show title screen on first run
		if (title_text) {
			int title_width = MeasureText(title_text, 60);
			int start_msg_width = MeasureText(start_msg, 30);
			int controls_msg_width = MeasureText(controls_msg, 20);

			// Draw title screen text
			DrawText(title_text, (GetScreenWidth() / 2.0f) - (title_width / 2), (GetScreenHeight() / 2.0f) - 100, 60, GREEN);
			DrawText(start_msg, (GetScreenWidth() / 2.0f) - (start_msg_width / 2), (GetScreenHeight() / 2.0f) + 120, 30, WHITE);
			DrawText(controls_msg, (GetScreenWidth() / 2.0f) - (controls_msg_width / 2), (GetScreenHeight() / 2.0f) + 180, 20, WHITE);

			// Launch the game is the user presses space bar
			if (IsKeyPressed(KEY_SPACE)) {
				title_text = nullptr;
			}
		}
		else {
			// Begin moving the ball
			ball.posX += ball.speedX * GetFrameTime();
			ball.posY += ball.speedY * GetFrameTime();

			// Bounce the ball off of the top of the screen
			if (ball.posY > GetScreenHeight()) {
				// set ball position to top of screen so it doesn't fly out of the frame
				ball.posY = GetScreenHeight();
				// reverse y direction
				ball.speedY *= -1;
			}

			// Bounce the ball off of the bottom of the screen
			if (ball.posY < 0) {
				// set ball position to bottom of screen so it doesn't fly out of the frame
				ball.posY = 0;
				// reverse y direction
				ball.speedY *= -1;
			}

			// Move left paddle up with 'W'
			if (!winner_text && IsKeyDown(KEY_W)) {
				left_paddle.posY -= left_paddle.speed * GetFrameTime();
				// limit movement of the paddle so half of it stays in the frame
				// allow half of the paddle to go off screen to allow for deliberate direction changes
				if (left_paddle.posY < 0) {
					left_paddle.posY = 0;
				}
			}

			// Move left paddle down with 'S'
			if (!winner_text && IsKeyDown(KEY_S)) {
				left_paddle.posY += left_paddle.speed * GetFrameTime();
				// limit movement of the paddle so half of it stays in the frame
				// allow half of the paddle to go off screen to allow for deliberate direction changes
				if (left_paddle.posY > GetScreenHeight()) {
					left_paddle.posY = GetScreenHeight();
				}
			}

			// Move right paddle up with 'UP_ARROW'
			if (!winner_text && IsKeyDown(KEY_UP)) {
				right_paddle.posY -= right_paddle.speed * GetFrameTime();
				// allow half of the paddle to go off screen to allow for deliberate direction changes
				if (right_paddle.posY < 0) {
					right_paddle.posY = 0;
				}
			}

			// Move right paddle up with 'DOWN_ARROW'
			if (!winner_text && IsKeyDown(KEY_DOWN)) {
				right_paddle.posY += right_paddle.speed * GetFrameTime();
				// allow half of the paddle to go off screen to allow for deliberate direction changes
				if (right_paddle.posY > GetScreenHeight()) {
					right_paddle.posY = GetScreenHeight();
				}
			}

			// Handle collisions with the left paddle
			if (CheckCollisionCircleRec(Vector2{ ball.posX, ball.posY }, ball.radius, Rectangle{ left_paddle.GetRect() })) {
				// If the balls hit the paddle, reverse x direction.  Depending on if ball hits top or bottom half of the paddle, bounce in that direction
				if (ball.speedX < 0) {
					ball.speedX *= -1.1f;
					ball.speedY = (ball.posY - left_paddle.posY) / (left_paddle.height / 2) * ball.speedX;
				}

			}

			// Handle collisions with the right paddle
			if (CheckCollisionCircleRec(Vector2{ ball.posX, ball.posY }, ball.radius, Rectangle{ right_paddle.GetRect() })) {
				// If the balls hit the paddle, reverse x direction.  Depending on if ball hits top or bottom half of the paddle, bounce in that direction
				if (ball.speedX > 0) {
					ball.speedX *= -1.1f;
					ball.speedY = (ball.posY - right_paddle.posY) / (right_paddle.height / 2) * -ball.speedX;
				}

			}

			// If ball goes off of the left side of the screen, player 2 is the winner
			if (ball.posX < 0)
			{
				winner_text = "Right Player Wins!!";
			}

			// If ball goes off of the left side of the screen, player 1 is the winner
			if (ball.posX > GetScreenWidth())
			{
				winner_text = "Left Player Wins!!";
			}

			// Restart the game using SPACE_BAR
			if (winner_text && IsKeyPressed(KEY_SPACE)) {
				ball.posX = GetScreenWidth() / 2.0f;
				ball.posY = GetScreenHeight() / 2.0f;
				ball.speedX = 300;
				ball.speedY = 300;
				winner_text = nullptr;
			}

			// Go back to title screen using B
			if (winner_text && IsKeyPressed(KEY_B)) {
				// Reset our ball position
				// TODO: Alter this to use a random position 
				ball.posX = GetScreenWidth() / 2.0f; 
				ball.posY = GetScreenHeight() / 2.0f;
				ball.speedX = 300;
				ball.speedY = 300;

				// Set the winner text to a null pointer to hide winner message
				winner_text = nullptr;

				// Set the title text to show the title screen
				title_text = "LE PONG";
			}

			// Draw the winner message if we detect a win condition
			if (winner_text) {
				int winner_text_width = MeasureText(winner_text, 60);
				int restart_msg_width = MeasureText(restart_msg, 24);

				DrawText(winner_text, (GetScreenWidth() / 2.0f) - (winner_text_width / 2), (GetScreenHeight() / 2.0f), 60, YELLOW);
				DrawText(restart_msg, (GetScreenWidth() / 2.0f) - (restart_msg_width / 2), (GetScreenHeight() / 2.0f) + 160, 24, WHITE);

			}

			// Draw the ball and paddles
			ball.Draw();
			left_paddle.Draw();
			right_paddle.Draw();
		}

		// Toggle FPS with F key
		if (IsKeyPressed(KEY_F)) {
			show_fps = !show_fps;
		}

		// Show FPS on the screen
		if (show_fps) {
			DrawFPS(10, 10);
		}

		// Kill all drawing functions
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
