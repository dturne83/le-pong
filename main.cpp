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


void FadeOutMusic(Music music_stream) {
	// Lower the volume from 1 to 0 over the course of 2 seconds
	for (float i = 10000.0f; i > 0; i--) {
		SetMusicVolume(music_stream, i/100);
	}
	// Stop the music stream to restart the song next time we are at the title screen
	StopMusicStream(music_stream);
	SetMusicVolume(music_stream, 1);
}

int main()
{
	// Set screen size here
	const float WIDTH = 1600.0f;
	const float HEIGHT = 1200.0f;

	// Declare some font size constants
	const float TITLE_SIZE = 72.0f;
	const float START_MSG_SIZE = 30.0f;
	const float CONTROLS_SIZE = 20.0f;

	// Hide FPS initially
	bool show_fps = true;

	// Initialize window
	InitWindow(WIDTH, HEIGHT, "DUFFMASTERPONG");

	// Set vsync to cap framerate
	SetWindowState(FLAG_VSYNC_HINT);

	// Initialize audio device
	InitAudioDevice();

	// Declare our audio files
	const Music TITLE_MUSIC = LoadMusicStream("resources/title.mp3");
	const Sound PADDLE_HIT_FX = LoadSound("resources/paddle_hit.mp3");
	const Sound WINNER_FX = LoadSound("resources/winner_sound.mp3");
	//Sound crowd_fx = LoadSound("resources/crowd_cheering.mp3");

	// Create our ball
	Ball ball{};
	ball.posX = GetScreenWidth() / 2.0f;
	ball.posY = GetScreenHeight() / 2.0f;
	ball.radius = 8.0f;
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
	const char* WINNER_TEXT = nullptr; // this is a null pointer on purpose, we use it to check for a win condition
	const char* TITLE_TEXT = "LE PONG";
	const char* START_MSG = "Press Space to play, Esc to quit";
	const char* CONTROLS_MSG = "Use W and S to control left paddle, Up and Down to control right paddle";
	const char* RESTART_MSG = "Press space to play again, B to go back to title screen";


	while (!WindowShouldClose())
	{

		// Begin drawing the game
		BeginDrawing();

		// Draw a black background
		ClearBackground(BLACK);

		// Show title screen on first run
		if (TITLE_TEXT) {
			// Stop any other playing sounds and start the title music stream
			StopSound(WINNER_FX);
			PlayMusicStream(TITLE_MUSIC);
			UpdateMusicStream(TITLE_MUSIC);
			
			int title_width = MeasureText(TITLE_TEXT, TITLE_SIZE);
			int start_msg_width = MeasureText(START_MSG, START_MSG_SIZE);
			int controls_msg_width = MeasureText(CONTROLS_MSG, CONTROLS_SIZE);

			// Draw title screen text
			DrawText(TITLE_TEXT, (GetScreenWidth() / 2.0f) - (title_width / 2), (GetScreenHeight() / 2.0f) - 100, TITLE_SIZE, GREEN);
			DrawText(START_MSG, (GetScreenWidth() / 2.0f) - (start_msg_width / 2), (GetScreenHeight() / 2.0f) + 120, START_MSG_SIZE, WHITE);
			DrawText(CONTROLS_MSG, (GetScreenWidth() / 2.0f) - (controls_msg_width / 2), (GetScreenHeight() / 2.0f) + 180, CONTROLS_SIZE, WHITE);

			// Launch the game is the user presses space bar
			if (IsKeyPressed(KEY_SPACE)) {
				TITLE_TEXT = nullptr;
			}
		}
		else {
			// Stop the music stream to restart the song next time we are at the title screen
			//StopMusicStream(TITLE_MUSIC);

			// Fade out the title music
			FadeOutMusic(TITLE_MUSIC);

			// Begin moving the ball
			ball.posX += ball.speedX * GetFrameTime();
			ball.posY += ball.speedY * GetFrameTime();

			// Bounce the ball off of the top of the screen
			if (ball.posY > GetScreenHeight()) {
				// Play sound 
				PlaySound(PADDLE_HIT_FX);
				// set ball position to top of screen so it doesn't fly out of the frame
				ball.posY = GetScreenHeight();
				// reverse y direction
				ball.speedY *= -1;
			}

			// Bounce the ball off of the bottom of the screen
			if (ball.posY < 0) {
				// Play sound 
				PlaySound(PADDLE_HIT_FX);
				// set ball position to bottom of screen so it doesn't fly out of the frame
				ball.posY = 0;
				// reverse y direction
				ball.speedY *= -1;
			}

			// Move left paddle up with 'W'
			if (!WINNER_TEXT && IsKeyDown(KEY_W)) {
				left_paddle.posY -= left_paddle.speed * GetFrameTime();
				// limit movement of the paddle so half of it stays in the frame
				// allow half of the paddle to go off screen to allow for deliberate direction changes
				if (left_paddle.posY < 0) {
					left_paddle.posY = 0;
				}
			}

			// Move left paddle down with 'S'
			if (!WINNER_TEXT && IsKeyDown(KEY_S)) {
				left_paddle.posY += left_paddle.speed * GetFrameTime();
				// limit movement of the paddle so half of it stays in the frame
				// allow half of the paddle to go off screen to allow for deliberate direction changes
				if (left_paddle.posY > GetScreenHeight()) {
					left_paddle.posY = GetScreenHeight();
				}
			}

			// Move right paddle up with 'UP_ARROW'
			if (!WINNER_TEXT && IsKeyDown(KEY_UP)) {
				right_paddle.posY -= right_paddle.speed * GetFrameTime();
				// allow half of the paddle to go off screen to allow for deliberate direction changes
				if (right_paddle.posY < 0) {
					right_paddle.posY = 0;
				}
			}

			// Move right paddle up with 'DOWN_ARROW'
			if (!WINNER_TEXT && IsKeyDown(KEY_DOWN)) {
				right_paddle.posY += right_paddle.speed * GetFrameTime();
				// allow half of the paddle to go off screen to allow for deliberate direction changes
				if (right_paddle.posY > GetScreenHeight()) {
					right_paddle.posY = GetScreenHeight();
				}
			}

			// Handle collisions with the left paddle
			if (CheckCollisionCircleRec(Vector2{ ball.posX, ball.posY }, ball.radius, Rectangle{ left_paddle.GetRect() })) {
				// If the balls hit the paddle, play sound and reverse x direction.  
				PlaySound(PADDLE_HIT_FX);
				// Depending on if ball hits top or bottom half of the paddle, bounce in that direction
				if (ball.speedX < 0) {
					ball.speedX *= -1.1f;
					ball.speedY = (ball.posY - left_paddle.posY) / (left_paddle.height / 2) * ball.speedX;
				}

			}

			// Handle collisions with the right paddle
			if (CheckCollisionCircleRec(Vector2{ ball.posX, ball.posY }, ball.radius, Rectangle{ right_paddle.GetRect() })) {
				// If the balls hit the paddle, play sound and reverse x direction.
				PlaySound(PADDLE_HIT_FX);
				// Depending on if ball hits top or bottom half of the paddle, bounce in that direction
				if (ball.speedX > 0) {
					ball.speedX *= -1.1f;
					ball.speedY = (ball.posY - right_paddle.posY) / (right_paddle.height / 2) * -ball.speedX;
				}

			}

			// If ball goes off of the left side of the screen, player 2 is the winner
			if (ball.posX < 0)
			{
				PlaySound(WINNER_FX);
				ball.posX = 0;
				ball.speedX = 0;
				ball.speedY = 0;

				// Update the winner message to show the winner screen
				WINNER_TEXT = "Right Player Wins!!";
			}

			// If ball goes off of the left side of the screen, player 1 is the winner
			if (ball.posX > GetScreenWidth())
			{
				PlaySound(WINNER_FX);
				ball.posX = GetScreenWidth();
				ball.speedX = 0;
				ball.speedY = 0;

				// Update the winner message to show the winner screen
				WINNER_TEXT = "Left Player Wins!!";
			}

			// Draw the winner message if we detect a win condition
			if (WINNER_TEXT) {
				int WINNER_TEXT_width = MeasureText(WINNER_TEXT, TITLE_SIZE);
				int restart_msg_width = MeasureText(RESTART_MSG, CONTROLS_SIZE);

				// Display winner message to user
				DrawText(WINNER_TEXT, (GetScreenWidth() / 2.0f) - (WINNER_TEXT_width / 2), (GetScreenHeight() / 2.0f), TITLE_SIZE, YELLOW);
				DrawText(RESTART_MSG, (GetScreenWidth() / 2.0f) - (restart_msg_width / 2), (GetScreenHeight() / 2.0f) + 160, CONTROLS_SIZE, WHITE);
			}

			// Restart the game using SPACE_BAR
			if (WINNER_TEXT && IsKeyPressed(KEY_SPACE)) {
				ball.posX = GetScreenWidth() / 2.0f;
				ball.posY = GetScreenHeight() / 2.0f;
				ball.speedX = 300;
				ball.speedY = 300;
				WINNER_TEXT = nullptr;
			}

			// Go back to title screen using B
			if (WINNER_TEXT && IsKeyPressed(KEY_B)) {
				// Reset our ball position
				// TODO: Alter this to use a random position 
				ball.posX = GetScreenWidth() / 2.0f;
				ball.posY = GetScreenHeight() / 2.0f;
				ball.speedX = 300;
				ball.speedY = 300;

				// Set the winner text to a null pointer to hide winner message
				WINNER_TEXT = nullptr;

				// Set the title text to show the title screen
				TITLE_TEXT = "DUFF PONG BITCH";
			}

			// Draw the ball and paddles
			ball.Draw();
			left_paddle.Draw();
			right_paddle.Draw();
		}


		if (IsKeyPressed(KEY_F)) {
			show_fps = !show_fps;	// Toggle FPS with F key
		}

		if (show_fps) {
			DrawFPS(10, 10);	// Show FPS on the screen	
		}

		EndDrawing();		// Kill all drawing functions
	}


	// Unload sound data
	UnloadMusicStream(TITLE_MUSIC);
	UnloadSound(PADDLE_HIT_FX);
	//UnloadSound(side_hit_fx);
	UnloadSound(WINNER_FX);
	//UnloadSound(crowd_fx);

	// Close audio device
	CloseAudioDevice();

	// Close down the window
	CloseWindow();
	return 0;
}
