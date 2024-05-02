#include "pal.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

const screen_dim_t PAL_SCREEN_WIDTH = 128;
const screen_dim_t PAL_SCREEN_HEIGHT = 128;
uint32_t PAL_AUDIO_SAMPLE_RATE = 44100;
const uint32_t PAL_RAND_MAX = RAND_MAX;

static SDL_Surface *surface;
static SDL_Window *window;
static SDL_AudioDeviceID audio_dev_id;

static pal_audio_callback_t pal_audio_callback;

static uint32_t prev_time;
static screen_dim_t prev_pointer_x;
static screen_dim_t prev_pointer_y;
static bool prev_pointer_pos_valid = false;

static SDL_Keycode keycode_mapping[NUM_BUTTONS] = {
    [BUTTON_LEFT] = SDLK_a,
    [BUTTON_RIGHT] = SDLK_d,
    [BUTTON_LEFT_TRIGGER] = SDLK_q,
    [BUTTON_RIGHT_TRIGGER] = SDLK_e,
};

static bool key_press_map[NUM_BUTTONS] = {
    [BUTTON_LEFT] = false,
    [BUTTON_RIGHT] = false,
    [BUTTON_LEFT_TRIGGER] = false,
    [BUTTON_RIGHT_TRIGGER] = false,
};

void pal_screen_clear(struct color c) {
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, c.r, c.g, c.b));
}

void pal_screen_render() {
    SDL_UpdateWindowSurface(window);
}

void pal_screen_draw_pixel(int x, int y, struct color c) {
    if (window == NULL) return;
    if (x >= surface->w || y >= surface->h || x < 0 || y < 0) return;
    if (c.a == 0) return;

    ((uint32_t *) surface->pixels)[y * surface->w + x] = SDL_MapRGB(surface->format, c.r, c.g, c.b);
}

double pal_get_time() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec + t.tv_nsec / 1.0e9;
}

static void pal_sdl_audio_callback(void *userdata, Uint8 *stream, int len) {
    (void) userdata;

    // audio format is fixed, convert Uint8 *stream and length
    len = len / sizeof(audio_sample_t);

    SDL_LockAudioDevice(audio_dev_id);
    if (pal_audio_callback != NULL) {
        pal_audio_callback((audio_sample_t *) stream, len);
    }
    SDL_UnlockAudioDevice(audio_dev_id);
}

void pal_set_audio_callback(pal_audio_callback_t audio_callback) {
    pal_audio_callback = audio_callback;
}

bool pal_poll_event(struct pal_event *event) {
    double current_time = pal_get_time();
    SDL_Event e;

    if (SDL_PollEvent(&e) == 0)
        return false;

    // event needs to be processed
    if (e.type == SDL_MOUSEMOTION && e.motion.state == SDL_PRESSED) {
        // set event
        event->type = PAL_EVENT_TYPE_POINTER;
        event->pointer.state = POINTER_STATE_DOWN;
        event->pointer.position.x = e.motion.x;
        event->pointer.position.y = e.motion.y;

        if (prev_pointer_pos_valid) {
            event->pointer.velocity.x = (e.motion.x - prev_pointer_x) / (current_time - prev_time);
            event->pointer.velocity.y = (e.motion.y - prev_pointer_y) / (current_time - prev_time);
        } else {
            event->pointer.velocity.x = 0.0;
            event->pointer.velocity.y = 0.0;
        }

        prev_pointer_x = e.motion.x;
        prev_pointer_y = e.motion.y;
        prev_time = current_time;
        prev_pointer_pos_valid = true;
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        // set event
        event->type = PAL_EVENT_TYPE_POINTER;
        event->pointer.state = POINTER_STATE_UP;

        prev_pointer_pos_valid = false;
    } else if (e.type == SDL_KEYDOWN) {
        // set event if it's one of the keys in the mapping
        for (int i = 0; i < NUM_BUTTONS; i++) {
            if (keycode_mapping[i] == e.key.keysym.sym) {
                // key has been reported
                if (key_press_map[i])
                    return false;
                event->type = PAL_EVENT_TYPE_BUTTON;
                event->button.which = (enum button) i;
                event->button.state = BUTTON_STATE_DOWN;
                key_press_map[i] = true;
                break;
            }
        }
    } else if (e.type == SDL_KEYUP) {
        // set event if it's one of the keys in the mapping
        for (int i = 0; i < NUM_BUTTONS; i++) {
            if (keycode_mapping[i] == e.key.keysym.sym) {
                if (!key_press_map[i])
                    return false;
                event->type = PAL_EVENT_TYPE_BUTTON;
                event->button.which = (enum button) i;
                event->button.state = BUTTON_STATE_UP;
                key_press_map[i] = false;
                break;
            }
        }
    }

    return true;
}

float pal_sin(float a) {
    return sinf(a);
}

float pal_cos(float a) {
    return cosf(a);
}

uint32_t pal_rand() {
    return rand();
}

bool pal_init() {
    srand(time(NULL));

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        return false;

    window = SDL_CreateWindow("Palygon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, PAL_SCREEN_WIDTH, PAL_SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
        return false;

    surface = SDL_GetWindowSurface(window);

    // clear screen
    pal_screen_clear((struct color) { 0, 0, 0 });
    pal_screen_render();

    const SDL_AudioSpec desired_spec = {
        .freq = PAL_AUDIO_SAMPLE_RATE,
        .format = AUDIO_U16SYS, // NOTE: change this field if pal audio_sample_t changes
        .samples = 1024,
        .channels = 1,
        .callback = pal_sdl_audio_callback
    };
    SDL_AudioSpec obtained_spec;

    // init audio
    audio_dev_id = SDL_OpenAudioDevice(NULL, 0, &desired_spec, &obtained_spec, 0);
    if (audio_dev_id == 0)
        return false;

    PAL_AUDIO_SAMPLE_RATE = obtained_spec.freq;

    // allow audio playback
    SDL_PauseAudioDevice(audio_dev_id, false);

    return true;
}