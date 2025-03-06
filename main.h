#pragma once
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <gtemac.h>
#include <libapi.h>
#include <inline_n.h>
#include <libcd.h>
#include <stdint.h>
#include <string.h>
#include <r3000.h>
#include <libmath.h>

#define VMODE 0                 // Video Mode : 0 : NTSC, 1: PAL
#define SCREENXRES 640          // Screen width
#define SCREENYRES 480          // Screen height
#define CENTERX SCREENXRES/2    // Center of screen on x 
#define CENTERY SCREENYRES/2    // Center of screen on y
#define MARGINX 0                // margins for text display
#define MARGINY 32
#define FONTSIZE 8 * 7           // Text Field Height
#define OTLEN       256        // Maximum number of OT entrie
#define PRIMBUFFLEN 131072       // Maximum number of POLY_GT3 primitives
DISPENV disp[2];                 // Double buffered DISPENV and DRAWENV
DRAWENV draw[2];
u_long      ot[2][OTLEN];                   // Ordering table (contains addresses to primitives)
char        primbuff[2][PRIMBUFFLEN]; // Primitive list // That's our prim buffer
char * nextpri = primbuff[0];                       // Primitive counter
short db = 0;                      // index of which buffer is used, values 0, 1

typedef struct {
    int vertex_index;
    int weight;
} __attribute__((aligned(4))) BoneVertex;  // Explicitly align BoneVertex

typedef struct {
    SVECTOR x, y, z; // Each component is also an SVECTOR
    u_char r0, g0, b0;    /* Color of vertex 0 */
    u_char r1, g1, b1;    /* Color of vertex 1 */
    u_char r2, g2, b2;    /* Color of vertex 2 */
    u_short tpage, clut;
    u_char u0, v0, u1, v1, u2, v2;        /* texture corner point */
} __attribute__((aligned(4))) SVECTOR_3D;

typedef struct {
    SVECTOR_3D mesho[256];
    char bone_name[32];
    BoneVertex vertices[256];
    short vertex_count;
    SVECTOR centro;
    SVECTOR rotacao;
    SVECTOR vertex_center;
} __attribute__((aligned(4))) Bone;  // Explicitly align Bone


typedef struct Cube {
    short len;
    short n_prim;
    POLY_GT3* polygon;
    SVECTOR Rotate;
    VECTOR Trans;
    VECTOR Scale;
    MATRIX Matrix;
    TIM_IMAGE tim;
    Bone bones[20];
    short boneCount;
    SVECTOR_3D *mesh;  // Use pointer for mesh instead of flexible array

} __attribute__((aligned(4))) Cube;  // Explicitly align Cube


struct Objects {
    struct Cube cubes[1];
} __attribute__((aligned(8)));