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
void producer(char*,int, int);
struct Data* get_data(FILE*,int);
void send_consumer(struct Data*, int);

int done = 0;

int main(int argc, char *argv[])
{
	int pid;

	if(argc != 3)
	{
    printf("Hacen falta dos argumentos: %s <file> <size_buffer>\n", argv[0]);
		exit(1);
	}

	char* filename = argv[1];
	int lines = atoi(argv[2]);

	pid = fork();

	if(pid==0) {
		consumer();
	}
	else
	{
		producer(filename,pid,lines);
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

void consumer()
{
	int fd;
	char filename[12];
	int buff_s = CONSUMER_BUFFER_SIZE, buff[CONSUMER_BUFFER_SIZE]; //TODO: Aixo s'hauria d'instanciar en el heap.
	int dataNum, i, maxRead, passenger_count = 0, trip_time_count = 0;
	int dataNumAux;

  printf("Consumidor pid: %d\n", getpid());

  signal(SIGUSR1, sigusr);
  signal(SIGTERM, end);

  sprintf(filename, "%d", getpid());
  fd = open(filename, O_CREAT | O_RDWR | O_SYNC | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP);

  while(!final){
      while(!dades_pendents && !final);
      
      dades_pendents = 0;
      read(fd, &dataNum, 4);
      dataNumAux = dataNum;
      dataNum *= 2;
      while(dataNum > 0){
          maxRead = dataNum < buff_s ? dataNum : buff_s;
          read(fd, &buff, maxRead*4);
          for(i = 0; i < maxRead-1; i+=2){
              passenger_count += buff[i];
              trip_time_count += buff[i+1];
          }
          dataNum -= maxRead;
      }
      printf("Consumer ha leido %d datos\n", dataNumAux);
    }

 	int result[2] = {passenger_count, trip_time_count};
    ftruncate(fd, 0);
    write(fd, result, 8);
 	close(fd);

	exit(0);
}

//-----------------------------------------------------------------------------------------------------

//--------------PRODUCER-------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


void producer(char* filedata, int pid,  int lines)
{
    FILE* file = fopen(filedata, "r");
    struct Data* d;
    char filename[12] = "";
    int i;
    int fd;
    int results[2];
    int readLines = 0;

    printf("Productor pid: %d\n",getpid());

    sprintf(filename, "%d", pid);
    fd = open(filename, O_CREAT | O_RDWR | O_SYNC | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);


    d = get_data(file,lines);
    while(d->total > 0)
    {
        printf("Productor ha leido %d datos\n", d->total);
        send_consumer(d,pid);
        kill(pid, SIGUSR1);
        readLines += d->total;
        d = get_data(file,lines);
    }

    kill(pid, SIGTERM);

    wait(NULL);

    float media_pasajeros = 0, media_tiempo_de_viaje = 0;

    read(fd, &results, 8);
    close(fd);
    sprintf(filename, "%d", pid);
    remove(filename);
    media_pasajeros = results[0];
    media_tiempo_de_viaje = results[1];

    media_pasajeros /= readLines;
    media_tiempo_de_viaje /= readLines;
    
    printf("TOTAL de lineas leidas: %d\n", readLines);
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
