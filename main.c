#include "main.h"
#include "piton/parser.c"
extern unsigned char _binary_realcubo_tmd_start[]; // filename must begin with _binary_ followed by the full path, with . and / replaced, and then suffixed with _ and end with _start[]; or end[];
extern unsigned char _binary_realcubo_tmd_end[];

extern unsigned char _binary_w_alien_tmd_start[]; // filename must begin with _binary_ followed by the full path, with . and / replaced, and then suffixed with _ and end with _start[]; or end[];
extern unsigned char _binary_w_alien_tmd_end[];

extern unsigned char _binary_cubo_tmd_start[]; // filename must begin with _binary_ followed by the full path, with . and / replaced, and then suffixed with _ and end with _start[]; or end[];
extern unsigned char _binary_cubo_tmd_end[];

extern unsigned long _binary_cubotex_tim_start[]; // filename must begin with _binary_ followed by the full path, with . and / replaced, and then suffixed with _ and end with _start[]; or end[];
extern unsigned long _binary_cubotex_tim_end[];
extern unsigned long _binary_cubotex_tim_length;

extern unsigned long _binary_cubetex_tim_start[]; // filename must begin with _binary_ followed by the full path, with . and / replaced, and then suffixed with _ and end with _start[]; or end[];
extern unsigned long _binary_cubetex_tim_end[];
extern unsigned long _binary_cubetex_tim_length;

extern unsigned long _binary_w_alientex_tim_start[]; // filename must begin with _binary_ followed by the full path, with . and / replaced, and then suffixed with _ and end with _start[]; or end[];
extern unsigned long _binary_w_alientex_tim_end[];
extern unsigned long _binary_w_alientex_tim_length;

extern unsigned long _binary_w_alientexx_tim_start[]; // filename must begin with _binary_ followed by the full path, with . and / replaced, and then suffixed with _ and end with _start[]; or end[];
extern unsigned long _binary_w_alientexx_tim_end[];
extern unsigned long _binary_w_alientexx_tim_length;

extern unsigned long _binary_og_pipe02_tim_start[]; // filename must begin with _binary_ followed by the full path, with . and / replaced, and then suffixed with _ and end with _start[]; or end[];
extern unsigned long _binary_og_pipe02_tim_end[];
extern unsigned long _binary_og_pipe02_tim_length;

extern unsigned long _binary_123_tim_start[]; // filename must begin with _binary_ followed by the full path, with . and / replaced, and then suffixed with _ and end with _start[]; or end[];
extern unsigned long _binary_123_tim_end[];
extern unsigned long _binary_123_tim_length;

