#include "main.h"
#include "parser/parser.c"

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
static unsigned long heap[256 * 1024];

int main(void){
    InitHeap(heap, sizeof(heap));
    init();

    struct Objects objects;
    struct Cube Model;
    loadMDL("\\CUBIN.OBJ;1", &Model, "\\CUBIN.JSON;1");
    //LoadTexture(_binary_w_alientex_tim_start, &Model.tim);

    objects.cubes[0] = Model;

    long OTz, Flag, p, t, size;
    size = 7000; 
    objects.cubes[0].Trans.vy = -50;
    objects.cubes[0].Trans.vz = 100; //modificar a translacao
    objects.cubes[0].Rotate.vx = 1000;
    objects.cubes[0].Rotate.vy = 0; //modificar rotacao 
    objects.cubes[0].Scale.vx = size; //modificar escala
    objects.cubes[0].Scale.vy = size;
    objects.cubes[0].Scale.vz = size;
    short angle = 0; //angulo 0 - 360
    while (1)
    {
        ClearOTagR(ot[db], OTLEN);
        //objects.cubes[0].Rotate.vy += 10;  ; // Girar em runtime, desabilitado
        //objects.cubes[0].Rotate.vx += 10;  ; // Girar em runtime, desabilitado
        //objects.cubes[0].Rotate.vz += 10;  ; // Girar em runtime, desabilitado
        RotMatrix(&objects.cubes[0].Rotate, &objects.cubes[0].Matrix); //girar matrix
        TransMatrix(&objects.cubes[0].Matrix, &objects.cubes[0].Trans); //translacionar matriz
        ScaleMatrix(&objects.cubes[0].Matrix, &objects.cubes[0].Scale); //escalar a matriz
        SetRotMatrix(&objects.cubes[0].Matrix);  //setar a matrix
        SetTransMatrix(&objects.cubes[0].Matrix); //setar a matriz
        for (int i = 0; i < 1; i++) {
            Bone bone = objects.cubes[0].bones[i];
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

                sinY = csin(eulerY);
                cosY = ccos(eulerY);

                sinZ = csin(eulerZ);
                cosZ = ccos(eulerZ);
                
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
            objects.cubes[0].polygon = (POLY_GT3 *)nextpri;
            SetPolyGT3(objects.cubes[0].polygon);
            /*
            objects.cubes[0].polygon->tpage = getTPage(objects.cubes[0].tim.mode&0x3, 0,
                                                objects.cubes[0].tim.prect->x,
                                                objects.cubes[0].tim.prect->y
                                                );
            
                                                
            if((objects.cubes[0].tim.mode & 0x3) < 2){
                setClut( objects.cubes[0].polygon,          
                        objects.cubes[0].tim.crect->x,
                        objects.cubes[0].tim.crect->y
                );
            }   
            */
            setRGB0(objects.cubes[0].polygon, 0,255,0);
            setRGB1(objects.cubes[0].polygon, 0,0,255);
            setRGB2(objects.cubes[0].polygon, 255,0,0);
            //setRGB1(objects.cubes[0].polygon, 120,0,0);
            //setRGB2(objects.cubes[0].polygon, 0,120,0);
            //setRGB0(objects.cubes[0].polygon, objects.cubes[0].mesh[i].r0, objects.cubes[0].mesh[i].g0, objects.cubes[0].mesh[i].b0);
            //setRGB1(objects.cubes[0].polygon, objects.cubes[0].mesh[i].r1, objects.cubes[0].mesh[i].g1, objects.cubes[0].mesh[i].b1);
            //setRGB2(objects.cubes[0].polygon, objects.cubes[0].mesh[i].r2, objects.cubes[0].mesh[i].g2, objects.cubes[0].mesh[i].b2);
            /*
            setUV3(objects.cubes[0].polygon,
                objects.cubes[0].mesh[i].u0, objects.cubes[0].mesh[i].v0,
                objects.cubes[0].mesh[i].u1, objects.cubes[0].mesh[i].v1,
                objects.cubes[0].mesh[i].u2, objects.cubes[0].mesh[i].v2);
            */
            Face *f = &faces[i];
            SVECTOR *v1 = &vertices[f->v1-1];
            SVECTOR *v2 = &vertices[f->v2-1];
            SVECTOR *v3 = &vertices[f->v3-1];

            OTz  = RotTransPers(v1, (long*)&objects.cubes[0].polygon->x0, &p, &Flag);
            OTz += RotTransPers(v2, (long*)&objects.cubes[0].polygon->x1, &p, &Flag);
            OTz += RotTransPers(v3, (long*)&objects.cubes[0].polygon->x2, &p, &Flag);
            OTz /= 3;
            //if ((OTz > 0) && (OTz < OTLEN))
            AddPrim(&ot[db][OTz], objects.cubes[0].polygon);
            nextpri += sizeof(POLY_GT3);
        }
        FntPrint("OLA MUNDO!\n");
        FntFlush(-1);
        display();
    
    }


    return 0;
}