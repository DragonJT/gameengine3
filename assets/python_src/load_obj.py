def load_obj(path):
    verts = []  # 1-based in OBJ
    norms = []  # 1-based in OBJ
    out = []  # flat float list: pos+normal per vertex

    with open(path, "r") as f:
        for line in f:
            if not line or line[0] == "#":
                continue

            parts = line.strip().split()
            if not parts:
                continue

            if parts[0] == "v":
                # v x y z
                verts.append((float(parts[1]), float(parts[2]), float(parts[3])))

            elif parts[0] == "vn":
                # vn x y z
                norms.append((float(parts[1]), float(parts[2]), float(parts[3])))

            elif parts[0] == "f":
                # supports polygons: f v... or v//vn or v/vt/vn
                face = parts[1:]
                if len(face) < 3:
                    continue

                # parse one face token -> (pos_index, norm_index or -1)
                def parse_tok(tok):
                    comps = tok.split("/")
                    vi = int(comps[0]) - 1  # OBJ is 1-based
                    ni = -1
                    if len(comps) >= 3 and comps[2] != "":
                        ni = int(comps[2]) - 1
                    return vi, ni

                # triangulate via fan: (0, i, i+1)
                v0i, n0i = parse_tok(face[0])
                for i in range(1, len(face) - 1):
                    v1i, n1i = parse_tok(face[i])
                    v2i, n2i = parse_tok(face[i + 1])

                    for vi, ni in [(v0i, n0i), (v1i, n1i), (v2i, n2i)]:
                        px, py, pz = verts[vi]
                        if 0 <= ni < len(norms):
                            nx, ny, nz = norms[ni]
                        else:
                            nx, ny, nz = 0.0, 0.0, 0.0
                        out += [px, py, pz, nx, ny, nz]

    return out
