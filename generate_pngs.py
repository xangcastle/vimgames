
import struct
import zlib
import math
import os

def write_png(filename, width, height, pixels):
    # pixels is a list of (r, g, b, a) tuples
    # Construct PNG file in memory
    
    # 1. Signature
    png_sig = b'\x89PNG\r\n\x1a\n'
    
    # 2. IHDR chunk
    # Width (4), Height (4), Bit depth (1), Color type (1), Compression (1), Filter (1), Interlace (1)
    # Color type 6 = Truecolor with alpha
    ihdr_data = struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0)
    ihdr_crc = zlib.crc32(b'IHDR' + ihdr_data) & 0xffffffff
    ihdr_chunk = struct.pack('>I', len(ihdr_data)) + b'IHDR' + ihdr_data + struct.pack('>I', ihdr_crc)
    
    # 3. IDAT chunk
    # Filter type 0 (None) for each scanline
    raw_data = b''
    for y in range(height):
        raw_data += b'\x00' # Filter type 0
        for x in range(width):
            r, g, b, a = pixels[y * width + x]
            raw_data += struct.pack('BBBB', r, g, b, a)
            
    compressed_data = zlib.compress(raw_data)
    idat_crc = zlib.crc32(b'IDAT' + compressed_data) & 0xffffffff
    idat_chunk = struct.pack('>I', len(compressed_data)) + b'IDAT' + compressed_data + struct.pack('>I', idat_crc)
    
    # 4. IEND chunk
    iend_crc = zlib.crc32(b'IEND') & 0xffffffff
    iend_chunk = struct.pack('>I', 0) + b'IEND' + struct.pack('>I', iend_crc)
    
    with open(filename, 'wb') as f:
        f.write(png_sig)
        f.write(ihdr_chunk)
        f.write(idat_chunk)
        f.write(iend_chunk)
    # print(f"Wrote {filename}")

# Colors
TRANSPARENT = (0, 0, 0, 0)
WHITE = (255, 255, 255, 255)
BLACK = (0, 0, 0, 255)
RED = (255, 0, 0, 255)
GREEN = (0, 255, 0, 255)
BLUE = (0, 0, 255, 255)
YELLOW = (255, 255, 0, 255)
CYAN = (0, 255, 255, 255)
MAGENTA = (255, 0, 255, 255)
GRAY = (100, 100, 100, 255)
DARK_GRAY = (50, 50, 50, 255)
BROWN = (160, 120, 60, 255)
DARK_BROWN = (100, 70, 30, 255)

def fill_rect(pixels, w, h, x, y, rw, rh, color):
    for j in range(max(0, y), min(h, y+rh)):
        for i in range(max(0, x), min(w, x+rw)):
            pixels[j*w + i] = color

def draw_line(pixels, w, h, x0, y0, x1, y1, color):
    # Bresenham's line algorithm
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx - dy
    
    while True:
        if 0 <= x0 < w and 0 <= y0 < h:
            pixels[y0*w + x0] = color
        if x0 == x1 and y0 == y1:
            break
        e2 = 2 * err
        if e2 > -dy:
            err -= dy
            x0 += sx
        if e2 < dx:
            err += dx
            y0 += sy

def create_wizard():
    w, h = 20, 20
    pixels = [TRANSPARENT] * (w * h)
    
    # Hat
    for i in range(2, 18): # Rim
        pixels[10 * w + i] = MAGENTA
    for y in range(2, 10): # Cone
        width_at_y = (y - 2)
        for x in range(10 - width_at_y, 10 + width_at_y + 1):
             pixels[y * w + x] = MAGENTA
             
    # Face
    fill_rect(pixels, w, h, 6, 11, 8, 6, CYAN)
    
    # Eyes
    pixels[13 * w + 8] = BLACK
    pixels[13 * w + 11] = BLACK
    
    write_png("hunter/assets/wizard.png", w, h, pixels)

def create_ghost():
    w, h = 20, 20
    pixels = [TRANSPARENT] * (w * h)
    
    # Body
    for y in range(4, 18):
        for x in range(4, 16):
            pixels[y * w + x] = CYAN
            
    # Rounded top
    for x in range(6, 14):
        pixels[3 * w + x] = CYAN
    for x in range(5, 15):
        pixels[4 * w + x] = CYAN
        
    # Eyes
    pixels[8 * w + 7] = BLACK
    pixels[8 * w + 12] = BLACK
    
    write_png("hunter/assets/ghost.png", w, h, pixels)

def create_demon():
    w, h = 20, 20
    pixels = [TRANSPARENT] * (w * h)
    
    # Face
    fill_rect(pixels, w, h, 4, 4, 12, 12, RED)
    
    # Horns
    pixels[3 * w + 3] = RED
    pixels[2 * w + 2] = RED
    pixels[3 * w + 16] = RED
    pixels[2 * w + 17] = RED
    
    # Eyes
    pixels[8 * w + 7] = YELLOW
    pixels[8 * w + 12] = YELLOW
    
    write_png("hunter/assets/demon.png", w, h, pixels)

