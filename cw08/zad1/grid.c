#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_t* threads;

typedef struct { //struktura przechowujaca:
    char* src; //src - foreground
    char* dst; //dst - background
    int col; //kolumna konkretnego threadu
    int row; //wiersz konkretnego threadu
} GridArgs;


char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void signal_handler(int signum) {} //pusty handler, nie konczymy programu tylko wznowimy thread

void update_grid() {
    for (int i = 0; i < grid_height; ++i) {
        for (int j = 0; j < grid_width; ++j) {
            pthread_kill(threads[i * grid_width + j], SIGUSR1); //zamiast odświezac kazda kratke bedziemy wysylac sygnal do kazdego z threadów, zeby sprawdzil swoje pole
        }
    }
}

void* update_grid_field(void* arg){ //funkcja wywolywana przez kazdy thread
    GridArgs* args = (GridArgs*) arg; //odczytujemy argumenty przekazywane przez wygodną strukturę
    int col = args->col;  //odczytuje dane do zmiennych
    int row = args->row;
    char* dst = args->dst;
    char* src = args->src;
    free(args); //zwalniam pamiec z args w celu unikniecia memory leakow
    while (1) { //petla aby wykonywac symulacje w nieskonczonosc
        dst[row * grid_width + col] = is_alive(row, col, src); //tak naprawde do backgroundu obliczamy pozycje z foregroundu zeby potem je podmienic
        pause(); //pauzujemy po obliczeniu dla pola konkretnego threada, bedzie on wznawiany po otrzymaniu sygnalu
    }
}

void create_threads(char* src, char *dst) { //zgodnie z wymogiem zadania chcielismy tworzyc watki tylko raz
    threads = malloc(sizeof(pthread_t) * grid_width * grid_height); //w threads przechowuje pthread_t wszystkich tworzonych threadów
    signal(SIGUSR1, signal_handler); //przypisuje obsluge sygnalu, zeby nie konczyl on programu

    for (int i = 0; i < grid_height; ++i) {
        for (int j = 0; j < grid_width; ++j) {
            GridArgs* args = malloc(sizeof(GridArgs)); //alokujemy pamiec na strukture, aby przekazac ja do naszych threadow
            args->src = src; //przypisujemy do niej okreslone wlasnosci
            args->dst = dst;
            args->row = i;
            args->col = j;
            int error = pthread_create(&threads[i * grid_width + j], NULL, update_grid_field, (void*) args); //tworzymy thread, kazdemu przekazujemy w ktorej jest kolumnie, wierszu oraz foreground i background
            if (error){ //w przypadku gdy nie utworzymy tego threada zadbamy o to, aby nie bylo memory leakow
                free(args);
            }
        }
    }
}
