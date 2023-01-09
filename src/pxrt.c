
#include <stdio.h>
#include <math.h>

#include "SDL2/SDL.h"
#include "GL/gl.h"



#define NES_PIXEL_WIDTH 256
#define NES_PIXEL_HEIGHT 224

const int window_width = 512;
const int window_height = 448;

float pixel_length = 2.0f;

float zoom_delta = 0.25f;

int mouse_x, mouse_y;

float fov_nes_x = 0.0f;
float fov_nes_y = 0.0f;
float fov_nes_width = 256.0f;
float fov_nes_height = 224.0f;

int mouseIsDown = 0;

void pixel(int, int);
void grid();


SDL_Window* mWindow;


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
	mWindow = SDL_CreateWindow(
		"OpenGL SDL Window", // Window title
		50,	// Top left x-coordinate of window
		50,	// Top left y-coordinate of window
		window_width,	// Width of window
		window_height,	// Height of window
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


    glPointSize(pixel_length);


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
    while (SDL_PollEvent(&event)) {
        if(event.type == SDL_MOUSEWHEEL && event.wheel.windowID == SDL_GetWindowID(mWindow)) {
            SDL_GetMouseState(&mouse_x, &mouse_y);

            float mouse_nes_x = ((float) mouse_x)/((float) window_width) * fov_nes_width + fov_nes_x;
            float mouse_nes_y = ((float) mouse_y)/((float) window_height) * fov_nes_height + fov_nes_y;

            float old_pixel_length = pixel_length;
            float translation_delta_x, translation_delta_y;

            if(event.wheel.y < 0) { // Zoom Out
                printf("Scroll Down\n");

                if(pixel_length > ((float) window_width)/((float) NES_PIXEL_WIDTH)) {
                    pixel_length = pixel_length * (1.0f - zoom_delta);

                    translation_delta_x = (mouse_nes_x - 1) * old_pixel_length * zoom_delta / pixel_length;
                    translation_delta_y = (mouse_nes_y - 1) * old_pixel_length * zoom_delta / pixel_length;

                    fov_nes_x = fov_nes_x - translation_delta_x;
                    fov_nes_y = fov_nes_y - translation_delta_y;

                    fov_nes_width = window_width / pixel_length;
                    fov_nes_height = window_height / pixel_length;
                }
            }else if(event.wheel.y > 0) { // Zoom In
                printf("Scroll Up\n");

                if(pixel_length < window_width/8.0f) {
                    pixel_length = pixel_length * (1.0f + zoom_delta);

                    translation_delta_x = (mouse_nes_x - 1) * old_pixel_length * zoom_delta / pixel_length;
                    translation_delta_y = (mouse_nes_y - 1) * old_pixel_length * zoom_delta / pixel_length;

                    fov_nes_x = fov_nes_x + translation_delta_x;
                    fov_nes_y = fov_nes_y + translation_delta_y;

                    fov_nes_width = window_width / pixel_length;
                    fov_nes_height = window_height / pixel_length;
                }
            }

            printf("Mouse X: %d, Mouse Y: %d\n", mouse_x, mouse_y);
            printf("Pixel Length: %f\n\n", pixel_length);

            glPointSize(pixel_length);
        }else if(event.type == SDL_MOUSEBUTTONDOWN) {
            printf("Mouse Down\n");

            if(event.button.button == SDL_BUTTON_RIGHT) {
                mouseIsDown = 1;
                printf("Right Button\n");
            }

            printf("Mouse X: %d, Mouse Y: %d\n", event.button.x, event.button.y);
        }else if(event.type == SDL_MOUSEBUTTONUP) {
            printf("Mouse Up\n");

            mouseIsDown = 0;

            printf("Mouse X: %d, Mouse Y: %d\n", event.button.x, event.button.y);
            printf("Window ID: %u\n\n", event.button.windowID);
        }else if(event.type == SDL_QUIT) {
            mIsRunning = 0;
        }

        if(event.type == SDL_MOUSEMOTION && mouseIsDown) {
            printf("Mouse Motion, Motion X: %d, Motion Y: %d\n\n", event.motion.xrel, event.motion.yrel);

            if(mouseIsDown) {
                fov_nes_x -= event.motion.xrel/pixel_length;
                fov_nes_y -= event.motion.yrel/pixel_length;
            }
        }
    }
}


void update_scene() {

}


void render_scene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, window_width, window_height);

    // Note I flip the Y Axis
    glOrtho(0.0, window_width, window_height, 0.0, -1.0, 1.0);


    pixel(0, 0);
    pixel(2, 0);

    pixel(2, 10);

    pixel(5, 5);

    pixel(128, 120);
    pixel(170, 170);


    grid();


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



void pixel(int x, int y) {
    if(x >= fov_nes_x && x < fov_nes_x + fov_nes_width && y >= fov_nes_y && y < fov_nes_y + fov_nes_height) {
        float screen_x = (x - fov_nes_x)/fov_nes_width * window_width + pixel_length/2.0f;
        float screen_y = (y - fov_nes_y)/fov_nes_height * window_height + pixel_length/2.0f;

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
            glVertex2f(screen_x, screen_y);
        glEnd();
    }
}


void grid() {
    for(int i = 0; i < floorf(fov_nes_width); i++) {
        float screen_x = (ceilf(fov_nes_x) + i - fov_nes_x)*pixel_length;
        float screen_y = (ceilf(fov_nes_y) + i - fov_nes_y)*pixel_length;

        glLineWidth(0.5f);

        glBegin(GL_LINES);
            glVertex2f(screen_x, 0.0f);
            glVertex2f(screen_x, window_height);
        glEnd();

        if(i < floorf(fov_nes_height)) {
            glBegin(GL_LINES);
                glVertex2f(0.0f, screen_y);
                glVertex2f(window_width, screen_y);
            glEnd();
        }
    }
}
