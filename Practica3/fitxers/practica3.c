#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>


#define CONSUMER_BUFFER_SIZE 256



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
	int parentPid = getpid();	
	
	pid = fork();

	/*
	 * TODO: Modificar (una vez funcione el resto) para crear mas de un consumidor
	 */
	
	if(pid==0) {
		consumer(parentPid);
	}
	else
	{
		pids[0] = pid;
		producer(filename,pids,consumers,lines);
	}

    return 0;
}


//--------------CONSUMER--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------

int dades_pendents = 0;
int final = 0;

void end(int s){
    final = 1;
}

void sigusr(int signe){
    dades_pendents = 1;
}

/**
*  Consumer llegeix el fitxer utilitzan un buffer a memoria per tal de solicitar menys crides read del sistema.
*
**/

void consumer(int parentPid)
{
	int fd;
	char filename[12];
	int buff_s = CONSUMER_BUFFER_SIZE, buff[CONSUMER_BUFFER_SIZE]; //TODO: Aixo s'hauria d'instanciar en el heap.
	int dataNum, i, maxRead, passenger_count = 0, trip_time_count = 0;
	int dataNumAux;

    signal(SIGUSR1, sigusr);
    signal(SIGTERM, end);
    
    sprintf(filename, "%d", getpid());
    fd = open(filename, O_CREAT | O_RDWR | O_SYNC | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP);

    while(!final){    
        while(!dades_pendents);
        dades_pendents = 0;
        read(fd, &dataNum, 4);
        //printf("\n================================\n===================================\n\n");
        dataNumAux = dataNum;
        dataNum *= 2;
        while(dataNum > 0){    
            maxRead = dataNum < buff_s ? dataNum : buff_s;
            //printf("maxRead: %d\n", maxRead);
            read(fd, &buff, maxRead*4);
            for(i = 0; i < maxRead-1; i+=2){
                //printf("passengers: %d, trip time: %d \n", buff[i], buff[i+1]);
                passenger_count += buff[i];
                trip_time_count += buff[i+1];
            }
            dataNum -= maxRead;
        }
        printf("Consumer ha leido %d datos\n", dataNumAux);
    }   
    
 	int result[2] = {passenger_count, trip_time_count};
 	write(fd, result, 8); 
 	 
 	close(fd);
    remove(filename);

	exit(0);
}

//-----------------------------------------------------------------------------------------------------

//--------------PRODUCER-------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


void producer(char* filedata, int* pids, int total_consumers, int lines)
{
    FILE* file = fopen(filedata, "r");
    struct Data* d;
    char filename[12] = "";
    int i;
    int fd[total_consumers];
    int results[total_consumers][2];
    for(i = 0; i < total_consumers; i++){
        sprintf(filename, "%d", pids[i]);
        fd[i] = open(filename, O_CREAT | O_RDWR | O_SYNC | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
    }    

    d = get_data(file,lines);
    while(d->total > 0)
    {		
        //printf("Productor ha leido %d datos\n", d->total);		
        
        send_consumer(d,pids[0]);
        kill(pids[0], SIGUSR1);	
        d = get_data(file,lines);
    }
    
    kill(pids[0], SIGTERM);

    for(i=0;i<total_consumers;i++)
        wait(NULL);

    float media_pasajeros = 0, media_tiempo_de_viaje = 0;
    int lineas = 0;
    
    for(i = 0; i < total_consumers; i++){
        lseek(fd[i], -2, SEEK_END);
        read(fd[i], &results[i], 8);
        close(fd[i]);
        sprintf(filename, "%d", pids[i]);
        remove(filename);
    }

    printf("TOTAL de lineas leidas: %d\n", lineas);
    printf("Media de pasajeros: %f - Media de tiempo de viaje: %f \n",media_pasajeros,media_tiempo_de_viaje);
}

//----------------------------------------------------------------------------------------------------------------------

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
    
    write(fd, data, l*4);

    close(fd);
}



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
