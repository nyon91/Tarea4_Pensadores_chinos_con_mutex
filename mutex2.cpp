#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <iomanip>  //  libreria de manipulacion de decimales
#include <unistd.h>
#include <term.h>
#include <sstream>
#include <pthread.h>
#include <ncurses.h>


using namespace std;

pthread_mutex_t palillos;
int cont=1, iteraciones=0;

struct Chino {
    int pensar;
    int comer;
};

void *brete(void* arg){
    struct Chino *data = (struct Chino *) arg;
    int valor;
    srand((unsigned) time (NULL));
    valor = rand() % 2;   //  funcion random entre 0 y 1    
    if (valor % 2 == 0){
        data->comer = 0;
        data->pensar = 1;
    }
    else if (valor % 2 != 0 && cont<=3){
        data->comer = 1;        
        data->pensar = 0;
        cont=cont+1;
    }
    else if (cont>3){
        data->comer = 0;
        data->pensar = 1;
    }
    else if (iteraciones>5){
        cont=1;
    }
    iteraciones=iteraciones+1;
    printf("comer = %d \n", data->comer);
    pthread_exit(NULL); 
}

const char* doubleToStr(double value){
    std::stringstream ss ;
    ss << value;
    const char* str = ss.str().c_str();
    return str;
}

int main(){   
    int N=6;
    pthread_t my_thread[N];
    struct Chino argArray[N];

    //  shm_open abre un objeto de memoria compartida:
    //  int shm_open(const char *name, int oflag, mode_t mode);
    //  O_CREAT     si el objeto de memoria compartida existe, ésta bandera no tiene efecto. Sino, se crea el objeto de memoria compartido
    //  O_RDWR      abre para leer o crear accesos
    //  O_TRUNC     si el objeto de memoria compartida existe y lo abrió el modo O_RDWR, el objeto será truncado a longitud cero
    //  Si el proceso es exitoso, la función debe retornar un entero no-negativo representando el descriptor de archivo no utilizado con el número más bajo
    //  De lo contrario, retorna un -1 y fija "errno" para indicar un error
    //int fd = shm_open("/foo", O_CREAT | O_TRUNC | O_RDWR, 0600);

    //  ftruncate trunca un archivo a un largo especificado:
    //  int ftruncate(int fd, off_t length);
    //  fd es el archivo
    //  el tamaño del archivo será el tamaño de la estructura
    //ftruncate(fd, sizeof(struct shared));

    //  mmap es una función que mapea páginas de memoria
    //  void *mmap(void *addr, size_t len, int prot, int flags, int files, off_t off);
    //  PROT_READ   los datos pueden ser leídos
    //  PROT_WRITE  los datos pueden ser escritos
    //  MAP_SHARED  los cambios son compartidos
    //  Deberá retornar la dirección en la cual el mapeo fue colocado. De otra manera, retornará un error
    //struct shared *p = mmap(0, sizeof(struct shared),
    //    PROT_READ, PROT_WRITE, MAP_SHARED, fd, 0);

    //  El puntero "p" accede al valor de palillos
    //p->palillos = 0;

    //  Asegurarse que puede ser compartido a través de procesos
    //pthread_mutexattr_t shared;         //  objeto de atributos mutex
    //pthread_mutexattr_init(&shared);    //  inicializa la dirección del objeto de atributos mutex
    //pthread_mutexattr_setpshared(&shared, PTHREAD_PROCESS_SHARED);  //  la función establecerá el atributo de proceso compartido en un objeto de atributos

    pthread_mutex_init(&palillos, NULL);   //  inicializa el mutex referenciado con los atributos que posee "&shared"
    
    int i;
    for (i = 1; i <= 6; i++) {
        int ret = pthread_create(&my_thread[i], NULL, &brete, (void*) &argArray[i]);    //  Crea un hilo y pasa por parametro el valor de la variable "pies"
        if (ret != 0){
            printf("Error: pthread_create() failed\n");
            exit(EXIT_FAILURE);  
        }
        pthread_mutex_lock(&palillos);    //  Se bloquea el objeto
        sleep(1);   //  Hace una pequeña pausa
        pthread_mutex_unlock(&palillos);  //  Desbloquea el objeto
        

    //munmap(p, sizeof(struct shared*));  //  "Desmapea" (elimina) el archivo de memoria para liberar recursos
    //shm_unlink("/foo"); //  Elimina el objeto compartido de memoria
    }

    char captura;
    bool ciclo=true;
    char *tecla;

    WINDOW *w;
    initscr ();
    noecho();

    move(2, 0);
    printw("Chino 1");

    move(4, 0);
    printw("Chino 2");

    move(6, 0);
    printw("Chino 3");

    move(8, 0);
    printw("Chino 4");

    move(10, 0);
    printw("Chino 5");

    move(12, 0);
    printw("Chino 6");

    while(ciclo==true){
        
        nodelay(stdscr, true);
        captura=getch();
        cbreak();

        if(captura==83 || captura==115){
            ciclo=false;
            tecla=&captura;
        }
        
        for (i = 1; i <= 6; i++){
            //const char* prog = doubleToStr(argArray[i].pensar);
            //const char* abc = doubleToStr(argArray[i].comer);
            
            if(argArray[i].pensar==1 && argArray[i].comer==0){
                move(2*i, 10);
                printw("Pensando");
                move(2*i,20);
                printw("-------");
            }
            else if(argArray[i].pensar==0 && argArray[i].comer==1){
                move(2*i, 10);
                printw("--------");
                move(2*i, 20);
                printw("Comiendo");
            }

        refresh();
        }

    }
    
    endwin ();

    pthread_exit(NULL);
}
