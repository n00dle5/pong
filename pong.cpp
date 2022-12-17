#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "rect.hpp"

//global variables
bool running = true;
int playerSpeedY = 20;
int WIDTH = 640;
int HEIGHT = 480;
int paddleWidth = 10;
int paddleHeight = 100;
int ballSpeedX = 1;
int ballSpeedY = 1;


void logSDLError(std::ostream &os, const std::string &msg) {
    os << msg << " error: " << SDL_GetError() << std::endl;
}

void cleanup(SDL_Window *window, SDL_Renderer *renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void input(Rect &player) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_UP:
                        if (player.getY() > 0) {
                            player.setY(player.getY() - playerSpeedY);
                        }
                        break;
                    case SDLK_DOWN:
                        if (player.getY() < HEIGHT - paddleHeight) {
                            player.setY(player.getY() + playerSpeedY);
                        }
                        break;
                }
                break;
        }
    }
}

void render(std::vector<Rect> rects, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (Rect rect : rects) {
        rect.drawRect(renderer);
    }

    SDL_RenderPresent(renderer);
}

void ballMove(Rect &ball, Rect &paddle) {
    SDL_Rect ballRect = ball.getRect();
    SDL_Rect paddleRect = paddle.getRect();

    if (SDL_HasIntersection(&ballRect, &paddleRect)) {
        ballSpeedX *= -1;
    }

    if (ball.getX() > WIDTH || ball.getX() < 0) {
        running = false;
    }
    ball.setX(ball.getX() + ballSpeedX);

    if (ball.getY() > HEIGHT - 10 || ball.getY() < 0) {
        ballSpeedY *= -1;
    }
    ball.setY(ball.getY() + ballSpeedY);
}

void AI(Rect &paddle, Rect ball) {
    if (paddle.getY() + paddleHeight/2 < ball.getY() + 5) {
        paddle.setY(paddle.getY() + playerSpeedY/2);
    }

    if (paddle.getY() + paddleHeight/2 > ball.getY() + 5) {
        paddle.setY(paddle.getY() - playerSpeedY/2);
    }
}


int main(int argc, char *argv[]) {
    //window to render to
    SDL_Window *window;
    //surface contained by window
    SDL_Renderer *renderer;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        logSDLError(std::cout, "SDL_Init");
    }

    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) != 0) {
        logSDLError(std::cout, "SDL_CreateWindowAndRenderer");
    }

    SDL_SetWindowTitle(window, "Pong");

    Rect l_paddle(0, HEIGHT/2, paddleWidth, paddleHeight);
    Rect r_paddle(WIDTH - paddleWidth, HEIGHT/2, paddleWidth, paddleHeight);
    Rect ball(WIDTH/2, HEIGHT/2, 10, 10);

    while (running) {
        SDL_Delay(5);
        input(l_paddle);
        ballMove(ball, l_paddle);
        ballMove(ball, r_paddle);
        AI(r_paddle, ball);
        render({l_paddle, r_paddle, ball}, renderer);
    }

    SDL_Delay(3000);

    cleanup(window, renderer);

    return 0;
};