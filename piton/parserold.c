/*

#include "../main.h"

typedef struct {
    char *data;
    int size;
} FileBuffer;

FileBuffer loadFile(const char* filename) {
    CdlFILE filePos;
    int numsecs;
    FileBuffer fileBuffer = {NULL, 0};

    // Locate the file on the CD
    if (CdSearchFile(&filePos, filename) == NULL) {
        printf("%s could not be found\n", filename);
    } else {
        numsecs = (filePos.size + 2047) / 2048;          // Calculate the number of sectors to read
        fileBuffer.size = filePos.size;                   // Store the file size
        fileBuffer.data = (char*)malloc(2048 * numsecs);  // Allocate buffer for the file

        CdControl(CdlSetloc, (u_char*)&filePos.pos, 0);   // Set the file as the read target
        CdRead(numsecs, (u_long*)fileBuffer.data, CdlModeSpeed); // Start the read operation
        CdReadSync(0, 0);                                // Wait until the read operation is complete
    }

    return fileBuffer;
}

typedef struct {
    unsigned long ID; /* Constant = 0x41 */
    unsigned long FLAGS;
    unsigned long NOBJ; /* Number of objects in file */
} Header;

typedef struct object {
    unsigned long vert_top;    // Store the actual value, not a pointer
    unsigned long n_vert;
    unsigned long normal;      // Store the actual value, not a pointer
    unsigned long n_normal;
    unsigned long primitive;   // Store the actual value, not a pointer
    unsigned long n_primitive;
    long scale;
} object;

typedef struct {
	unsigned char olen;
	unsigned char ilen; /* Length of following primitive in 32bit words */
	unsigned char flag;
	unsigned char mode;	/* primitive type */
    //char adasd[20];
} tmd_prim_header_t;

typedef struct {
	short x;
	short y;
	short z;
	short zero;
} tmd_vertex_t;

typedef struct 			// ilen 6 olen 7
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char r0, g0, b0, pad1;
	u_short vert0, vert1;
	u_short vert2, pad2;
	} TMD_F_3T_NL;

typedef struct 			// ilen 7 olen 9
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_char r0, g0, b0, pad2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_F_4T_NL;

typedef struct  			// ilen 4 olen 6
	{
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	} TMD_G_3;

typedef struct 			// ilen 5 olen 6
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_short vert0, vert1;
	u_short vert2, pad2;
	} TMD_G_3_NL;


