#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>


bool reindeer_waiting = false; //czy renifery czekaja na mikolaja
int reindeer_count = 0; //licznik czekajacych reniferow
int delivering_gifts_count = 0; //ile razy dostarczalismy prezenty

pthread_mutex_t reindeer_count_mutex;
pthread_mutex_t reinder_waiting_mutex;

pthread_cond_t reindeer_count_cond;
pthread_cond_t reindeer_waiting_cond;


void* santa(void* arg){
    while (true){
        pthread_mutex_lock(&reindeer_count_mutex);
        while (reindeer_count < 9){ //do czasu jak nie zbierze sie 9 reniferow mikolaj spi, kiedy sie zbierze dostanie sygnal od 9tego renifera i sie obudzi
            pthread_cond_wait(&reindeer_count_cond, &reindeer_count_mutex);
        }
        pthread_mutex_unlock(&reindeer_count_mutex);

        printf("Mikołaj: budzę się\n");

        delivering_gifts_count += 1; //tu zliczamy ile razy mikolaj dowozil prezenty (zadanie ogranicza nas do 3 razy)
        printf("Mikołaj: dostarczam zabawki\n");
        sleep(rand() % 3 + 2); //mikolaj dostarcza zabawki

        pthread_mutex_lock(&reindeer_count_mutex);
        reindeer_count = 0; //zmniejszamy liczbe oczekujacych reniferow
        pthread_mutex_unlock(&reindeer_count_mutex);

        if (delivering_gifts_count == 3) { //jesli dostarczono 3 razy to konczymy program
            printf("Mikołaj: Prezenty dostarczone\n");
            return 0;
        }

        printf("Mikołaj: zasypiam\n");

        pthread_mutex_lock(&reinder_waiting_mutex);
        reindeer_waiting = false; //ustawiamy to, ze renifery juz nie oczekuja i nie pracuja, wiec beda one jechac na wakacje
        pthread_cond_broadcast(&reindeer_waiting_cond);
        pthread_mutex_unlock(&reinder_waiting_mutex);
    }
}


void* reindeer(void* arg){
    int id = *((int *) arg);
    while (true){
        sleep(rand() % 6 + 5); //wakacje reniferow

        pthread_mutex_lock(&reinder_waiting_mutex);
        reindeer_waiting = true; //pierwszy renifer ustawi, ze renifery czekają
        pthread_mutex_unlock(&reinder_waiting_mutex);

        pthread_mutex_lock(&reindeer_count_mutex);
        reindeer_count += 1; //zwiekszamy licznik reniferow
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", reindeer_count, id);
        if (reindeer_count == 9){ //jesli zbierze sie 9 reniferow to ostatni budzi mikolaja
            printf("Renifer: wybudzam Mikołaja, %d\n", id);

            pthread_cond_broadcast(&reindeer_count_cond); //wysylamy do mikolaja informacje, zeby sie obudzil
        }
        pthread_mutex_unlock(&reindeer_count_mutex);

        pthread_mutex_lock(&reinder_waiting_mutex);
        while(reindeer_waiting){ //wstrzymujemy renifery do czasu az nie wroca z rozwozenia prezentow
            pthread_cond_wait(&reindeer_waiting_cond, &reinder_waiting_mutex);
        }
        pthread_mutex_unlock(&reinder_waiting_mutex);
    }
}


int main(){
    pthread_mutex_init(&reindeer_count_mutex, NULL); //inicjujemy mutexy z domyślnymi ustawieniami
    pthread_mutex_init(&reinder_waiting_mutex, NULL);

    pthread_cond_init(&reindeer_count_cond, NULL); //inicjujemy warunki sprawdzające  z domyślnymi ustawieniami
    pthread_cond_init(&reindeer_waiting_cond, NULL);

    srand(time(NULL)); //inicjujemy generator liczb "losowych"

    pthread_t santa_t; //tworzymy wątek mikołaja
    pthread_create(&santa_t, NULL, &santa, NULL);

    pthread_t* reindeer_threads = calloc(9, sizeof(pthread_t)); //tworzymy wątki reniferów
    for (int i = 0; i < 9; i++){
        pthread_create(&reindeer_threads[i], NULL, &reindeer, &i);
    }

    pthread_join(santa_t, NULL); //czekamy aż wątek mikołaja skończy działanie (dostarczono prezenty 3 razy)

    return 0; //kończymy program
}
