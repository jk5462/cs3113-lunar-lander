#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

#define PLATFORM_COUNT 17

struct GameState {
	Entity player;
	Entity platforms[PLATFORM_COUNT];
	Entity goal;
	Entity success;
	Entity fail;
};

GameState state;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    state.player.textureID = LoadTexture("ship.png");
    state.player.position = glm::vec3(2, 4, 0);
    state.player.acceleration = glm::vec3(0, -1.0f, 0);
	state.player.isStatic = false;
	state.player.width = 0.7;

	GLuint tileTextureID = LoadTexture("tile.png");
	GLuint goalTextureID = LoadTexture("goal.png");
	GLuint successTextureID = LoadTexture("success.png");
	GLuint failTextureID = LoadTexture("fail.png");

	//Bottom Tiles
	for (int i = 0; i < 5; i++) {
		state.platforms[i].textureID = tileTextureID;
		state.platforms[i].position = glm::vec3(-4.5 + i, -3.25f, 0);
	}

	for (int i = 5; i < 9; i++) {
		state.platforms[i].textureID = tileTextureID;
		state.platforms[i].position = glm::vec3(-3.5 + i, -3.25f, 0);
	}

	//Other Tiles
	for (int i = 9; i < 11; i++) {
		state.platforms[i].textureID = tileTextureID;
		state.platforms[i].position = glm::vec3(-4.5 + (i - 9), 1, 0);
	}

	//Other Tiles
	for (int i = 11; i < 17; i++) {
		state.platforms[i].textureID = tileTextureID;
		state.platforms[i].position = glm::vec3(0.5 + (i - 12), 0, 0);
	}

	state.goal.textureID = goalTextureID;
	state.goal.position = glm::vec3(0.5, -3.24f, 0);

	state.success.textureID = successTextureID;
	state.success.position = glm::vec3(0, 0, 0);
	state.success.isActive = false;
	state.success.vertices[0] = -2;
	state.success.vertices[2] = 2;
	state.success.vertices[4] = 2;
	state.success.vertices[6] = -2;
	state.success.vertices[8] = 2;
	state.success.vertices[10] = -2;

	state.fail.textureID = failTextureID;
	state.fail.position = glm::vec3(0, 0, 0);
	state.fail.isActive = false;
	state.fail.vertices[0] = -1.75;
	state.fail.vertices[2] = 1.75;
	state.fail.vertices[4] = 1.75;
	state.fail.vertices[6] = -1.75;
	state.fail.vertices[8] = 1.75;
	state.fail.vertices[10] = -1.75;

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        // Some sort of action
                        break;
                        
                }
                break;
        }
    }
	// Check for pressed/held keys below
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_A])
	{
		state.player.acceleration.x = -8.0f;
	}
	else if (keys[SDL_SCANCODE_D])
	{
		state.player.acceleration.x = 8.0f;
	}
	else if (keys[SDL_SCANCODE_R])
	{
		state.player.isStatic = false;
		state.player.velocity = glm::vec3(0, 0, 0);
		state.player.acceleration = glm::vec3(0, -1.0f, 0);
		state.player.position = glm::vec3(2, 4, 0);
		state.success.isActive = false;
		state.fail.isActive = false;

	}
	else {
		state.player.acceleration.x = 0.0f;
	}
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	deltaTime += accumulator;
	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}

	while (deltaTime >= FIXED_TIMESTEP) {
		// Update. Notice it's FIXED_TIMESTEP. Not deltaTime
		state.player.Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.goal, &state.success, &state.fail);
		deltaTime -= FIXED_TIMESTEP;
	}

	accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    state.player.Render(&program);

	state.goal.Render(&program);

	for (int i = 0; i < PLATFORM_COUNT; i++) {
		state.platforms[i].Render(&program);
	}

	if (state.success.isActive == true) {
		state.success.Render(&program);
	}
	else if (state.fail.isActive == true) {
		state.fail.Render(&program);
	}
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