void init(void)
{
    PadInit(0);
    ResetGraph(0);                 // Initialize drawing engine with a complete reset (0)
    InitGeom();
    SetGeomOffset(CENTERX, CENTERY);

    SetGeomScreen(CENTERX);
    SetDefDispEnv(&disp[0], 0, 0         , SCREENXRES, SCREENYRES);     // Set display area for both &disp[0] and &disp[1]
    SetDefDispEnv(&disp[1], 0, SCREENYRES, SCREENXRES, SCREENYRES);     // &disp[0] is on top  of &disp[1]
    SetDefDrawEnv(&draw[0], 0, SCREENYRES, SCREENXRES, SCREENYRES);     // Set draw for both &draw[0] and &draw[1]
    SetDefDrawEnv(&draw[1], 0, 0         , SCREENXRES, SCREENYRES);     // &draw[0] is below &draw[1]
    if (VMODE)                  // PAL
    {
        SetVideoMode(MODE_PAL);
        disp[0].screen.y += 8;  // add offset : 240 + 8 + 8 = 256
        disp[1].screen.y += 8;
    }
    SetDispMask(1);                 // Display on screen    
    setRGB0(&draw[0], 0, 0, 255); // set color for first draw area
    setRGB0(&draw[1], 0, 0, 255); // set color for second draw area
    draw[0].isbg = 1;               // set mask for draw areas. 1 means repainting the area with the RGB color each frame 
    draw[1].isbg = 1;
    PutDispEnv(&disp[db]);          // set the disp and draw environnments
    PutDrawEnv(&draw[db]);
    FntLoad(960, 0);                // Load font to vram at 960,0(+128)
    FntOpen(MARGINX, SCREENYRES - MARGINY - FONTSIZE, SCREENXRES - MARGINX * 2, FONTSIZE, 0, 280 ); // FntOpen(x, y, width, height,  black_bg, max. nbr. chars
    if (!CdInit()) {
        printf("Failed to initialize CD system.\n");
    }
}
void display(void)
{
    DrawSync(0);                    // Wait for all drawing to terminate
    VSync(0);                       // Wait for the next vertical blank
    PutDispEnv(&disp[db]);          // set alternate disp and draw environnments
    PutDrawEnv(&draw[db]);  
    DrawOTag(&ot[db][OTLEN-1]);
    db = !db;                       // flip db value (0 or 1)
    db=1;
    nextpri = primbuff[db];

}
void LoadTexture(u_long * tim, TIM_IMAGE * tparam){     // This part is from Lameguy64's tutorial series : lameguy64.net/svn/pstutorials/chapter1/3-textures.html login/pw: annoyingmous
        OpenTIM(tim);                                   // Open the tim binary data, feed it the address of the data in memory
        ReadTIM(tparam);                                // This read the header of the TIM data and sets the corresponding members of the TIM_IMAGE structure
        
        LoadImage(tparam->prect, tparam->paddr);        // Transfer the data from memory to VRAM at position prect.x, prect.y
        DrawSync(0);                                    // Wait for the drawing to end
        
        if (tparam->mode & 0x8){ // check 4th bit       // If 4th bit == 1, TIM has a CLUT
            LoadImage(tparam->crect, tparam->caddr);    // Load it to VRAM at position crect.x, crect.y
            DrawSync(0);                                // Wait for drawing to end
    }

}
#define FIXED_SHIFT 12
#define FIXED(x) ((short)((x) * (1 << FIXED_SHIFT))) // Convert float to fixed-point
#define FIXED_MUL(a, b) (((a) * (b)) >> FIXED_SHIFT) // Fixed-point multiplication

// Precompute fixed-point trigonometric values for 45 degrees
const short cosTheta = FIXED(0.707);
const short sinTheta = FIXED(0.707);
static unsigned long heap[256 * 1024];

short sineTable[360] = {
    4096, 71, 142, 213, 284, 355, 426, 496, 567, 637,
    707, 777, 847, 917, 986, 1056, 1125, 1194, 1262, 1331,
    1399, 1467, 1535, 1602, 1669, 1736, 1803, 1869, 1935, 2000,
    2065, 2130, 2194, 2258, 2321, 2384, 2446, 2508, 2569, 2630,
    2690, 2750, 2809, 2867, 2925, 2982, 3039, 3095, 3150, 3205,
    3259, 3312, 3365, 3417, 3468, 3519, 3569, 3618, 3666, 3714,
    3761, 3807, 3852, 3896, 3940, 3982, 4024, 4065, 4095, 4096,
    4095, 4065, 4024, 3982, 3940, 3896, 3852, 3807, 3761, 3714,
    3666, 3618, 3569, 3519, 3468, 3417, 3365, 3312, 3259, 3205,
    3150, 3095, 3039, 2982, 2925, 2867, 2809, 2750, 2690, 2630,
    2569, 2508, 2446, 2384, 2321, 2258, 2194, 2130, 2065, 2000,
    1935, 1869, 1803, 1736, 1669, 1602, 1535, 1467, 1399, 1331,
    1262, 1194, 1125, 1056, 986, 917, 847, 777, 707, 637,
    567, 496, 426, 355, 284, 213, 142, 71, 4096, -71,
    -142, -213, -284, -355, -426, -496, -567, -637, -707, -777,
    -847, -917, -986, -1056, -1125, -1194, -1262, -1331, -1399, -1467,
    -1535, -1602, -1669, -1736, -1803, -1869, -1935, -2000, -2065, -2130,
    -2194, -2258, -2321, -2384, -2446, -2508, -2569, -2630, -2690, -2750,
    -2809, -2867, -2925, -2982, -3039, -3095, -3150, -3205, -3259, -3312,
    -3365, -3417, -3468, -3519, -3569, -3618, -3666, -3714, -3761, -3807,
    -3852, -3896, -3940, -3982, -4024, -4065, -4095, -4096, -4095, -4065,
    -4024, -3982, -3940, -3896, -3852, -3807, -3761, -3714, -3666, -3618,
    -3569, -3519, -3468, -3417, -3365, -3312, -3259, -3205, -3150, -3095,
    -3039, -2982, -2925, -2867, -2809, -2750, -2690, -2630, -2569, -2508,
    -2446, -2384, -2321, -2258, -2194, -2130, -2065, -2000, -1935, -1869,
    -1803, -1736, -1669, -1602, -1535, -1467, -1399, -1331, -1262, -1194,
    -1125, -1056, -986, -917, -847, -777, -707, -637, -567, -496,
    -426, -355, -284, -213, -142, -71, 4096
};

