#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

typedef struct {
    int* v;
    int st;
    int dr;
    int nr_cautat;
} ThreadData;

int numar_aparitii = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* numarare(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    //cazul de baza: interval gol
    if (data->st > data->dr) {
        return NULL;
    }
    
    //cazul de baza: un singur element
    if (data->st == data->dr) {
        //daca elementul este cel cautat, incrementeaza contorul
        if (data->v[data->st] == data->nr_cautat) {
            //blocheaza mutex-ul pentru acces exclusiv
            if (pthread_mutex_lock(&mutex) != 0) {
                perror("eroare la blocarea mutex-ului");
                return NULL;
            }
            numar_aparitii++;
            //deblocheaza mutex-ul
            if (pthread_mutex_unlock(&mutex) != 0) {
                perror("eroare la deblocarea mutex-ului");
                return NULL;
            }
        }
        return NULL;
    }
    
    //divide et impera: imparte vectorul in doua
    int mij = (data->st + data->dr) / 2;
    
    //aloca memorie pentru datele thread-ului stang
    ThreadData* left = (ThreadData*)malloc(sizeof(ThreadData));
    if (left == NULL) {
        perror("eroare la alocarea memoriei pentru left");
        return NULL;
    }
    
    //aloca memorie pentru datele thread-ului drept
    ThreadData* right = (ThreadData*)malloc(sizeof(ThreadData));
    if (right == NULL) {
        perror("eroare la alocarea memoriei pentru right");
        free(left);
        return NULL;
    }
    
    //initializeaza datele pentru jumatatea stanga
    left->v = data->v;
    left->st = data->st;
    left->dr = mij;
    left->nr_cautat = data->nr_cautat;
    
    //initializeaza datele pentru jumatatea dreapta
    right->v = data->v;
    right->st = mij + 1;
    right->dr = data->dr;
    right->nr_cautat = data->nr_cautat;
    
    pthread_t t_left, t_right;
    
    //creeaza thread-ul pentru jumatatea stanga
    if (pthread_create(&t_left, NULL, numarare, left) != 0) {
        perror("eroare la crearea thread-ului stang");
        free(left);
        free(right);
        return NULL;
    }
    
    //creeaza thread-ul pentru jumatatea dreapta
    if (pthread_create(&t_right, NULL, numarare, right) != 0) {
        perror("eroare la crearea thread-ului drept");
        pthread_join(t_left, NULL); // asteapta thread-ul stang
        free(left);
        free(right);
        return NULL;
    }
    
    //asteapta terminarea thread-ului stang
    if (pthread_join(t_left, NULL) != 0) {
        perror("eroare la asteptarea thread-ului stang");
    }
    
    //asteapta terminarea thread-ului drept
    if (pthread_join(t_right, NULL) != 0) {
        perror("eroare la asteptarea thread-ului drept");
    }
    
    //dezaloca memoria pentru datele thread-urilor
    free(left);
    free(right);
    
    return NULL;
}

int main(int argc, char* argv[]) {
    //verifica daca sunt suficiente argumente in linia de comanda
    if (argc < 3) {
        fprintf(stderr, "utilizare: %s <numar_cautat> <elem1> <elem2> ... <elemN>\n", argv[0]);
        return 1;
    }
    
    //extrage numarul cautat din primul argument
    int nr_cautat = atoi(argv[1]);
    //calculeaza dimensiunea vectorului
    int n = argc - 2;
    
    //aloca memorie pentru vector
    int* v = (int*)malloc(n * sizeof(int));
    if (v == NULL) {
        perror("eroare la alocarea memoriei pentru vector");
        return 1;
    }
    
    //construieste vectorul din argumentele in linia de comanda
    for (int i = 0; i < n; i++) {
        v[i] = atoi(argv[i + 2]);
    }
    
    //aloca memorie pentru datele thread-ului principal
    ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
    if (data == NULL) {
        perror("eroare la alocarea memoriei pentru data");
        free(v);
        return 1;
    }
    
    //initializeaza datele pentru thread-ul principal
    data->v = v;
    data->st = 0;
    data->dr = n - 1;
    data->nr_cautat = nr_cautat;
    
    pthread_t t;
    
    //creeaza thread-ul principal care va gestiona procesul de numarare
    if (pthread_create(&t, NULL, numarare, data) != 0) {
        perror("eroare la crearea thread-ului principal");
        free(data);
        free(v);
        return 1;
    }
    
    //asteapta terminarea thread-ului principal
    if (pthread_join(t, NULL) != 0) {
        perror("eroare la asteptarea thread-ului principal");
    }
    
    //afiseaza rezultatul final
    printf("numarul %d apare de %d ori in vector\n", nr_cautat, numar_aparitii);
    
    //dezaloca memoria pentru data si vector
    free(data);
    free(v);
    
    //distruge mutex-ul
    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("eroare la distrugerea mutex-ului");
    }

    return 0;
}
