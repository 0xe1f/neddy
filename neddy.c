// Copyright (c) 2024 Akop Karapetyan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "rgbserver.h"

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

#include "screen.h"
#include "sprite.h"

#include "neddy.h"
#include "hi_neddy.h"

struct Screen screen = {};

volatile int interrupt_received = 0;

#define SPRITE_NEDDY 1
#define SPRITE_HI_NEDDY 2

#define SYNC_ANIM_DELAY ((long long)((0.05)*1000000.0))

struct SpriteDef {
    unsigned char type;
    signed short x;
    signed short y;
};

static void InterruptHandler(int signo) {
    interrupt_received = 1;
    rgbs_end();
    screen_destroy(&screen);
}

long long millis() {
    static struct timeval tv;
    gettimeofday(&tv, 0);
    return (tv.tv_sec) * 1000LL + tv.tv_usec / 1000LL;
}

void sprite_init(
    struct Sprite *sprite,
    const struct SpriteDef *def
) {
    if (def->type == SPRITE_NEDDY) {
        sprite->bitmap = (unsigned char *) neddy;
        sprite->width = 320;
        sprite->height = 256;
        sprite->frame_width = 320;
        sprite->anim_dir = 1;
        sprite->frame = 0;
    } else if (def->type == SPRITE_HI_NEDDY) {
        sprite->bitmap = (unsigned char *) hi_neddy;
        sprite->width = 320;
        sprite->height = 256;
        sprite->frame_width = 320;
        sprite->anim_dir = 1;
        sprite->frame = 0;
    } else {
        return;
    }

    sprite->delta = 0;
    sprite->bpp = 2;
    sprite->x = def->x;
    sprite->y = def->y;
    sprite->type = def->type;
    sprite->format = FORMAT_RGBA5551;
}

int main(int argc, char **argv) {
    if (!screen_create(&screen, 320, 240, 2)) {
        fprintf(stderr, "screen_create() failed\n");
        return 1;
    }

    struct FrameGeometry data = {
        screen.buffer_size,
        screen.width * screen.bpp,
        screen.width,
        screen.height,
        PIXEL_FORMAT_RGBA5551,
        0,
        MAGIC_NUMBER
    };
    rgbs_set_buffer_data(data);

    if (!rgbs_start()) {
        fprintf(stderr, "Error initializing rgbs\n");
        return 1;
    }

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    struct SpriteDef defaults[] = {
        { SPRITE_NEDDY, 0, 0 },
        { SPRITE_HI_NEDDY, 0, 0 },
        { 0, 0, 0 },
    };
    int count = 0;
    const struct SpriteDef *def;
    for (def = defaults; def->type; def++, count++);

    struct Sprite *sprites = malloc(count * sizeof(struct Sprite));
    struct Sprite *sprite;
    for (sprite = sprites, def = defaults; def->type; sprite++, def++) {
        sprite_init(sprite, def);
    }

    int index = 0;
    long long start = 0;
    screen_clear(&screen);
    while (!interrupt_received) {
        long long tick = millis();
        usleep(SYNC_ANIM_DELAY);

        if (tick - start > 10000) {
            start = tick;
            if (++index > 1) {
                index = 0;
            }
            screen_clear(&screen);
        }
        screen_draw_sprite(&screen, &sprites[index]);

        rgbs_poll();
        rgbs_send(screen.buffer, screen.buffer_size);
    }

    free(sprites);
    screen_destroy(&screen);

    return 0;
}
