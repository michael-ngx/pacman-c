from PIL import Image


def map():
    with Image.open("newmap.png") as im:
        px = im.load()

    f = open("drawMap2.c", "w")
    f.write("void drawMap() {\n    int walls[10891] = {")
    c = 0
    pxs = []
    for i in range(320):
        for j in range(240):
            if bool(px[i, j]):
                c +=1
                pxs.append(str((j << 10) + (i << 1)))

    f.write(", ".join(pxs))
    f.write("};\n}")
    f.close()

def pacman():
    with Image.open("images/pac.png", "r") as im:
        px = im.load()
    
    f = open("py Generated Source/drawPac2.c", "w")
    f.write("void drawPacOpen(int x, int y, short int c1) {\n")
    f.write("    int open [9][9] = {\n")

    c = 0
    for i in range(9):
        pxs = []
        f.write("    {")
        for j in range(9):
            if (px[j, i] != 0):
                pxs.append("1")
            else: 
                pxs.append("0")

        f.write(", ".join(pxs))
        f.write("},\n")
    f.write("    }\n}")
    f.close()

#     with Image.open("pacClosed.png", "r") as im2:
#         px2 = im2.load()
#     c = 0
#     f.write("void drawPacClosed (int x, int y, short int c1) {\n")
#     f.write("    int closed[68] = {")
#     pxs = []
#     for i in range(9):
#         for j in range(9):
#             if (px2[i, j] != 0):
#                 pxs.append(f"((x+({i-4}) << 10) + (y-({j-4}) << 1))")
#                 c+=1
#                 # print(f"(x+({i-4}) << 10) + (y-({j-4}) << 1)")
#     f.write(", ".join(pxs))
#     f.write("\n    };\n")
#     s = """    for (int n = 0; n < 68; n++) {
#         *(short int *)(pixel_buffer_start + closed[n]) = c1;
#     }\n"""
#     f.write(s)
#     f.write("}\n")
#     s2 = """void drawPac(int x, int y, int clear, int c)
# {
#     short int c1;
#     if (clear == 1)
#     {
#         c1 = BLACK;
#     }
#     else
#     {
#         c1 = 0xff20;
#     }
#     bool face = ((c % 10) >= 5);

#     if (face || clear == 1)
#     {
#         drawPacClosed(x, y, c1);
#     }
#     else
#     {
#         drawPacOpen(x, y, c1);
#     }
# }"""
#     f.write(s2)
#     f.close()
pacman()

def graph():
    with Image.open("images/smallMap.png", "r") as im:
        px = im.load()
    
    print("white is barrier")
    print("-"*46)
    for j in range(19):
        print("|", end="")
        for i in range(22):
            print("  " if (px[i, j]) else "██", end="")
        print("|")
    print("-"*46)

    print("int graph[19][22] =  {") # maps to y, x
    for j in range(19):
        print("    {", end="")
        for i in range(22):
            print("2" if (px[i, j]) else "0", end=(", " if (i!=21) else ""))
        print("}", end=(", \n" if (j!=18) else "\n"))
    print("}")

    f = open("graph.c", "w")
    f.write("int graph[19][22] =  {\n")  # maps to y, x
    for j in range(19):
        f.write("    {")
        for i in range(22):
            f.write(str(px[i, j]) + (", " if (i != 21) else ""))
        f.write("}" + (", \n" if (j != 18) else "\n"))
    f.write("};")