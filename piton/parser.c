

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
    short v1, v2, v3; // Indices to vertices
    short vt1, vt2, vt3;
    short vn1, vn2, vn3;
} Face;

/*
typedef struct {
    Bone *ossos;
} Osseador;
*/

SVECTOR vertices[4096]; // Adjust size accordingly
SVECTOR vertices2[4096];
Face faces[4096];      
SVECTOR normals[4096];
DVECTOR textures[4096];
int vertexCount = 0, faceCount = 0, normalCount = 0, textureCount = 0;

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

int Cucucu(const char *str, int times) {
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
    //printf("%d %d \n", integerPart, fixedPoint);
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
    }else{
        cu = cu/2100;
    }
    return (int)cu;
}

int Testee(const char *str) {
    long integerPart = 0, fractionPart = 0, divisor = 1;
    short isFraction = 0, digitCount = 0;

    // Parse integer and fractional parts
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            if (isFraction) {
                if (digitCount < 4) {  // Limit fraction to 4 decimal places
                    fractionPart = fractionPart * 10 + (*str - '0');
                    divisor *= 10;
                    digitCount++;
                }
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

    // Convert to integer scaled by 10000
    long result = (integerPart * 100);
    if (divisor > 1) {
        result += (fractionPart * 100) / divisor;
    }

    return (int)result;
}


void parse_bone_data(const char *data, Bone *bones, short *bone_count) {
    const char *current_pos = data;
    
    while (1) {
        // Locate the start of a bone name (detects "Bone", "Bone.001", etc.)
        while (*current_pos && *current_pos != '\"') current_pos++;
        if (!*current_pos) break;

        // Extract bone name dynamically
        const char *name_start = ++current_pos;
        while (*current_pos && *current_pos != '\"') current_pos++;
        if (!*current_pos) break;

        int name_length = current_pos - name_start;
        if (name_length >= 32) name_length = 31; // Prevent overflow

        Bone *current_bone = &bones[*bone_count];
        strncpy(current_bone->bone_name, name_start, name_length);
        current_bone->bone_name[name_length] = '\0';

        //printf("Debug: Found bone %s\n", current_bone->bone_name);

        // Move to the vertex array

        current_pos = strstr(current_pos, "src");
        if (!current_pos) break;
        //current_pos += 7;  // Skip past "\"src\":"
        current_pos = strstr(current_pos, "\"x\":");
        current_pos += 5;
        current_bone->centro.vx =  (int)strtol(current_pos, NULL, 10);
        current_pos = strstr(current_pos, "\"y\":");
        current_pos += 5;
        current_bone->centro.vy =  (int)strtol(current_pos, NULL, 10);
        current_pos = strstr(current_pos, "\"z\":");
        current_pos += 5;
        current_bone->centro.vz =  (int)strtol(current_pos, NULL, 10);

        current_pos = strstr(current_pos, "vertex_center");
        if (!current_pos) break;
        //current_pos += 12;  // Skip past "\"src\":"
        current_pos = strstr(current_pos, "\"x\":");
        current_pos += 5;
        current_bone->vertex_center.vx =  (int)strtol(current_pos, NULL, 10);
        current_pos = strstr(current_pos, "\"y\":");
        current_pos += 5;
        current_bone->vertex_center.vy =  (int)strtol(current_pos, NULL, 10);
        current_pos = strstr(current_pos, "\"z\":");
        current_pos += 5;
        current_bone->vertex_center.vz =  (int)strtol(current_pos, NULL, 10);

        current_pos = strstr(current_pos, "rotation");
        if (!current_pos) break;
        //current_pos += 12;  // Skip past "\"src\":"
        current_pos = strstr(current_pos, "\"x\":");
        current_pos += 5;
        current_bone->rotacao.vx =  (int)strtol(current_pos, NULL, 10);
        current_pos = strstr(current_pos, "\"y\":");
        current_pos += 5;
        current_bone->rotacao.vy =  (int)strtol(current_pos, NULL, 10);
        current_pos = strstr(current_pos, "\"z\":");
        current_pos += 5;
        current_bone->rotacao.vz =  (int)strtol(current_pos, NULL, 10);

        if (*current_pos == ',') current_pos++;  // Skip comma

        printf("Bone %s - Position: %d, %d, %d\n",
            current_bone->bone_name,
            current_bone->centro.vx, current_bone->centro.vy, current_bone->centro.vz);
        printf("Bone %s - vertex center: %d, %d, %d\n",
            current_bone->bone_name,
            current_bone->vertex_center.vx, current_bone->vertex_center.vy, current_bone->vertex_center.vz);
        printf("Bone %s - rotacao: %d, %d, %d\n",
            current_bone->bone_name,
            current_bone->rotacao.vx, current_bone->rotacao.vy, current_bone->rotacao.vz);
        current_bone->vertex_count = 0;
        printf("CU\n");
        current_pos = strstr(current_pos, "\"data\":");
        while (1) {
            // Find vertex index

            const char *vertex_pos = strstr(current_pos, "\"vertex_index\":");
            if (!vertex_pos || vertex_pos > strstr(current_pos, "}")) break; // Ensure we don't go into another bone
            vertex_pos += 15; // Move past `"vertex_index":`

            int vertex_index = (int)strtol(vertex_pos, NULL, 10);

            // Find weight
            const char *weight_pos = strstr(vertex_pos, "\"weight\":");
            if (!weight_pos) break;
            weight_pos += 10; // Move past `"weight":`
            //printf("Debug: Found vertex %d", vertex_index);

            // Convert weight manually
            int weight = Testee(weight_pos);
            //printf("Debug: Found vertex %d, weight %d\n", vertex_index, weight);

            // Store the parsed vertex
            int v_count = current_bone->vertex_count;
            current_bone->vertices[v_count].vertex_index = vertex_index;
            current_bone->vertices[v_count].weight = weight;//(short)(weight * 32767); // Convert to 16-bit fixed-point
            current_bone->vertex_count++;
            printf("Debug: Found vertex %d, weight %d\n", current_bone->vertices[v_count].vertex_index, current_bone->vertices[v_count].weight);

            // Move to the next vertex
            current_pos = strstr(weight_pos, "}");
            current_pos += 1;
            //printf("Debug: Current char at position %ld: '%c'\n", current_pos - data, *current_pos);
            if (*current_pos != ',') {
                current_pos++; // Move past ']'
                break;
            current_pos = strstr(current_pos, "{");
            if (!current_pos) break;
            }
        }

        //printf("Debug: Finished parsing Bone %d (%s) with %d vertices\n", *bone_count, current_bone->bone_name, current_bone->vertex_count);

        (*bone_count)++;
        if (*bone_count >= 64) break; // Avoid overflow

        // Move to the next bone dynamically
        const char *next_bone = strstr(current_pos, "\"bone");
        if (!next_bone) break;
        current_pos = next_bone;
    }

    //printf("Debug: Total bones parsed: %d\n", *bone_count);
}
    
void loadMDL(const char *filename, Cube* myCube, const char *skeleton) {
    FileBuffer fileBuffer = loadFile(filename);
    FileBuffer fileBuffer2;// = loadFile(filename);
    if(skeleton != NULL){
        fileBuffer2 = loadFile(skeleton);
    }
    char *line = fileBuffer.data;
    char *line2;
    
    if(fileBuffer2.data != NULL){
        line2 = fileBuffer2.data;

    }

    while (*line) {
        if (*line == 'v' && *(line + 1) == ' ') { // Vertex line
            line += 2; // Skip "v "
            vertices[vertexCount].vx = Cucucu(line, 1);
            while (*line != ' ' && *line) line++; line++; // Move to next number
            vertices[vertexCount].vy = Cucucu(line, 1);
            while (*line != ' ' && *line) line++; line++; // Move to next number
            vertices[vertexCount].vz = Cucucu(line, 1);
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
            textures[textureCount].vx = ParseFixedPointTimes10(line, 0, 256);
            while (*line != ' ' && *line) line++; line++; // Move to next number
            textures[textureCount].vy = ParseFixedPointTimes10(line, 0, -256);
            //printf("Debug: Found  %d, %d \n", textures[textureCount].vx, textures[textureCount].vy);
            /*
            printf("Vertex %d: (%d, %d)\n", textureCount, 
                textures[textureCount].vx, 
                textures[textureCount].vy);
            */
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
    parse_bone_data(line2, myCube->bones, &myCube->boneCount);  
    //printf("\n=== Final Face Associations ===\n");

    // Allocate memory once
    myCube->mesh = malloc(faceCount * sizeof(SVECTOR_3D));
    if (!myCube->mesh) {
        printf("Memory allocation failed!\n");
        return;
    }

    myCube->n_prim = faceCount;
    for(int i = 0; i< myCube->boneCount; i++){
        Bone *cuzin = &myCube->bones[i];
        for(int j = 0; j < cuzin->vertex_count; j++){
            //printf("%d ", cuzin.vertices[j].vertex_index);
            Face *f = &faces[cuzin->vertices[j].vertex_index];
            SVECTOR *v1 = &vertices[f->v1-1];
            SVECTOR *v2 = &vertices[f->v2-1];
            SVECTOR *v3 = &vertices[f->v3-1];
            cuzin->mesho[j].x = *v1;
            cuzin->mesho[j].y = *v2;
            cuzin->mesho[j].z = *v3;
        }
    }
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
    memcpy(vertices2, vertices, sizeof(vertices)); // Copy the entire array
    //memset(vertices, 0, sizeof(vertices));
    //memset(textures, 0, sizeof(textures));
    //memset(normals, 0, sizeof(normals));
    //memset(faces, 0, sizeof(faces));
   // vertexCount = 0, faceCount = 0, normalCount = 0, textureCount = 0;

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

