

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
    int v1, v2, v3; // Indices to vertices
    int vt1, vt2, vt3;
    int vn1, vn2, vn3;
} Face;

SVECTOR vertices[2048]; // Adjust size accordingly
Face faces[2048];      
SVECTOR normals[2048];
DVECTOR textures[2048];
int vertexCount = 0, faceCount = 0, normalCount = 0, textureCount = 0;


void PrintFacesWithVertices(Cube* myCube) {
    //printf("\n=== Final Face Associations ===\n");

    // Allocate memory once
    myCube->mesh = malloc(faceCount * sizeof(SVECTOR_3D));
    if (!myCube->mesh) {
        printf("Memory allocation failed!\n");
        return;
    }

    myCube->n_prim = faceCount;

    // Loop through each face and print debug info
    for (int i = 0; i < faceCount; i++) {
        Face *f = &faces[i];

        // Get vertex data
        SVECTOR *v1 = &vertices[f->v1-1];
        SVECTOR *v2 = &vertices[f->v2-1];
        SVECTOR *v3 = &vertices[f->v3-1];

        DVECTOR *vt1 = &textures[f->vt1-1];
        DVECTOR *vt2 = &textures[f->vt2-1];
        DVECTOR *vt3 = &textures[f->vt3-1];

        /*
        // Debug: print the vertices for this face
        printf("Face %d:\n", i + 1);
        printf("  Vertex 1 (v1): (%d, %d, %d)\n", v1->vx, v1->vy, v1->vz);
        printf("  Vertex 2 (v2): (%d, %d, %d)\n", v2->vx, v2->vy, v2->vz);
        printf("  Vertex 3 (v3): (%d, %d, %d)\n", v3->vx, v3->vy, v3->vz);
*/
        // Copy values explicitly (fixing the issue)
        myCube->mesh[i].x.vx = v1->vx;
        myCube->mesh[i].x.vy = v1->vy;
        myCube->mesh[i].x.vz = v1->vz;

        myCube->mesh[i].y.vx = v2->vx;
        myCube->mesh[i].y.vy = v2->vy;
        myCube->mesh[i].y.vz = v2->vz;

        myCube->mesh[i].z.vx = v3->vx;
        myCube->mesh[i].z.vy = v3->vy;
        myCube->mesh[i].z.vz = v3->vz;

        myCube->mesh[i].u0 = vt1->vx;
        myCube->mesh[i].v0 = vt1->vy;

        myCube->mesh[i].u1 = vt2->vx;
        myCube->mesh[i].v1 = vt2->vy;

        myCube->mesh[i].u2 = vt3->vx;
        myCube->mesh[i].v2 = vt3->vy;

        //myCube->mesh[i].u0 = 
        
        // Debug: Print the values stored in the mesh after copying
        /*
        printf("Stored in mesh[%d]:\n", i);
        printf("  u: (%d, %d, %d)\n", myCube->mesh[i].u0, myCube->mesh[i].u1, myCube->mesh[i].u2);
        printf("  v: (%d, %d, %d)\n", myCube->mesh[i].v1, myCube->mesh[i].v1, myCube->mesh[i].v2);
        */
    }
}


int ParseFixedPoint(const char *str, int times) {
    int integerPart = 0, fractionPart = 0, divisor = 1, isFraction = 0, sign = 1;

    // Check for negative sign at the start
    if (*str == '-') { 
        sign = -1; 
        str++; 
    }

    // Parse the integer and fractional parts
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            if (isFraction) {
                fractionPart = fractionPart * 10 + (*str - '0');
                divisor *= 10;  // Adjust divisor to match decimal places
            } else {
                integerPart = integerPart * 10 + (*str - '0');
            }
        } else if (*str == '.') {
            isFraction = 1;  // Start parsing the fractional part
        } else {
            break;  // End of number parsing (non-digit character)
        }
        str++;
    }

    // Convert to fixed-point format (12.4 format)
    int fixedPoint = sign * ((integerPart << 4) + ((fractionPart << 4) / divisor));
    /*
    // Debugging: Print parsed values to check the output
    printf("Parsed fixed-point value: %d (integer: %d, fraction: %d, divisor: %d)\n", 
        fixedPoint, integerPart, fractionPart, divisor);
    */
    if(times>1){
        fixedPoint = sign * ((integerPart << 1) + ((fractionPart << 1) / divisor));

    }

    return fixedPoint;
}
int ParseFixedPointTimes10(const char *str,int width, int height) {
    long integerPart = 0, fractionPart = 0, divisor = 1, isFraction = 0;

    // Parse integer and fractional parts
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            if (isFraction) {
                fractionPart = fractionPart * 10 + (*str - '0');
                divisor *= 10;
            } else {
                integerPart = integerPart * 10 + (*str - '0');
            }
        } else if (*str == '.') {
            isFraction = 1;
        } else {
            break;
        }
        str++;
    }

    // Convert to integer scaled by 100
    long cu = (integerPart * 2100) + ((fractionPart * 2100) / divisor);
    if(width){
        cu = (cu * width)/2100;
    }
    else if(height){
        cu = (cu * height)/2100;
    }
    return (int)cu;
}
    
    
    