void loadMDL(const char *filename, Cube* myCube) {
    FileBuffer fileBuffer = loadFile(filename);

    if (fileBuffer.data != NULL) {
        printf("File loaded successfully\n");

        // Parse the header (first 12 bytes)
        Header* header = (Header*)fileBuffer.data;
        printf("ID: %x\n", header->ID);
        printf("FLAGS: %x\n", header->FLAGS);
        printf("NOBJ: %u\n", header->NOBJ);

        // Move the pointer past the header
        char* dataPtr = fileBuffer.data + sizeof(Header);
        printf("offset 1: %#lx\n", (unsigned long)(dataPtr - fileBuffer.data));

        // Now parse the objects (NOBJ)
        object* objects = (object*)malloc(header->NOBJ * sizeof(object));
        if (objects == NULL) {
            printf("Failed to allocate memory for objects\n");
            free(fileBuffer.data);
            return;
        }
        for (uint32_t i = 0; i < header->NOBJ; i++) {
            objects->vert_top = *(unsigned long*)dataPtr;
            dataPtr += sizeof(unsigned long);

            objects->n_vert = *(unsigned long*)dataPtr;
            dataPtr += sizeof(unsigned long);

            objects->normal = *(unsigned long*)dataPtr;
            dataPtr += sizeof(unsigned long);

            objects->n_normal = *(unsigned long*)dataPtr;
            dataPtr += sizeof(unsigned long);

            objects->primitive = *(unsigned long*)dataPtr;
            dataPtr += sizeof(unsigned long);

            objects->n_primitive = *(unsigned long*)dataPtr;
            dataPtr += sizeof(unsigned long);

            objects->scale = *(long*)dataPtr;
            dataPtr += sizeof(long);

            printf("offset 2: %#lx\n", (unsigned long)(dataPtr - fileBuffer.data));

            printf("vert_top: %lu\n", objects->vert_top);
            printf("nvert: %lu\n", objects->n_vert);
            printf("normal: %lu\n", objects->normal);
            printf("n_normal: %lu\n", objects->n_normal);
            printf("primitive: %lu\n", objects->primitive);
            printf("n_primitive: %lu\n", objects->n_primitive);
            printf("scale: %lu\n", objects->scale);

            dataPtr += objects->n_primitive*sizeof(tmd_prim_header_t);
            printf("offset 3: %#lx\n", (unsigned long)(dataPtr - fileBuffer.data));

            //char* dataPtr2 = fileBuffer.data + sizeof(Header) + (sizeof(object)) + (objects->n_primitive * sizeof(tmd_prim_header_t));
            unsigned long dataPtr2 =  (unsigned long)(fileBuffer.data) + objects->primitive + sizeof(Header);
            unsigned long dataPtr3 =  (unsigned long)(fileBuffer.data) + objects->vert_top + sizeof(Header);
            printf("offset 4: %#lx\n", dataPtr2-(unsigned long)fileBuffer.data);
            //printf("offset 5 normal: %#lx\n", dataPtr3-(unsigned long)fileBuffer.data);
            myCube->mesh = malloc(objects->n_primitive * sizeof(SVECTOR_3D));
            myCube->n_prim = objects->n_primitive;


            printf("offset 5: %#lx\n", dataPtr3-(unsigned long)fileBuffer.data);
            int iii = 0;
            for(int j = 0; j < objects->n_primitive; j++){
                unsigned char olena, ilena, flaga, modea;
                olena = *(unsigned char*)dataPtr2;
                dataPtr2 += sizeof(unsigned char);

                ilena = *(unsigned char*)dataPtr2;
                dataPtr2 += sizeof(unsigned char);

                flaga = *(unsigned char*)dataPtr2;
                dataPtr2 += sizeof(unsigned char);

                modea = *(unsigned char*)dataPtr2;
                dataPtr2 += sizeof(unsigned char);

                if(olena == 6 && ilena == 5){
                    TMD_G_3_NL aha = *(TMD_G_3_NL*)dataPtr2;
                    dataPtr2 += sizeof(TMD_G_3_NL);
                    tmd_vertex_t* vertexData = (tmd_vertex_t*)dataPtr3;
                    myCube->mesh[j].x.vx = vertexData[aha.vert0].x;
                    myCube->mesh[j].x.vy = vertexData[aha.vert0].y;
                    myCube->mesh[j].x.vz = vertexData[aha.vert0].z;

                    myCube->mesh[j].y.vx = vertexData[aha.vert1].x;
                    myCube->mesh[j].y.vy = vertexData[aha.vert1].y;
                    myCube->mesh[j].y.vz = vertexData[aha.vert1].z;

                    myCube->mesh[j].z.vx = vertexData[aha.vert2].x;
                    myCube->mesh[j].z.vy = vertexData[aha.vert2].y;
                    myCube->mesh[j].z.vz = vertexData[aha.vert2].z;
                }
                if(olena == 9 && ilena == 7){
                    TMD_F_4T_NL aha = *(TMD_F_4T_NL*)dataPtr2;
                    dataPtr2 += sizeof(TMD_F_4T_NL);

                }if(olena == 7 && ilena == 6){
                        TMD_F_3T_NL aha = *(TMD_F_3T_NL*)dataPtr2;
                        dataPtr2 += sizeof(TMD_F_3T_NL);
                        tmd_vertex_t* vertexData = (tmd_vertex_t*)dataPtr3;
                        myCube->mesh[j].x.vx = vertexData[aha.vert0].x;
                        myCube->mesh[j].x.vy = vertexData[aha.vert0].y;
                        myCube->mesh[j].x.vz = vertexData[aha.vert0].z;

                        myCube->mesh[j].y.vx = vertexData[aha.vert1].x;
                        myCube->mesh[j].y.vy = vertexData[aha.vert1].y;
                        myCube->mesh[j].y.vz = vertexData[aha.vert1].z;

                        myCube->mesh[j].z.vx = vertexData[aha.vert2].x;
                        myCube->mesh[j].z.vy = vertexData[aha.vert2].y;
                        myCube->mesh[j].z.vz = vertexData[aha.vert2].z;
                }

            }
        }

        free(fileBuffer.data);  // Free the file buffer after use
    } else {
        printf("Failed to load file: %s\n", filename);
    }
}


*/