def create_heart():
    w, h = 40, 40
    pixels = [TRANSPARENT] * (w * h)
    
    # Simple mathematical heart approximation
    for y in range(h):
        for x in range(w):
            # Normalize to -1.5 to 1.5
            nx = (x - w/2) / (w/4)
            ny = (y - h/2) / (h/4)
            ny = -ny # Flip Y
            
            # (x^2 + y^2 - 1)^3 - x^2 * y^3 <= 0
            val = (nx**2 + ny**2 - 1)**3 - (nx**2) * (ny**3)
            if val <= 0:
                pixels[y*w + x] = RED
                
    write_png("hunter/assets/heart.png", w, h, pixels)

def create_wall():
    w, h = 40, 40
    pixels = [GRAY] * (w * h)
    
    # Outline
    draw_rect_outline(pixels, w, h, 0, 0, 40, 40, DARK_GRAY)
    
    # Brick pattern (horizontal lines)
    draw_line(pixels, w, h, 0, 20, 39, 20, DARK_GRAY)
    # Vertical lines
    draw_line(pixels, w, h, 20, 0, 20, 20, DARK_GRAY)
    draw_line(pixels, w, h, 10, 20, 10, 39, DARK_GRAY) # Staggered
    draw_line(pixels, w, h, 30, 20, 30, 39, DARK_GRAY) # Staggered
    
    write_png("sokoban/assets/wall.png", w, h, pixels)

def create_crate():
    w, h = 40, 40
    pixels = [BROWN] * (w * h)
    
    # Frame
    draw_rect_outline(pixels, w, h, 2, 2, 36, 36, DARK_BROWN)
    draw_rect_outline(pixels, w, h, 3, 3, 34, 34, DARK_BROWN) # Thicker
    
    # Cross
    draw_line(pixels, w, h, 2, 2, 37, 37, DARK_BROWN)
    draw_line(pixels, w, h, 3, 2, 37, 36, DARK_BROWN) # Thicker
    draw_line(pixels, w, h, 37, 2, 2, 37, DARK_BROWN)
    draw_line(pixels, w, h, 36, 2, 2, 36, DARK_BROWN) # Thicker
    
    write_png("sokoban/assets/crate.png", w, h, pixels)

def create_target():
    w, h = 40, 40
    pixels = [TRANSPARENT] * (w * h)
    
    cx, cy = 20, 20
    radius = 10
    
    for y in range(h):
        for x in range(w):
            dist = math.sqrt((x-cx)**2 + (y-cy)**2)
            if abs(dist - radius) < 2.0:
                 pixels[y*w + x] = RED
            elif dist < radius:
                 pixels[y*w + x] = (255, 100, 100, 100) # Semi transparent red
                 
    write_png("sokoban/assets/target.png", w, h, pixels)

def create_filled_target():
    # Green version of crate
    w, h = 40, 40
    BG_GREEN = (160, 160, 60, 255)
    LINE_GREEN = (50, 150, 50, 255)
    CHECK_GREEN = (50, 255, 50, 255)
    
    pixels = [BG_GREEN] * (w * h)
    
    # Frame
    draw_rect_outline(pixels, w, h, 2, 2, 36, 36, LINE_GREEN)
    draw_rect_outline(pixels, w, h, 3, 3, 34, 34, LINE_GREEN) 
    
    # Cross
    draw_line(pixels, w, h, 2, 2, 37, 37, LINE_GREEN)
    draw_line(pixels, w, h, 37, 2, 2, 37, LINE_GREEN)

    # Checkmark overlay
    # Simple check shape
    #      /
    #   \ /
    #    v
    draw_line(pixels, w, h, 10, 20, 18, 28, CHECK_GREEN)
    draw_line(pixels, w, h, 18, 28, 30, 10, CHECK_GREEN)
    # Thicken
    draw_line(pixels, w, h, 11, 20, 19, 28, CHECK_GREEN)
    draw_line(pixels, w, h, 19, 28, 31, 10, CHECK_GREEN)
    
    write_png("sokoban/assets/filled_target.png", w, h, pixels)

def create_player():
    w, h = 40, 40
    pixels = [TRANSPARENT] * (w * h)
    
    # Wizard Hat (Purple)
    # Triangle
    for y in range(5, 20):
        width = (y - 5)
        for x in range(20 - width, 20 + width + 1):
             pixels[y*w + x] = MAGENTA
             
    # Face (Cyan)
    fill_rect(pixels, w, h, 12, 20, 16, 12, CYAN)
             
    # Eyes
    pixels[24*w + 16] = BLACK
    pixels[24*w + 22] = BLACK
    
    write_png("sokoban/assets/player.png", w, h, pixels)

def draw_rect_outline(pixels, w, h, x, y, rw, rh, color):
    # Top
    draw_line(pixels, w, h, x, y, x+rw-1, y, color)
    # Bottom
    draw_line(pixels, w, h, x, y+rh-1, x+rw-1, y+rh-1, color)
    # Left
    draw_line(pixels, w, h, x, y, x, y+rh-1, color)
    # Right
    draw_line(pixels, w, h, x+rw-1, y, x+rw-1, y+rh-1, color)

if __name__ == "__main__":
    if not os.path.exists("hunter/assets"):
        os.makedirs("hunter/assets")
        
    create_wizard()
    create_ghost()
    create_demon()
    create_heart()

    if not os.path.exists("sokoban/assets"):
        os.makedirs("sokoban/assets")

    create_wall()
    create_crate()
    create_target()
    create_filled_target()
    create_player()
    
    print("Assets generated.")
