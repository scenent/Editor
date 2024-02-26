#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>

/* Preferences */
#define TARGET_LANG_TYPE_CPP 0
#define TARGET_LANG_TYPE_PYTHON 1
#define TARGET_LANG_TYPE_JAVASCRIPT 2

#define TARGET_LANG                           TARGET_LANG_TYPE_CPP
#define FONT_SIZE                             48
#define FONT_PATH                             "res/monaspace_neon.otf"
#define TAB_SIZE                              4
// #define BACKGROUND_TEXTURE_PATH               "res/my_background.png" // YOU CAN ACTIVATE THIS LINE
#define BACKGROUND_TEXTURE_MODULATE_RGB       0.25f


/* Macros */

#define PUSH_ERROR(msg) fprintf(stderr, "Error: %s, in file %s, at line %d\n", msg, __FILE__, __LINE__)
#define GLYPH_VERTEX_SHADER_PATH "res/glyph_vert.glsl"
#define GLYPH_FRAGMENT_SHADER_PATH "res/glyph_frag.glsl"
#define COLOR_RECT_VERTEX_SHADER_PATH "res/color_rect_vert.glsl"
#define COLOR_RECT_FRAGMENT_SHADER_PATH "res/color_rect_frag.glsl"
#define SPRITE_VERTEX_SHADER_PATH "res/sprite_vert.glsl"
#define SPRITE_FRAGMENT_SHADER_PATH "res/sprite_frag.glsl"

#endif