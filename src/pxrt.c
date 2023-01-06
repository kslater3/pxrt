
#include <stdio.h>

#include "SDL2/SDL.h"
#include "GL/gl.h"


int mIsRunning = 1;
GLenum glError;

void process_input();
void update_scene();
void render_scene();

void print_gl_error(GLenum);


int main(int argc, char **argv) {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

    // Use the core OpenGL profile (as opposed to Compatibility or ES)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);


    // Request a color buffer with 8 bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Double buffer is enabled by default
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);


    // Create an SDL Window
	SDL_Window* mWindow = SDL_CreateWindow(
		"OpenGL SDL Window", // Window title
		50,	// Top left x-coordinate of window
		50,	// Top left y-coordinate of window
		600,	// Width of window
		600,	// Height of window
		SDL_WINDOW_OPENGL	// Flags (0 for no flags set)
	);

	if (!mWindow) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return 1;
	}

    // Create the OpenGL Context
    SDL_GLContext mContext = SDL_GL_CreateContext(mWindow);

    if (!mContext) {
		SDL_Log("Failed to create context: %s", SDL_GetError());
		return 1;
	}


    glError = glGetError();

    if(glError != GL_NO_ERROR) {
        print_gl_error(glError);
    }


    while(mIsRunning) {
        process_input();

        update_scene();

        render_scene();
    }


    SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();


    return 0;
}



void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // If we get an SDL_QUIT event, end loop
        case SDL_QUIT:
            mIsRunning = 0;
            break;
        }
    }
}


void update_scene() {

}


void render_scene() {
    glClearColor(0.1f, 0.1f, 1.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, 600, 600);

    glOrtho(-5.0, 5.0, -5.0, 5.0, -1.0, 1.0);


    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2d(-3.0, -3.0);
        glVertex2d(3.0, 3.0);
    glEnd();


    glFlush();


    glError = glGetError();

    if(glError != GL_NO_ERROR) {
        print_gl_error(glError);
    }
}


void print_gl_error(GLenum err) {
    switch (err) {
        case GL_NO_ERROR:
            printf("No error has been recorded. The value of this symbolic constant is guaranteed to be 0.");

            break;

        case GL_INVALID_ENUM:
            printf("An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.");

            break;

        case GL_INVALID_VALUE:
            printf("A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.");

            break;

        case GL_INVALID_OPERATION:
            printf("The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.");

            break;

        case GL_INVALID_FRAMEBUFFER_OPERATION:
            printf("The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.");

            break;

        case GL_OUT_OF_MEMORY:
            printf("There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.");

            break;

        case GL_STACK_UNDERFLOW:
            printf("An attempt has been made to perform an operation that would cause an internal stack to underflow.");

            break;

        case GL_STACK_OVERFLOW:
            printf("An attempt has been made to perform an operation that would cause an internal stack to overflow.");

            break;

        default:
            printf("No Matching GL Error Enum Found");

            break;
    }
}