void loadMDL(const char *filename, Cube* myCube) {
    FileBuffer fileBuffer = loadFile(filename);
    char *line = fileBuffer.data;
    
    while (*line) {
        if (*line == 'v' && *(line + 1) == ' ') { // Vertex line
            line += 2; // Skip "v "
            vertices[vertexCount].vx = ParseFixedPoint(line, 1);
            while (*line != ' ' && *line) line++; line++; // Move to next number
            vertices[vertexCount].vy = ParseFixedPoint(line, 1);
            while (*line != ' ' && *line) line++; line++; // Move to next number
            vertices[vertexCount].vz = ParseFixedPoint(line, 1);
            /*
            // Debugging: Print parsed vertex
            printf("Vertex %d: (%d, %d, %d)\n", vertexCount, 
                vertices[vertexCount].vx, 
                vertices[vertexCount].vy, 
                vertices[vertexCount].vz);
            */
            vertexCount++;
        } 
        if (*line == 'v' && *(line + 1) == 't' && *(line + 2) == ' ') { // Vertex line
            line += 3; // Skip "vt "
            textures[textureCount].vx = ParseFixedPointTimes10(line, 0, 128);
            while (*line != ' ' && *line) line++; line++; // Move to next number
            textures[textureCount].vy = ParseFixedPointTimes10(line, 0, -256);
            
            printf("Vertex %d: (%d, %d)\n", textureCount, 
                textures[textureCount].vx, 
                textures[textureCount].vy);
            
           textureCount++;
        } 
        if (*line == 'v' && *(line + 1) == 'n' && *(line + 2) == ' ') { // Vertex line
            line += 3; // Skip "vn "
            normals[normalCount].vx = ParseFixedPoint(line, 1);
            while (*line != ' ' && *line) line++; line++; // Move to next number
            normals[normalCount].vy = ParseFixedPoint(line, 1);
            while (*line != ' ' && *line) line++; line++; // Move to next number
            normals[normalCount].vz = ParseFixedPoint(line, 1);
            /*
            // Debugging: Print parsed vertex
            printf("Vertex %d: (%d, %d, %d)\n", vertexCount, 
                vertices[vertexCount].vx, 
                vertices[vertexCount].vy, 
                vertices[vertexCount].vz);
            */
            normalCount++;
        } 
        else if (*line == 'f' && *(line + 1) == ' ') { // Face line
            line += 2; // Skip "f "
            int v1, v2, v3;
            int vt1, vt2, vt3;
            int vn1, vn2, vn3;
            // Parse the face vertex indices (1-based indexing)
            
            v1 = strtol(line, &line, 10); // Subtract 1 for 0-based index
            if (*line == '/'){ 
                line++;
                vt1 = strtol(line, &line, 10);
            }
            if (*line == '/'){ 
                line++;
                vn1 = strtol(line, &line, 10);
            }

            while (*line == ' ') line++; // Skip spaces

            v2 = strtol(line, &line, 10);
            if (*line == '/'){ 
                line++;
                vt2 = strtol(line, &line, 10);
            }
            if (*line == '/'){ 
                line++;
                vn2 = strtol(line, &line, 10);
            }

            while (*line == ' ') line++; // Skip spaces

            v3 = strtol(line, &line, 10);
            if (*line == '/'){ 
                line++;
                vt3 = strtol(line, &line, 10);
            }
            if (*line == '/'){ 
                line++;
                vn3 = strtol(line, &line, 10);
            }

            faces[faceCount].v1 = v1;
            faces[faceCount].v2 = v2;
            faces[faceCount].v3 = v3;

            faces[faceCount].vt1 = vt1;
            faces[faceCount].vt2 = vt2;
            faces[faceCount].vt3 = vt3;

            faces[faceCount].vn1 = vn1;
            faces[faceCount].vn2 = vn2;
            faces[faceCount].vn3 = vn3;

            /*
            // Debugging: Print parsed face
            printf("Face %d: (%d, %d, %d)\n", faceCount, 
                faces[faceCount].v1, 
                faces[faceCount].v2, 
                faces[faceCount].v3);
            */
            faceCount++;
        }

        // Move to next line
        while (*line && *line != '\n') line++;
        if (*line == '\n') line++;
    }

    // Optional: Print the faces and their corresponding vertices
    //printf("=== Final Face Associations ===\n");
// After parsing the faces
/*
for (int i = 0; i < faceCount; i++) {
    printf("Face %d:\n", i);
    // Access the correct vertex using face indices (adjusted for 0-based)
    printf("  Vertex 1 (v1): (%d, %d, %d)\n", 
           vertices[faces[i].v1 - 1].vx, 
           vertices[faces[i].v1 - 1].vy, 
           vertices[faces[i].v1 - 1].vz);
    printf("  Vertex 2 (v2): (%d, %d, %d)\n", 
           vertices[faces[i].v2 - 1].vx, 
           vertices[faces[i].v2 - 1].vy, 
           vertices[faces[i].v2 - 1].vz);
    printf("  Vertex 3 (v3): (%d, %d, %d)\n", 
           vertices[faces[i].v3 - 1].vx, 
           vertices[faces[i].v3 - 1].vy, 
           vertices[faces[i].v3 - 1].vz);
}
*/
}
