import struct
import zlib
import os

def write_png(filename, width, height, pixels):
    # pixels is a list of (r, g, b, a) tuples
    def chunk(tag, data):
        return struct.pack("!I", len(data)) + tag + data + struct.pack("!I", zlib.crc32(tag + data))

    # Signature
    png = b"\x89PNG\r\n\x1a\n"
    
    # IHDR
    ihdr = struct.pack("!IIBBBBB", width, height, 8, 6, 0, 0, 0)
    png += chunk(b"IHDR", ihdr)
    
    # IDAT
    raw_data = b""
    for y in range(height):
        raw_data += b"\x00" # Filter type 0 (None)
        for x in range(width):
            r, g, b, a = pixels[y * width + x]
            raw_data += struct.pack("BBBB", r, g, b, a)
            
    png += chunk(b"IDAT", zlib.compress(raw_data))
    
    # IEND
    png += chunk(b"IEND", b"")
    
    with open(filename, "wb") as f:
        f.write(png)

# Colors
TRANSPARENT = (0, 0, 0, 0)
PURPLE = (128, 0, 128, 255)
CYAN = (0, 255, 255, 255)
RED = (230, 41, 55, 255)
WHITE = (255, 255, 255, 255)
BLACK = (0, 0, 0, 255)
YELLOW = (255, 255, 0, 255)

def create_wizard():
    w, h = 20, 20
    pixels = [TRANSPARENT] * (w * h)
    for y in range(h):
        for x in range(w):
            # Hat
            if 2 <= y <= 8 and abs(x - 10) <= (y - 2):
                pixels[y * w + x] = PURPLE
            # Body
            elif 8 < y < 18 and 6 <= x <= 14:
                pixels[y * w + x] = PURPLE
    write_png("hunter/assets/wizard.png", w, h, pixels)

def create_ghost():
    w, h = 20, 20
    pixels = [TRANSPARENT] * (w * h)
    for y in range(h):
        for x in range(w):
            if 4 <= y <= 16 and 4 <= x <= 16:
                pixels[y * w + x] = CYAN
            # Eyes
            if y == 8 and (x == 7 or x == 12):
                pixels[y * w + x] = WHITE
            if y == 8 and (x == 8 or x == 13):
                pixels[y * w + x] = BLACK
    write_png("hunter/assets/ghost.png", w, h, pixels)

def create_demon():
    w, h = 20, 20
    pixels = [TRANSPARENT] * (w * h)
    for y in range(h):
        for x in range(w):
            # Box
            if 4 <= y <= 16 and 4 <= x <= 16:
                pixels[y * w + x] = RED
            # Horns
            if (y == 3 or y==4) and (x == 4 or x == 16):
                 pixels[y * w + x] = (150, 0, 0, 255)
            # Eyes
            if y == 8 and (x == 7 or x == 12):
                pixels[y * w + x] = YELLOW
    write_png("hunter/assets/demon.png", w, h, pixels)

def create_heart():
    w, h = 20, 20
    pixels = [TRANSPARENT] * (w * h)
    for y in range(h):
        for x in range(w):
            # Simple 8-bit heart shape
            # Upper lobes
            if 4 <= y <= 7:
                if (3 <= x <= 9) or (10 <= x <= 16):
                    pixels[y * w + x] = RED
            # Taper
            elif 8 <= y <= 16:
                 if abs(x - 9.5) < (17 - y):
                    pixels[y * w + x] = RED
    write_png("hunter/assets/heart.png", w, h, pixels)

if __name__ == "__main__":
    if not os.path.exists("hunter/assets"):
        os.makedirs("hunter/assets")
    create_wizard()
    create_ghost()
    create_demon()
    create_heart()
    print("Assets generated.")
