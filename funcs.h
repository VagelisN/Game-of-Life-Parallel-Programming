int calculate_neighbours_nsew(char, int*, int**, int, int);
int calculate_neighbours_inside(int**, int i, int j, int arraysize);
int next_state(int neighbours,int dead_or_alive);
int calculate_neighbours_corners(int corner, int *arr1, int* arr2, int dead_or_alive ,int **current, int arraysize, int j);
void print_state(int** ,int ,int, int);
