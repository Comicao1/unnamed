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

static unsigned long heap[256 * 1024];
int main(void){
    InitHeap(heap, sizeof(heap));
    init();

    struct Cube myCube2 = {};
    //loadFile("\\MODEL.BIN;1");
    //loadMDL("\\CUBINHO.OBJ;1", &myCube2);
    //loadMDL("\\CUBAS.OBJ;1", &myCube2);
    //loadMDL("\\TRIGIX.OBJ;1", &myCube2);
    loadMDL("\\HIBRIDO3.OBJ;1", &myCube2, "\\BONE.JSON;1");
    //PrintFacesWithVertices(&myCube2);

    struct Objects object;
    LoadTexture(_binary_og_pipe02_tim_start, &myCube2.tim);

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
    object.cubes[0].Trans.vy = 85;
    object.cubes[0].Trans.vz = 200;
    object.cubes[0].Rotate.vx = 2000;
    long size = 350; 
    object.cubes[0].Scale.vx = size;
    object.cubes[0].Scale.vy = size;
    object.cubes[0].Scale.vz = size;

    while (1)
    {
        ClearOTagR(ot[db], OTLEN);
        object.cubes[0].Rotate.vy += 20  ; // Pan
        //object.cubes[0].Rotate.vx += 10  ; // Pan
        RotMatrix(&object.cubes[0].Rotate, &object.cubes[0].Matrix);
        TransMatrix(&object.cubes[0].Matrix, &object.cubes[0].Trans);
        ScaleMatrix(&object.cubes[0].Matrix, &object.cubes[0].Scale);
        SetRotMatrix(&object.cubes[0].Matrix);
        SetTransMatrix(&object.cubes[0].Matrix);
        //printf("hallo");
        //printf("index: %d", object.cubes[0].boneCount);
        /*
        for(int i = 0; i < object.cubes[0].boneCount; i++){
            Bone cuzin = object.cubes[0].bones[i];
            for(int j = 0; j < cuzin.vertex_count; j++){
                printf("%d ", cuzin.vertices[j].vertex_index);
            }
            break;
        }
        */
        for (int i = 0; i < object.cubes[0].n_prim; i++) {
            object.cubes[0].polygon = (POLY_GT3 *)nextpri;
            SetPolyGT3(object.cubes[0].polygon);
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
            
            setRGB0(object.cubes[0].polygon, 255,255,255);
            setRGB1(object.cubes[0].polygon, 255,255,255);
            setRGB2(object.cubes[0].polygon, 255,255,255);
            //setRGB1(object.cubes[0].polygon, 120,0,0);
            //setRGB2(object.cubes[0].polygon, 0,120,0);
            //setRGB0(object.cubes[0].polygon, object.cubes[0].mesh[i].r0, object.cubes[0].mesh[i].g0, object.cubes[0].mesh[i].b0);
            //setRGB1(object.cubes[0].polygon, object.cubes[0].mesh[i].r1, object.cubes[0].mesh[i].g1, object.cubes[0].mesh[i].b1);
            //setRGB2(object.cubes[0].polygon, object.cubes[0].mesh[i].r2, object.cubes[0].mesh[i].g2, object.cubes[0].mesh[i].b2);

            setUV3(object.cubes[0].polygon,
                object.cubes[0].mesh[i].u0, object.cubes[0].mesh[i].v0,
                object.cubes[0].mesh[i].u1, object.cubes[0].mesh[i].v1,
                object.cubes[0].mesh[i].u2, object.cubes[0].mesh[i].v2);

            OTz  = RotTransPers(&object.cubes[0].mesh[i].x, (long*)&object.cubes[0].polygon->x0, &p, &Flag);
            OTz += RotTransPers(&object.cubes[0].mesh[i].y, (long*)&object.cubes[0].polygon->x1, &p, &Flag);
            OTz += RotTransPers(&object.cubes[0].mesh[i].z, (long*)&object.cubes[0].polygon->x2, &p, &Flag);
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