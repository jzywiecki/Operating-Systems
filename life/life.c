#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>


int main()
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	init_grid(foreground);
    create_threads(foreground, background); //tworzymy wszystkie thready

	while (true)
	{
		draw_grid(foreground); //rysujemy foreground
		usleep(500 * 1000);

		// Step simulation
		update_grid(); //update'ujemy nasz background na podstawie foregroundu

        //zależało mi, aby podmieniać zawartość pamięci, na którą wskazują wskaźniki
        char* tmp = malloc(grid_width * grid_height * sizeof(char)); //tworzymy miejsce w pamieci dla zmiennej pomocniczej
        memcpy(tmp, foreground, grid_width * grid_height * sizeof(char)); //kopiujemy tam foreground
        memcpy(foreground, background, grid_width * grid_height * sizeof(char)); //w miejsce foregroundu background
        memcpy(background, tmp, grid_width * grid_height * sizeof(char)); //w miejsce backgroundu foreground zapisany w tmp
        free(tmp); //zwalniamy pamiec uzyta do tej podmiany
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
