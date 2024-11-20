#!/usr/bin/env python3

import argparse
import numpy
import os.path
from PIL import Image

def get_image(image_path):
    image = Image.open(image_path, "r")
    width, height = image.size
    pixel_values = list(image.getdata())
    if image.mode == "RGB":
        channels = 3
    elif image.mode == "RGBA":
        channels = 4
    elif image.mode == "L":
        channels = 1
    else:
        print("Unknown mode: %s" % image.mode)
        return None
    pixel_values = numpy.array(pixel_values).reshape((width, height, channels))
    return pixel_values

def rgb(r, g, b):
    return (((r&0xff)>>3)<<11)|(((g&0xff)>>2)<<5)|((b&0xff)>>3)

def rgba(r, g, b, a):
    r5 = int(r * 31 / 255)
    g5 = int(g * 31 / 255)
    b5 = int(b * 31 / 255)
    a1 = 1 # if a > 0 else 0
    return (r5 << 11) | (g5 << 6) | (b5 << 1) | a1

def rgba8(r, g, b, a):
    r8 = int(r)
    g8 = int(g)
    b8 = int(b)
    a8 = int(a)
    return (r8 << 24) | (g8 << 16) | (b8 << 8) | a8

parser = argparse.ArgumentParser("pixify")
parser.add_argument("file")
args = parser.parse_args()

image = get_image(args.file)

# print("%d x %d" % (len(image), len(image[0])))
filename, _ = os.path.splitext(args.file)
row = "unsigned short %s_bmp[] = {" % (filename)
for i in range(len(image)):
    for j in range(len(image[i])):
        r, g, b, a = image[i][j]
        row += "0x%x," % rgba(r,g,b,a)

print(row.rstrip(",") + "};")
