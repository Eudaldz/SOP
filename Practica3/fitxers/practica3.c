#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

struct Data {
    int* passenger_count;
    int* trip_time_in_secs;
    int total;
};

void consumer();
void producer(char*,int*, int, int);
struct Data* get_data(FILE*,int);
void send_consumer(struct Data*, int);

int done = 0;

int main(int argc, char *argv[])
{
	int pid;
	
	if(argc != 4)
	{
        printf("Hacen falta tres argumentos: %s <file> <num_consumers> <size_buffer>\n", argv[0]);
		exit(1);
	}
	
	char* filename = argv[1];
	int consumers = atoi(argv[2]);
	int lines = atoi(argv[3]);
	consumers = 1; /* forzamos un consumidor */
	int pids[consumers];	
	
	pid = fork();

	/*
	 * TODO: Modificar (una vez funcione el resto) para crear mas de un consumidor
	 */
	
	if(pid==0) {
		consumer();
	}
	else
	{
		pids[0] = pid;
		producer(filename,pids,consumers,lines);
	}

    return 0;
}

int main2(int argc, char*argv[]){
    char* filename = "data.csv";
    int consumers = 1;
    int pids = 0;
    int lines = 1000;
    producer(filename, &pids, consumers, lines);
    return 0;
}

int dades_pendents = 0;
int final = 0;

void destroy(int s){
    final = 1;
}

void sigusr(int signe){
    dades_pendents = 1;
}

void consumer()
{
	int fd;
	char filename[12];

    printf("Consumidor pid: %d\n", getpid());

    signal(SIGUSR1, sigusr);
    signal(SIGTERM, destroy);
    /*
     * TODO: preparar el codigo para recibir senyales del productor
     */
    while(!final){    
        while(!dades_pendents);
        dades_pendents = 0;
                
        // Abrimos un fichero con el nombre = pid para recibir los datos
        sprintf(filename, "%d", getpid());
        fd = open(filename, O_CREAT | O_RDWR | O_SYNC | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP);

        /*
        *  TODO: Bucle para leer y procesar los datos del fichero que envia el productor
        */
        int l = read(fd, &l, 4);
        printf("numero de linies per llegir: %d\n", l);
    }
    printf("\nEND consumer\n");    
    close(fd);
    remove(filename);

	/*
 	 * TODO: Codigo para entregar resultado parcial al productor a traves del mismo fichero
     *       que se ha utilizado para recibir datos.
 	 */	

	exit(0);
}

void producer(char* filedata, int* pids, int total_consumers, int lines)
{
    FILE* file = fopen(filedata, "r");
    struct Data* d;
    char filename[12] = "";
    int i, fd;

    printf("Productor pid: %d\n",getpid());	

    d = get_data(file,lines);
    while(d->total > 0)
    {		
        printf("Productor ha leido %d lineas\n", d->total);		
        
        send_consumer(d,pids[0]);
        kill(pids[0], SIGUSR1);
        /*
         * TODO: Codigo para notificar que hay datos a los consumidores (SIGUSR1)
         */			
        d = get_data(file,lines);
    }

    /*
     * TODO: Codigo para notificar que finalicen los consumidores (SIGTERM)
     */
    kill(pids[0], SIGTERM);

    // Espera que los consumidores terminen
    for(i=0;i<total_consumers;i++)
        wait(NULL);

    float media_pasajeros = 0, media_tiempo_de_viaje = 0;
    int lineas = 0;

    sprintf(filename, "%d", pids[0]);

    /*
     * TODO: Leer resultados parciales de los consumidores y calcular valores finales.
     * Borrar el fichero filename al finalizar.
     */
 
    //remove(filename);	

    printf("TOTAL de lineas leidas: %d\n", lineas);
    printf("Media de pasajeros: %f - Media de tiempo de viaje: %f \n",media_pasajeros,media_tiempo_de_viaje);
}

void send_consumer(struct Data* d, int consumer)
{
    int l = 2*d->total+1;
    int fd, data[l];
    char filename[12];
    sprintf(filename, "%d", consumer);
    fd = open(filename, O_CREAT | O_RDWR | O_SYNC | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP);

    data[0] = d->total;
    int i = 1;
    int j = 0;
    
    for(i = 1, j = 0; i < l; i+=2, j++){
        data[i] = d->passenger_count[j];
        data[i+1] = d->trip_time_in_secs[j];
    }
    
    write(fd, data, l);

    close(fd);
}

//

int get_column_int(char* line, int num)
{
    char new_line[256];
    char* tok;
    int retvalue;

    strncpy(new_line, line, 256);
    for (tok = strtok(new_line, ",\n"); tok; tok = strtok(NULL, ",\n"))
    {
        if (!--num) {
            retvalue = (int) strtol(tok, (char **)NULL, 10);
            return retvalue;
        }
    }
    
    printf("ERROR: get_column_int\n");
    exit(1); 
}

struct Data* get_data(FILE* file, int max)
{
    struct Data *ret;
    char line[256];
    int pos = 0;
    char *c_passenger_count, *c_trip_time_in_secs;

    ret = (struct Data*)malloc(sizeof(struct Data));
    ret->passenger_count = (int*)malloc(sizeof(int) * max);
    ret->trip_time_in_secs = (int*)malloc(sizeof(int) * max);

    if(ftell(file) == 0)
        fgets(line, sizeof(line), file);

    while (pos < max && fgets(line, sizeof(line), file))
    {
        ret->passenger_count[pos] = get_column_int(line, 8); 
        ret->trip_time_in_secs[pos] = get_column_int(line, 9); 
        pos++;
    }
    ret->total = pos;

    return ret;
}