// Get sine from lookup table
short fixed_sin(int angle) {
    angle = angle % 360;  // Ensure wrap-around at 360 degrees
    if (angle < 0) angle += 360;  // Handle negative angles
    return sineTable[angle];
}

short fixed_cos(int angle) {
    angle = (angle + 90) % 360;  // Shift angle by 90° to get cosine
    if (angle < 0) angle += 360;  // Handle negative angles
    return sineTable[angle];
}



int main(void){
     InitHeap(heap, sizeof(heap));
    init();

    struct Cube myCube2 = {};
    //loadFile("\\MODEL.BIN;1");
    //loadMDL("\\CUBINHO.OBJ;1", &myCube2);
    //loadMDL("\\CUBAS.OBJ;1", &myCube2);
    //loadMDL("\\TRIGIX.OBJ;1", &myCube2);
    loadMDL("\\CUBIN.OBJ;1", &myCube2, "\\CUBIN.JSON;1");
    //PrintFacesWithVertices(&myCube2);

    struct Objects object;
    LoadTexture(_binary_w_alientex_tim_start, &myCube2.tim);

    object.cubes[0] = myCube2;

    /*
    if(1 == 2)
        for (int i = 0; i < object.cubes[1].n_prim; i++) {
            ReadTMD(&object.cubes[1].tmd);
            object.cubes[1].mesh[i].x = object.cubes[1].tmd.x0;
            object.cubes[1].mesh[i].y = object.cubes[1].tmd.x1;
            object.cubes[1].mesh[i].z = object.cubes[1].tmd.x2;

            object.cubes[1].mesh[i].r0 = object.cubes[1].tmd.r0;
            object.cubes[1].mesh[i].r1 = object.cubes[1].tmd.r1;
            object.cubes[1].mesh[i].r2 = object.cubes[1].tmd.r2;

            object.cubes[1].mesh[i].g0 = object.cubes[1].tmd.g0;
            object.cubes[1].mesh[i].g1 = object.cubes[1].tmd.g1;
            object.cubes[1].mesh[i].g2 = object.cubes[1].tmd.g2;

            object.cubes[1].mesh[i].b0 = object.cubes[1].tmd.b0;
            object.cubes[1].mesh[i].b1 = object.cubes[1].tmd.b1;
            object.cubes[1].mesh[i].b2 = object.cubes[1].tmd.b2;

            object.cubes[1].mesh[i].u0 = object.cubes[1].tmd.u0;
            object.cubes[1].mesh[i].u1 = object.cubes[1].tmd.u1;
            object.cubes[1].mesh[i].u2 = object.cubes[1].tmd.u2;

            object.cubes[1].mesh[i].v0 = object.cubes[1].tmd.v0;
            object.cubes[1].mesh[i].v1 = object.cubes[1].tmd.v1;
            object.cubes[1].mesh[i].v2 = object.cubes[1].tmd.v2;

        }
    */
    long OTz, Flag, p, t;
    object.cubes[0].Trans.vy = -50;
    object.cubes[0].Trans.vz = 100;
    object.cubes[0].Rotate.vx = 1000;
    object.cubes[0].Rotate.vy = 0;
    long size = 7000; 
    object.cubes[0].Scale.vx = size;
    object.cubes[0].Scale.vy = size;
    object.cubes[0].Scale.vz = size;
    short angle = 0;
    while (1)
    {
        ClearOTagR(ot[db], OTLEN);
        //object.cubes[0].Rotate.vy += 10;  ; // Pan
        //object.cubes[0].Rotate.vx += 10;  ; // Pan
        //object.cubes[0].Rotate.vz += 10;  ; // Pan
        RotMatrix(&object.cubes[0].Rotate, &object.cubes[0].Matrix);
        TransMatrix(&object.cubes[0].Matrix, &object.cubes[0].Trans);
        ScaleMatrix(&object.cubes[0].Matrix, &object.cubes[0].Scale);
        SetRotMatrix(&object.cubes[0].Matrix);
        SetTransMatrix(&object.cubes[0].Matrix);
        //printf("hallo");
        //printf("index: %d", object.cubes[0].boneCount);
        /*
        for(int i = 0; i < object.cubes[0].boneCount; i++){
            Bone bone = object.cubes[0].bones[i];
            for(int j = 0; j < bone.vertex_count; j++){
                printf("%d ", bone.vertices[j].vertex_index);
            }
            break;
        }
        */
        for (int i = 0; i < 1; i++) {
            //i = 2;
            Bone bone = object.cubes[0].bones[i];
            angle++;



            for (int j = 0; j < bone.vertex_count; j++) {
                if (angle == 360) {
                    angle = 0;
                }

                short eulerX, eulerY, eulerZ, sinX, cosX, sinY, cosY, sinZ, cosZ, transX, transY, transZ;
        
                eulerX = (4096 * angle) / 360;
                //eulerY = (4096 * bone.rotacao.vy) / 360;
                //eulerZ = (4096 * bone.rotacao.vz) / 360;
                
                sinX = csin(eulerX);
                cosX = ccos(eulerX);

                sinY = fixed_sin(eulerY);
                cosY = fixed_cos(eulerY);

                sinZ = fixed_sin(eulerZ);
                cosZ = fixed_cos(eulerZ);
                
                short rotationMatrixX[3][3] = {
                    {4096, 0, 0},
                    {0, cosX, -sinX},  // 4096 representa 1.0
                    {0, sinX, cosX}
                };

                short rotationMatrixaa[3][3] = {
                    { cosY,    0,  sinY  },
                    {    0,  4096,    0  },
                    { -sinY,   0,  cosY  }
                };
                

                short rotationMatrixB[3][3] = {
                    { cosZ, -sinZ,    0  },
                    { sinZ,  cosZ,    0  },
                    {    0,     0,  4096 }
                };
                

                
                // Obtendo o vértice original
                SVECTOR unRotatedVertex = vertices2[bone.vertices[j].vertex_index];
                SVECTOR rotatedVertex = vertices2[bone.vertices[j].vertex_index];
                
                
                unRotatedVertex.vx -= bone.vertex_center.vx*16;
                unRotatedVertex.vy -= bone.vertex_center.vy*16;
                unRotatedVertex.vz -= bone.vertex_center.vz*16;
                
                rotatedVertex.vx = ((rotationMatrixX[0][0] * unRotatedVertex.vx +
                                        rotationMatrixX[0][1] * unRotatedVertex.vy +
                                        rotationMatrixX[0][2] * unRotatedVertex.vz) / 4096);
                                        

                rotatedVertex.vy = ((rotationMatrixX[1][0] * unRotatedVertex.vx +
                                        rotationMatrixX[1][1] * unRotatedVertex.vy +
                                        rotationMatrixX[1][2] * unRotatedVertex.vz) / 4096);
                                        

                rotatedVertex.vz = ((rotationMatrixX[2][0] * unRotatedVertex.vx +
                                        rotationMatrixX[2][1] * unRotatedVertex.vy +
                                        rotationMatrixX[2][2] * unRotatedVertex.vz) / 4096);
                

                rotatedVertex.vx += bone.vertex_center.vx*16;
                rotatedVertex.vy += bone.vertex_center.vy*16;
                rotatedVertex.vz += bone.vertex_center.vz*16;
                
                //rotatedVertex.vx += center.vx;
                //rotatedVertex.vy += center.vy;
                //rotatedVertex.vz += center.vz;

                //SVECTOR center = {0, 48, -13, 0};

                /*
                HALF WORKS
                rotatedVertex.vx += bone.centro.vz*16; 
                rotatedVertex.vy += bone.centro.vx*16;
                rotatedVertex.vz += -bone.centro.vy*16;
                */
               //rotatedVertex.vy += -bone.centro.vz*16; 
               //rotatedVertex.vx += bone.centro.vx*16;
               //rotatedVertex.vz += -bone.centro.vy*16;

                
                //rotatedVertex.vx += bone.centro.vx*16; 
                //rotatedVertex.vy += bone.centro.vy*16;
                //rotatedVertex.vz += bone.centro.vz*16;

                //printf("%d\n", bone.vertices[j].vertex_index);
                // Armazenando o vértice rotacionado


                vertices[bone.vertices[j].vertex_index] = rotatedVertex;

            }
            
            
        }
    
    
            
        for (int i = 0; i < faceCount; i++) {
            object.cubes[0].polygon = (POLY_GT3 *)nextpri;
            SetPolyGT3(object.cubes[0].polygon);
            /*
            object.cubes[0].polygon->tpage = getTPage(object.cubes[0].tim.mode&0x3, 0,
                                                object.cubes[0].tim.prect->x,
                                                object.cubes[0].tim.prect->y
                                                );
            
                                                
            if((object.cubes[0].tim.mode & 0x3) < 2){
                setClut( object.cubes[0].polygon,          
                        object.cubes[0].tim.crect->x,
                        object.cubes[0].tim.crect->y
                );
            }   
            */
            setRGB0(object.cubes[0].polygon, 0,255,0);
            setRGB1(object.cubes[0].polygon, 0,0,255);
            setRGB2(object.cubes[0].polygon, 255,0,0);
            //setRGB1(object.cubes[0].polygon, 120,0,0);
            //setRGB2(object.cubes[0].polygon, 0,120,0);
            //setRGB0(object.cubes[0].polygon, object.cubes[0].mesh[i].r0, object.cubes[0].mesh[i].g0, object.cubes[0].mesh[i].b0);
            //setRGB1(object.cubes[0].polygon, object.cubes[0].mesh[i].r1, object.cubes[0].mesh[i].g1, object.cubes[0].mesh[i].b1);
            //setRGB2(object.cubes[0].polygon, object.cubes[0].mesh[i].r2, object.cubes[0].mesh[i].g2, object.cubes[0].mesh[i].b2);
            /*
            setUV3(object.cubes[0].polygon,
                object.cubes[0].mesh[i].u0, object.cubes[0].mesh[i].v0,
                object.cubes[0].mesh[i].u1, object.cubes[0].mesh[i].v1,
                object.cubes[0].mesh[i].u2, object.cubes[0].mesh[i].v2);
            */
            Face *f = &faces[i];
            SVECTOR *v1 = &vertices[f->v1-1];
            SVECTOR *v2 = &vertices[f->v2-1];
            SVECTOR *v3 = &vertices[f->v3-1];

            OTz  = RotTransPers(v1, (long*)&object.cubes[0].polygon->x0, &p, &Flag);
            OTz += RotTransPers(v2, (long*)&object.cubes[0].polygon->x1, &p, &Flag);
            OTz += RotTransPers(v3, (long*)&object.cubes[0].polygon->x2, &p, &Flag);
            OTz /= 3;
            //if ((OTz > 0) && (OTz < OTLEN))
            AddPrim(&ot[db][OTz], object.cubes[0].polygon);
            nextpri += sizeof(POLY_GT3);
        }
        FntPrint("OLA MUNDO!\n");
        FntFlush(-1);
        display();
    
    }


    return 0;
}