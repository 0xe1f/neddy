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

#ifndef SCREEN_H
#define SCREEN_H

#include "sprite.h"

struct Screen {
    unsigned char *buffer;
    int buffer_size;
    int width;
    int height;
    int bpp;
};

int screen_create(
    struct Screen *screen,
    int width,
    int height,
    int bpp
);
void screen_destroy(struct Screen *screen);

void screen_clear(
    const struct Screen *screen
);
void screen_draw_sprite(
    const struct Screen *screen,
    const struct Sprite *sprite
);

#endif // SCREEN_H
