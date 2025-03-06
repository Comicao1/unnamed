import struct

def read_tmd(file_path):
    with open(file_path, 'rb') as f:
        # Read the TMD header and data
        # (You'll need to know the TMD file structure for this)
        header = f.read(32)  # Example: Read the first 32 bytes as the header
        vertices = []
        polygons = []
        # Parse vertices and polygons from the file
        # ...
        return vertices, polygons

def write_bin(file_path, vertices, polygons):
    with open(file_path, 'wb') as f:
        # Write vertices and polygons to the binary file
        for vertex in vertices:
            f.write(struct.pack('fff', *vertex))  # Example: Write as 3 floats
        for polygon in polygons:
            f.write(struct.pack('HHH', *polygon))  # Example: Write as 3 unsigned shorts

# Convert TMD to BIN
vertices, polygons = read_tmd('w_alien.tmd')
write_bin('modelo_alien.bin', vertices, polygons)