import re
import sys

def parse_file(input_path):
    models = []
    current_model = None
    vertex_map = {}
    vertex_list = []
    faces = []

    with open(input_path, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith("*"):
                if current_model is not None:
                    models.append((vertex_list, faces))
                current_model = int(line[1:].split()[0])
                vertex_map = {}
                vertex_list = []
                faces = []
            elif line.startswith("("):
                matches = re.findall(r'\(\s*([-.\d]+)\s+([-.\d]+)\s+([-.\d]+)\s*\)', line)
                face_indices = []
                for match in matches:
                    x, y, z = (float(coord) for coord in match)
                    # Convert from (X, Y, Z) with Z-up to (X, Z, -Y) for OBJ
                    converted = (x, z, -y)
                    if converted not in vertex_map:
                        vertex_map[converted] = len(vertex_list) + 1
                        vertex_list.append(converted)
                    face_indices.append(vertex_map[converted])
                if len(face_indices) >= 3:
                    for i in range(1, len(face_indices) - 1):
                        faces.append((face_indices[0], face_indices[i], face_indices[i+1]))
        if current_model is not None:
            models.append((vertex_list, faces))
    return models

def write_obj(models, output_path):
    with open(output_path, 'w') as out:
        global_index_offset = 0
        for i, (vertices, faces) in enumerate(models):
            out.write(f'o model_{i}\n')
            for v in vertices:
                out.write(f'v {v[0]} {v[1]} {v[2]}\n')
            for face in faces:
                a, b, c = (idx + global_index_offset for idx in face)
                out.write(f'f {a} {b} {c}\n')
            global_index_offset += len(vertices)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python convert_to_obj.py input.txt output.obj")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]

    models = parse_file(input_path)
    write_obj(models, output_path)
    print(f"Wrote {len(models)} model(s) to {output_path}")
