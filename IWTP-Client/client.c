#include "commonDefs.h"
#include "commonFunctions.h"
#include "clientFunctions.h"
#include <netdb.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

int sd;
int sd2;
int Clisd;
struct protocolo_t *msg;
char txBuf[P_SIZE];
users_t user;
uint8_t userIdAssigned;
prop_t CliServer;

struct sockaddr_in Cliservidor;
int Clilon;
struct hostent * Clih;
pthread_t Clithread;

void printMenu() {
    //system("clear");
	printf("Menu\n");

	printf("1 -		Publicar Contenido\n");
	printf("2 -		Consultar Contenido\n");
	printf("3 -		Consultar Informacion Contenido\n");
	printf("4 -		Desconectar\n");

	printf(
			"---------------------------------------------------------------------- \r\n");
}

//------------------------------------------------------------------------------
// chequear si hay mensajes. esto es por si algun usuario manda algo... no va
//------------------------------------------------------------------------------
void *checkConnections(void *data) {
	int interval = *(int *) data;
	int nro;
	content_t de;
	while (1) {
		if (readMsg(sd, msg) > 0) {

			//nro=0;

			switch (msg->TYPE) {
			case 1: {
				//printMenu();
				int j, cant, act = 1;
				printf(
						"	ID     |     Titulo     |     Autor     |     Descripcion \r\n");
				printf(
						"---------------------------------------------------------------------- \r\n");
				cant = msg->MSG[0];
				for (j = 0; j < cant; j++) {
					de = BytesToData(&act, msg);

					printf("%d			%s           %s        %s", de.id_content,
							de.det.title, de.det.aut, de.det.desc);
					printf("\r\n");

				}
				printf("> \r");
			}
				break;
			case 2: {
			}
				break;
			case 3: {
				printMenu();
				printf("Datos del contenido \r\n");
				printf("	 Titulo     |        Propietario IP:PUERTO \r\n");
				printf(
						"---------------------------------------------------------------------- \r\n");

				de = BytesToDataIp(msg);
				printf("     %s                  %s:%d", de.det.title,
						de.propietario.ip, de.propietario.puerto);
				printf("> \r");

				iniciarStreaming(de, msg);
				printMenu();

			}
				break;
			case 4: {
			}
				break;

			case 99: {
				userIdAssigned = msg->ID_USER;
				printf(
						"Ya esta conectado con el servidor, su id de usuario es %d  \r\n",
						msg->ID_USER);
				if (msg->LEN != 0) {
					printf("El Servidor Respondio --->   %s\n", msg->MSG);
				}

				SendCliServerData(sd, userIdAssigned, CliServer, msg);

			}
				break;
			default: {
				printf("Mensaje desconocido");
			}
			}

		}
		usleep(interval);
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int main(int argc, char *argv[]) {

	//Server Variables
	int n, sdc, nro;
	char buffer[P_SIZE];
	struct sockaddr_in cliente;

	fd_set copia, conjunto;

	int interval = 30;

	//Server and client variables
	struct sockaddr_in servidor;
	int lon;

	//Client Variables
	struct hostent *h;
	pthread_t thread;
	pid_t childPID;

	loadConfiguration();

	childPID = fork();

	if (childPID >= 0) {	// fork was successful
		if (childPID == 0) {	// Parent process- client
			if (argc < 2) {
				printf(
						"Debe configurar el parametro correspondiente a la IP del servidor\n",
						argv[0]);
				exit(-1);
			}

			sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			servidor.sin_family = AF_INET;
			servidor.sin_port = htons(4444);

			if (h = gethostbyname(argv[1])) {
				memcpy(&servidor.sin_addr, h->h_addr, h->h_length);
			}

			lon = sizeof(servidor);
			if (connect(sd, (struct sockaddr *) &servidor, lon) < 0) {
				perror("Error en connect");
				exit(-1);
			}

			msg = (struct protocolo_t *) txBuf;

			pthread_create(&thread, NULL, checkConnections, &interval);

			while (1) {
				int opcion;
				//system("clear");

				printMenu();

				switch (opcion) {
				case 1: {
					publicarContenido(sd, userIdAssigned, msg);
					printMenu();
					printf("> \r");
					scanf("%d", &opcion); //Leyendo opcion
				}
					break;
				case 2: {
					consultarContenido(sd, userIdAssigned, msg);

					printf("> \r");
					scanf("%d", &opcion); //Leyendo opcion
				}
					break;
				case 3: {
					uint8_t id;
					printf("Ingrese id que desea obtener info\r\n");
					scanf("%d", &id);
					consultarInformacionContenido(sd, id, userIdAssigned, msg);
					printf("> \r");
					scanf("%d", &opcion); //Leyendo opcion
					break;
				}
				case 4: {
					desconectar(sd, userIdAssigned, msg);
					printf("> \r");
					scanf("%d", &opcion); //Leyendo opcion
				}
					break;
				default: {
					printf("> \r");
					scanf("%d", &opcion); //Leyendo opcion
				}
					break;
				}

			}
		} else { //Child process  - Server
			printf("Este es otro proceso, para el server\n");

			printf("Servidor\r\n");

			servidor.sin_family = AF_INET;
			servidor.sin_port = htons(CliServer.puerto);
			//		servidor.sin_port = htons(4455);
			servidor.sin_addr.s_addr = INADDR_ANY;

			sd2 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (bind(sd2, (struct sockaddr *) &servidor, sizeof(servidor))
					< 0) {
				perror("Error en bind");
				exit(-1);
			}

			const int optVal = 0;
			const socklen_t optLen = sizeof(optVal);

			setsockopt(sd2, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);

			listen(sd2, 5);

			FD_ZERO(&conjunto);
			FD_SET(sd2, &conjunto);
			msg = (struct protocolo_t *) buffer;
			while (1) {
				copia = conjunto;
				select(FD_SETSIZE, &copia, NULL, NULL, NULL);

				if (FD_ISSET(sd2, &copia)) { // Recibe un cliente que se quiere conectar
					lon = sizeof(cliente);
					sdc = accept(sd2, (struct sockaddr *) &cliente, &lon);
					FD_SET(sdc, &conjunto);
					//Responde q esta conectado
					msg->LEN = 14;
					msg->ID_USER = (uint8_t) sdc;
					msg->TYPE = 8;
					msg->MSG[0] = 'C';
					msg->MSG[1] = 'o';
					msg->MSG[2] = 'n';
					msg->MSG[3] = 'e';
					msg->MSG[4] = 'c';
					msg->MSG[5] = 't';
					msg->MSG[6] = 'a';
					msg->MSG[7] = 'd';
					msg->MSG[8] = 'o';
					msg->MSG[9] = '\0';

					writeMsg(sdc, msg);
				} else {

					for (sdc = 1; sdc < FD_SETSIZE; sdc++) {
						if (FD_ISSET(sdc, &copia) && (sdc != sd2)) {
							msg = (struct protocolo_t *) buffer;
							if ((n = readMsg(sdc, msg)) > 0) {
								if (msg->TYPE == 1) {

									printf("Look up content");
									lookUpContent(sdc, msg);
								}

							}
							//Se cerro el socket
							else {
								//Cierra el socket cerrado en el otro extremo para que pueda ser reutilizado
								close(sdc);
								//Dado socket (sdc) obtiene numero
								nro = buscarPosicionPorSocket(sdc);
								//Lo marca como cerrado
								usersArray[nro].socketNumber = 0; // se le pone 0 porq al iniciarse el array arranca con 0 y se usa esa comparacion, antes era -1;
								//Borra descriptor del set
								FD_CLR(sdc, &conjunto);

								printf("El usuario se desconecto\n");

							}
						}
					}
				}
			}
			FD_CLR(sdc, &conjunto); //Borra descriptor del set
			for (nro = 0; nro < 8; nro++) {
				close(nro + 4);
			}
			close(sd2);
			return 0;
		}

	} else { // fork failed
		printf("\n Fork failed, quitting!!!!!!\n");
		return 1;
	}

	close(sd);
}

void iniciarStreaming(content_t de, struct protocolo_t *msg) {

	int i, act = 1, interval = 30;

	Clisd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	Cliservidor.sin_family = AF_INET;
	Cliservidor.sin_port = htons(de.propietario.puerto);

	if (Clih = gethostbyname(de.propietario.ip)) {
		memcpy(&Cliservidor.sin_addr, Clih->h_addr, Clih->h_length);
	}

	Clilon = sizeof(Cliservidor);
	if (connect(Clisd, (struct sockaddr *) &Cliservidor, Clilon) < 0) {
		perror("Error en connect");
		exit(-1);
	}

	uint8_t posTitle = msg->MSG[1] + 4;
	uint8_t longTitle = msg->MSG[posTitle];
	posTitle++;
	act=posTitle;
	char bufferTitle[50];
	msg->MSG[0]= longTitle;
	bufferTitle[0] = longTitle;
	for (i = 0; i < longTitle ; i++) {
		bufferTitle[i+1] = msg->MSG[act];
		act++;
	}
	bufferTitle[longTitle + 1] ='\0';

	msg = (struct protocolo_t *) txBuf;

	//pthread_create(&Clithread, NULL, checkConnectionsClientServer, &interval);
	int ok = 1;
	while (ok) {
		if (readMsg(Clisd, msg) > 0) {

			switch (msg->TYPE) {

			case 2: {
				printf("Iniciando Streaming \r\n");
				guardarBuffer(msg);
				close(Clisd);
				ok = 0;

			}
				break;
			default: {
				userIdAssigned = msg->ID_USER;
				printf(
						"Ya esta conectado con el clienteservidor, su id de usuario es %d  \r\n",
						msg->ID_USER);
				if (msg->LEN != 0) {
					printf("El Servidor Respondio --->   %s\n", msg->MSG);
					printf("Iniciando Conect Streaming \r\n");
				}
				memcpy(msg->MSG,bufferTitle,strlen(bufferTitle)+1);
				solicitarFile(Clisd, msg);

			}
			}

		}

	}

	//while (1) {

//	}
	close(Clisd);
}

void loadConfiguration() {

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	fp = fopen("/media/joaquin/Data/FUCK-ULTAD/IW/workspace/iw/IWTP-Client/config", "r");
	while ((read = getline(&line, &len, fp)) != -1) {

		CliServer.puerto = (uint16_t) line;
	}

	fclose(fp);
	if (line) {
		free(line);
	}

	/*CliServer.puerto = 4455; */
}
