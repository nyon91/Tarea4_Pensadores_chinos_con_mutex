#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>

//  Se crea una estructura
struct shared {
    pthread_mutex_t mutex;  //  semaforo de exlcusion mutua
    int palillos;           //  éste es el recurso compartido
};

int main()
{   
    //  shm_open abre un objeto de memoria compartida:
    //  int shm_open(const char *name, int oflag, mode_t mode);
    //  O_CREAT     si el objeto de memoria compartida existe, ésta bandera no tiene efecto. Sino, se crea el objeto de memoria compartido
    //  O_RDWR      abre para leer o crear accesos
    //  O_TRUNC     si el objeto de memoria compartida existe y lo abrió el modo O_RDWR, el objeto será truncado a longitud cero
    //  Si el proceso es exitoso, la función debe retornar un entero no-negativo representando el descriptor de archivo no utilizado con el número más bajo
    //  De lo contrario, retorna un -1 y fija "errno" para indicar un error
    int fd = shm_open("/foo", O_CREAT | O_TRUNC | O_RDWR, 0600);

    //  ftruncate trunca un archivo a un largo especificado:
    //  int ftruncate(int fd, off_t length);
    //  fd es el archivo
    //  el tamaño del archivo será el tamaño de la estructura
    ftruncate(fd, sizeof(struct shared));

    //  mmap es una función que mapea páginas de memoria
    //  void *mmap(void *addr, size_t len, int prot, int flags, int files, off_t off);
    //  PROT_READ   los datos pueden ser leídos
    //  PROT_WRITE  los datos pueden ser escritos
    //  MAP_SHARED  los cambios son compartidos
    //  Deberá retornar la dirección en la cual el mapeo fue colocado. De otra manera, retornará un error
    struct shared *p = (struct shared*)mmap(0, sizeof(struct shared),
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    //  El puntero "p" accede al valor de palillos
    p->palillos = 0;

    //  Asegurarse que puede ser compartido a través de procesos
    pthread_mutexattr_t shared;         //  objeto de atributos mutex
    pthread_mutexattr_init(&shared);    //  inicializa la dirección del objeto de atributos mutex
    pthread_mutexattr_setpshared(&shared, PTHREAD_PROCESS_SHARED);  //  la función establecerá el atributo de proceso compartido en un objeto de atributos

    pthread_mutex_init(&(p->mutex), &shared);   //  inicializa el mutex referenciado con los atributos que posee "&shared"

    int i;
    for (i = 0; i < 100; i++) {
        pthread_mutex_lock(&(p->mutex));    //  Se bloquea el objeto
        printf("%d\n", p->palillos);        //  El palillo lo tiene alguien
        pthread_mutex_unlock(&(p->mutex));  //  Desbloquea el objeto
        sleep(1);   //  Hace una pequeña pausa
    }

    munmap(p, sizeof(struct shared*));  //  "Desmapea" (elimina) el archivo de memoria para liberar recursos
    shm_unlink("/foo"); //  Elimina el objeto compartido de memoria
}