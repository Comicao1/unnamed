/*

#include "main.h"

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

void LoadTexture(u_long * tim, TIM_IMAGE * tparam);

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
}
void display(void)
{
    DrawSync(0);                    // Wait for all drawing to terminate
    VSync(0);                       // Wait for the next vertical blank
    PutDispEnv(&disp[db]);          // set alternate disp and draw environnments
    PutDrawEnv(&draw[db]);  
    DrawOTag(&ot[db][OTLEN-1]);
    db = !db;                       // flip db value (0 or 1)
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

void RenderTIM(TIM_IMAGE *tim, int x, int y) {
    POLY_FT4 *sprite;
    RECT tws = {0, 0, tim->prect->w, tim->prect->h}; // Texture window dimensions
    
    sprite = (POLY_FT4 *)nextpri; // Allocate a primitive

    // Initialize the sprite
    SetPolyFT4(sprite);
    setXYWH(sprite, x, y, tim->prect->w, tim->prect->h);
    setRGB0(sprite, 128, 128, 128); // Set a neutral color for the sprite

    // Set texture page and UV coordinates
    sprite->tpage = getTPage(tim->mode & 0x3, 0, tim->prect->x, tim->prect->y);
    setUV4(sprite,
           tim->prect->x, tim->prect->y,
           tim->prect->x + tim->prect->w, tim->prect->y,
           tim->prect->x, tim->prect->y + tim->prect->h,
           tim->prect->x + tim->prect->w, tim->prect->y + tim->prect->h);

    // Add the sprite to the ordering table
    AddPrim(&ot[db][OTLEN - 1], sprite);
    nextpri += sizeof(POLY_FT4);
}


typedef struct {
    SVECTOR x, y, z; // Each component is also an SVECTOR
} SVECTOR_3D;


struct Cube{
    POLY_GT3 *polygon;
    SVECTOR Rotate;
    VECTOR  Trans;
    VECTOR  Scale;
    MATRIX  Matrix;
    TMD_PRIM tmdprim;
    TIM_IMAGE tim_cube;
    int n_prim;
    SVECTOR_3D mesh[];
};

struct Objects{
    struct Cube cubes[1024];
};

int main(void){
    RECT tws = {0, 0, 256, 256};            // Texture window coordinates : x, y, w, h
    init();
    //LoadTexture(_binary_w_alientexx_tim_start, &tim_cube);
    struct Objects object;
    for(int i = 0; i<1; i++){
        struct Cube myCube = {{0}, { 0, 232, 0, 0 }, { 0, 0, CENTERX * 3, 0 }, { ONE/2, ONE/2, ONE/2, 0 }, {0}};
        object.cubes[i] = myCube;
        LoadTexture(_binary_w_alientexx_tim_start, &object.cubes[i].tim_cube);
        object.cubes[i].n_prim =  OpenTMD((u_long*)_binary_w_alien_tmd_start, 0);
        object.cubes[i].mesh[object.cubes[i].n_prim*3];
        printf("NPRIM: %d\n", object.cubes[i].n_prim);
        for (int j = 0; j < (object.cubes[i].n_prim)*3; j+=3) {
            ReadTMD(&object.cubes[i].tmdprim);
            object.cubes[i].mesh[j].x = object.cubes[i].tmdprim.x0;//(SVECTOR){1,2,3,0};
            object.cubes[i].mesh[j].y = object.cubes[i].tmdprim.x1;
            object.cubes[i].mesh[j].z = object.cubes[i].tmdprim.x2;
            //object.cubes[i].mesh[j].vx = object.cubes[i].tmdprim.x0;
            //object.cubes[i].mesh[j].vy = object.cubes[i].tmdprim.x1;
            //object.cubes[i].mesh[j].vz = object.cubes[i].tmdprim.x2;
        }
    }
    int aka = 0;                       
    long OTz, Flag, p, t;
    while(1){
        ClearOTagR(ot[db], OTLEN);
        for(int i = 0; i < 1; i++){
            RotMatrix(&object.cubes[i].Rotate, &object.cubes[i].Matrix);
            TransMatrix(&object.cubes[i].Matrix, &object.cubes[i].Trans);
            ScaleMatrix(&object.cubes[i].Matrix, &object.cubes[i].Scale);
            SetRotMatrix(&object.cubes[i].Matrix);
            SetTransMatrix(&object.cubes[i].Matrix);
            t=0;
            if(i == 1){
                object.cubes[i].Rotate.vy += 28; // Pan
                object.cubes[i].Rotate.vx += 28; // Tilt
            }
            else{
                object.cubes[i].Rotate.vy += 28; // Pan
                object.cubes[i].Rotate.vx += 0; // Tilt
                object.cubes[i].Trans.vx = -100;
                object.cubes[i].Trans.vy = 200;
                long escala = 12000;
                object.cubes[i].Scale.vx = escala;
                object.cubes[i].Scale.vy = escala;
                object.cubes[i].Scale.vz = escala;
            }
            if(aka==1){
                printf("---------------- CODE START ----------------\n");
            }
            for (i = 0; i < (object.cubes[i].n_prim*3); i += 3) {               
                object.cubes[i].polygon = (POLY_GT3 *)nextpri;
                SetPolyGT3(object.cubes[i].polygon);
                OTz  = RotTransPers(&object.cubes[1].mesh[i].x  , (long*)&object.cubes[i].polygon->x0, &p, &Flag);
                OTz += RotTransPers(&object.cubes[1].mesh[i].y, (long*)&object.cubes[i].polygon->x1, &p, &Flag);
                OTz += RotTransPers(&object.cubes[1].mesh[i].z, (long*)&object.cubes[i].polygon->x2, &p, &Flag);
                OTz /= 3;
                if ((OTz > 0) && (OTz < OTLEN))
                    AddPrim(&ot[db][OTz-2], object.cubes[i].polygon);
                nextpri += sizeof(POLY_G3);
                t+=3;
            }
            /*
            for (i = 0; i < n_prim ; i++) {
                ReadTMD(&tmdprim);
                poly = (POLY_GT3 *)nextpri;
                // Initialize the primitive and set its color values
                SetPolyGT3(poly);
                poly->tpage = getTPage(tim_cube.mode&0x3, 1,
                                                    tim_cube.prect->x,
                                                    tim_cube.prect->y
                                                    );
                if((tim_cube.mode & 0x3) < 2){
                    setClut( poly,          
                            tim_cube.crect->x,
                            tim_cube.crect->y
                    );
                }
                setRGB0(poly, tmdprim.r0, tmdprim.g0, tmdprim.b0);
                setRGB1(poly, tmdprim.r1, tmdprim.g1, tmdprim.b1);
                setRGB2(poly, tmdprim.r2, tmdprim.g2, tmdprim.b2);
                setSemiTrans(poly, 0);
                
                setUV3(poly,
                    tmdprim.u0, tmdprim.v0,
                    tmdprim.u1, tmdprim.v1,
                    tmdprim.u2, tmdprim.v2);
                
               /*
                setUV3(poly,
                    tim_cube.prect->x+tmdprim.u0, tim_cube.prect->y+tmdprim.v0,                          // First vertex (top-left corner of texture)
                    tim_cube.prect->x+tmdprim.u1, tim_cube.prect->y+tmdprim.v1,      // Second vertex (top-right corner of texture)
                    tim_cube.prect->x+tmdprim.u2, tim_cube.prect->y+tmdprim.v2);     // Third vertex (bottom-left corner of texture)
                
                if(aka==1){
                printf("----------------\n");
                printf("1: %d\n", tim_cube.prect->x);
                printf("2: %d\n", tim_cube.prect->h);
                printf("3: %d\n", tim_cube.prect->y);
                printf("4: %d\n", tim_cube.prect->w);
                printf("-\n");
                printf("vy i:: %u\n", tmdprim.u0);
                printf("vy i+2:: %u\n", tmdprim.v0);
                printf("vy i:: %u\n", tmdprim.u1);
                printf("vy i+2:: %u\n", tmdprim.v1);
                printf("vy i:: %u\n", tmdprim.u2);
                printf("vy i+2:: %u\n", tmdprim.v2);
                printf("----------------");
                }
                // Rotate, translate, and project the vectors and output the results into a primitive
                OTz  = RotTransPers(&tmdprim.x0, (long*)&poly->x0, &p, &Flag);
                OTz += RotTransPers(&tmdprim.x1, (long*)&poly->x1, &p, &Flag);
                OTz += RotTransPers(&tmdprim.x2, (long*)&poly->x2, &p, &Flag);
                // Sort the primitive into the OT
                OTz /= 3;
                //if ((OTz > 0) && (OTz < OTLEN))
                AddPrim(&ot[db][OTz], poly);
                nextpri += sizeof(POLY_GT3);
                t+=3;
                //i++;
            }
            
        }
        if(aka==1){
        printf("---------------- CODE END ----------------\n"); 
        }

        FntPrint("OLA MUNDO!\n");
        FntFlush(-1);
        display();
    }
    return 0;
}

*/