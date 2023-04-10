void drawPink(int x, int y) {
    int pink [9][9] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 3, 3, 3, 3, 3, 0, 0},
    {0, 3, 3, 5, 3, 3, 5, 3, 0},
    {0, 3, 5, 5, 1, 5, 5, 1, 0},
    {0, 3, 4, 5, 2, 4, 5, 2, 0},
    {0, 3, 3, 3, 3, 3, 3, 3, 0},
    {0, 3, 3, 3, 3, 3, 3, 3, 0},
    {0, 3, 3, 3, 0, 3, 3, 3, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    }

    for (int i=0; i<9; i++) {
        for (int j=0; j<9; j++) {
            if (pink[i][j] != 0){
                switch (pink[i][j]) {
                    case 1: c1 = BLUE; break;
                    case 2: c1 = PURPLE; break;
                    case 3: c1 = PINK; break;
                    case 4: c1 = COLOR_FOR_LIGHT_PINK; break;
                    case 5: c1 = WHITE; break;
                    default: ;
                }

                plot_pixel(j+x-4, i+y-4, c1);}
        }
    }
}
