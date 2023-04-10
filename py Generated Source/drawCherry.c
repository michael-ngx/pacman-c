void drawCherry(int x, int y, short int c1) {
    int cherry [9][9] = {
        {0, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 1, 1},
        {0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 2, 2, 0, 0, 0, 1, 0, 0},
        {2, 2, 2, 2, 0, 0, 1, 0, 0},
        {2, 2, 2, 2, 0, 2, 1, 0, 0},
        {0, 2, 2, 0, 2, 2, 2, 2, 0},
        {0, 0, 0, 0, 2, 3, 2, 2, 0},
        {0, 0, 0, 0, 0, 2, 2, 0, 0},
    };
    for (int i=0; i<9; i++) {
        for (int j=0; j<9; j++) {
            if (cherry[i][j] != 0){
                switch (cherry[i][j]) {
                    case 1: c1 = ORANGE; break;
                    case 2: c1 = RED; break;
                    case 3: c1 = WHITE; break;
                    default: ;
                }

                plot_pixel(j+x-4, i+y-4, c1);}
        }
    }
}
