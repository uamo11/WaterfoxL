import os
import struct

CACHE_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "native_ui", "cache")
os.makedirs(CACHE_DIR, exist_ok=True)

def write_bmp(filename, width, height, pixels):
    """
    Escribe un archivo BMP estándar de 24-bits (BGR sin compresión).
    pixels: lista de filas de abajo hacia arriba (bottom-to-top), cada fila tiene (R, G, B) por pixel.
    """
    row_bytes = width * 3
    padding = (4 - (row_bytes % 4)) % 4
    image_size = (row_bytes + padding) * height
    file_size = 54 + image_size

    # BITMAPFILEHEADER (14 bytes)
    header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, 54)
    # BITMAPINFOHEADER (40 bytes)
    info = struct.pack('<IIIHHIIIIII', 40, width, height, 1, 24, 0, image_size, 2835, 2835, 0, 0)

    data = bytearray()
    for row in pixels:
        for r, g, b in row:
            data.extend([b, g, r]) # BGR
        data.extend([0] * padding)

    filepath = os.path.join(CACHE_DIR, filename)
    with open(filepath, 'wb') as f:
        f.write(header)
        f.write(info)
        f.write(data)
    print(f"[+] Generado icono en cache: {filepath} ({len(header) + len(info) + len(data)} bytes)")

def create_icons():
    W, H = 32, 32

    # 1. x.bmp (Twitter / X) - Fondo negro con X blanca
    x_pixels = []
    for y in range(H):
        row = []
        for x in range(W):
            # Líneas diagonales de X
            diag1 = abs(x - y) <= 2
            diag2 = abs((W - 1 - x) - y) <= 2
            if (diag1 or diag2) and 5 <= x <= 26 and 5 <= y <= 26:
                row.append((240, 246, 252))
            else:
                row.append((10, 12, 16))
        x_pixels.append(row)
    write_bmp("x.bmp", W, H, x_pixels)

    # 2. ytm.bmp (YouTube Music) - Círculo rojo oscuro con triángulo de reproducción
    ytm_pixels = []
    cx, cy, r_circ = 15.5, 15.5, 13.5
    for y in range(H):
        row = []
        for x in range(W):
            dist = ((x - cx)**2 + (y - cy)**2)**0.5
            if dist <= r_circ:
                # Triángulo de play en el centro
                px, py = x - 11, y - 9
                in_play = 0 <= px <= 12 and abs(py - 6) <= (px * 0.55)
                if in_play:
                    row.append((255, 255, 255))
                else:
                    row.append((230, 33, 23))
            else:
                row.append((13, 17, 23)) # Color fondo
        ytm_pixels.append(row)
    write_bmp("ytm.bmp", W, H, ytm_pixels)

    # 3. yt.bmp (YouTube) - Rectángulo redondeado rojo con play blanco
    yt_pixels = []
    for y in range(H):
        row = []
        for x in range(W):
            in_rect = 3 <= x <= 28 and 7 <= y <= 24
            if in_rect:
                # Triángulo blanco
                px, py = x - 13, y - 10
                in_play = 0 <= px <= 9 and abs(py - 5) <= (px * 0.6)
                if in_play:
                    row.append((255, 255, 255))
                else:
                    row.append((255, 0, 0))
            else:
                row.append((13, 17, 23))
        yt_pixels.append(row)
    write_bmp("yt.bmp", W, H, yt_pixels)

    # 4. fb.bmp (Facebook) - Fondo azul con 'f' blanca
    fb_pixels = []
    for y in range(H):
        row = []
        for x in range(W):
            dist = ((x - cx)**2 + (y - cy)**2)**0.5
            if dist <= r_circ:
                # Dibuja la 'f'
                is_stem = 15 <= x <= 19 and 6 <= y <= 24
                is_cross = 12 <= x <= 22 and 17 <= y <= 20
                is_hook = 15 <= x <= 22 and 22 <= y <= 25
                if is_stem or is_cross or is_hook:
                    row.append((255, 255, 255))
                else:
                    row.append((24, 119, 242))
            else:
                row.append((13, 17, 23))
        fb_pixels.append(row)
    write_bmp("fb.bmp", W, H, fb_pixels)

    # 5. gemini.bmp (Gemini) - Gradiente cian/violeta con estrella
    gem_pixels = []
    for y in range(H):
        row = []
        for x in range(W):
            dist = ((x - cx)**2 + (y - cy)**2)**0.5
            if dist <= r_circ:
                # Estrella de 4 puntas
                dx, dy = abs(x - cx), abs(y - cy)
                if (dx <= 2 and dy <= 9) or (dy <= 2 and dx <= 9) or (dx + dy <= 5):
                    row.append((255, 255, 255))
                else:
                    # Gradiente
                    r = int(27 + (155 - 27) * (x / W))
                    g = int(161 + (81 - 161) * (y / H))
                    b = int(226 + (224 - 226) * (x / W))
                    row.append((r, g, b))
            else:
                row.append((13, 17, 23))
        gem_pixels.append(row)
    write_bmp("gemini.bmp", W, H, gem_pixels)

    # 6. gta.bmp (GTA V Web) - Verde oscuro con 'V' dorada
    gta_pixels = []
    for y in range(H):
        row = []
        for x in range(W):
            dist = ((x - cx)**2 + (y - cy)**2)**0.5
            if dist <= r_circ:
                # Letra V romana
                vx = x - 15.5
                vy = y - 7
                in_v = 0 <= vy <= 16 and (abs(abs(vx) - vy * 0.45) <= 1.6)
                if in_v:
                    row.append((255, 215, 0)) # Oro
                else:
                    row.append((30, 100, 45)) # Verde GTA
            else:
                row.append((13, 17, 23))
        gta_pixels.append(row)
    write_bmp("gta.bmp", W, H, gta_pixels)

    # 7. wf.bmp (Waterfox Logo)
    wf_pixels = []
    for y in range(H):
        row = []
        for x in range(W):
            dist = ((x - cx)**2 + (y - cy)**2)**0.5
            if dist <= r_circ:
                if dist >= 11:
                    row.append((88, 166, 255)) # Anillo azul
                elif x > cx and y > cy:
                    row.append((31, 111, 235)) # Zorro azul marino
                else:
                    row.append((88, 166, 255)) # Zorro celeste
            else:
                row.append((13, 17, 23))
        wf_pixels.append(row)
    write_bmp("wf.bmp", W, H, wf_pixels)

    # 8. folder.bmp (Carpeta para barra de marcadores)
    folder_pixels = []
    for y in range(16):
        row = []
        for x in range(16):
            if (1 <= x <= 14 and 2 <= y <= 12) or (1 <= x <= 6 and 12 <= y <= 14):
                row.append((235, 180, 50))
            else:
                row.append((22, 27, 34))
        folder_pixels.append(row)
    write_bmp("folder.bmp", 16, 16, folder_pixels)

if __name__ == '__main__':
    create_icons()
