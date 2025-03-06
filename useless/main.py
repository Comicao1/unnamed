import struct

def obj_to_binary(input_file, output_file):
    vertices = []
    textures = []
    normals = []
    faces = []

    with open(input_file, 'r') as obj_file:
        for line in obj_file:
            parts = line.split()
            if not parts:
                continue
            if parts[0] == 'v':  # Vertex
                vertices.append(tuple(map(float, parts[1:4])))
            elif parts[0] == 'vt':  # Texture coordinate
                textures.append(tuple(map(float, parts[1:3])))
            elif parts[0] == 'vn':  # Normal
                normals.append(tuple(map(float, parts[1:4])))
            elif parts[0] == 'f':  # Face
                face = [tuple(map(int, vertex.split('/'))) for vertex in parts[1:]]
                faces.append(face)

    with open(output_file, 'wb') as binary_file:
        # Write header info
        binary_file.write(struct.pack('I', len(vertices)))
        binary_file.write(struct.pack('I', len(textures)))
        binary_file.write(struct.pack('I', len(normals)))
        binary_file.write(struct.pack('I', len(faces)))

        # Write vertex data
        for vertex in vertices:
            binary_file.write(struct.pack('fff', *vertex))

        # Write texture data
        for texture in textures:
            binary_file.write(struct.pack('ff', *texture))

        # Write normal data
        for normal in normals:
            binary_file.write(struct.pack('fff', *normal))

        # Write face data
        for face in faces:
            for vertex in face:
                binary_file.write(struct.pack('iii', *vertex))

obj_to_binary('mio.obj', 'model.bin')