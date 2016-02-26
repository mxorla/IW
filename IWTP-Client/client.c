#include "commonDefs.h"
#include "commonFunctions.h"
#include "clientFunctions.h"
#include <netdb.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <unistd.h>

int sd;
//int sd2;
//int Clisd;
struct protocolo_t *msg;
char txBuf[P_SIZE];
//users_t user;
uint8_t userIdAssigned;
prop_t CliServer;

//struct sockaddr_in Cliservidor;
//int Clilon;
//struct hostent * Clih;

char *getcwd(char *buf, size_t size);

void printMenu() {
	//system("clear");
	printf("Menu\n");

	printf("1 -		Publicar Contenido\n");
	printf("2 -		Consultar Contenido\n");
	printf("3 -		Consultar Informacion Contenido\n");
	printf("4 -		Desconectar\n");

	printf("---------------------------------------------------------------------- \r\n");
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
				printf("	ID     |     Titulo     |     Autor     |     Descripcion \r\n");
				printf("---------------------------------------------------------------------- \r\n");
				cant = msg->MSG[0];
				for (j = 0; j < cant; j++) {
					de = BytesToData(&act, msg);

					printf("%d			%s           %s        %s", de.id_content, de.det.title, de.det.aut, de.det.desc);
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
				printf("---------------------------------------------------------------------- \r\n");

				de = BytesToDataIp(msg);
				printf("     %s                  %s:%d", de.det.title, de.propietario.ip, de.propietario.puerto);
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
				printf("Ya esta conectado con el servidor, su id de usuario es %d  \r\n", msg->ID_USER);
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

	fd_set copia, conjunto, copia2, conjunto2;

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
		if (childPID != 0) {	// Parent process- client
			if (argc < 2) {
				printf("Debe configurar el parametro correspondiente a la IP del servidor\n", argv[0]);
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
				printMenu();
				int opcion;
				scanf("%d", &opcion); //Leyendo opcion

				switch (opcion) {
				case 1: {
					publicarContenido(sd, userIdAssigned, msg);
					printf("> \r");
				}
					break;
				case 2: {
					consultarContenido(sd, userIdAssigned, msg);
					printf("> \r");
				}
					break;
				case 3: {
					uint8_t id;
					printf("Ingrese id que desea obtener info\r\n");
					scanf("%d", &id);
					consultarInformacionContenido(sd, id, userIdAssigned, msg);
					printf("> \r");
					break;
				}
				case 4: {
					desconectar(sd, userIdAssigned, msg);
					printf("> \r");
				}
					break;
				default: {
					printf("> \r");
				}
					break;
				}
				opcion = NULL;

			}
		} else {
			int listenfd = 0;
			int connfd = 0;
			struct sockaddr_in serv_addr;
			char sendBuff[1025];
			int numrv;
			char * path;

			listenfd = socket(AF_INET, SOCK_STREAM, 0);

			printf("Socket retrieve success in %s:%d\n", CliServer.ip, CliServer.puerto);

			memset(&serv_addr, '0', sizeof(serv_addr));
			memset(sendBuff, '0', sizeof(sendBuff));

			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = inet_addr(CliServer.ip);
			serv_addr.sin_port = htons(CliServer.puerto);

			bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));

			if (listen(listenfd, 10) == -1) {
				printf("Failed to listen\n");
				return -1;
			}

			FD_ZERO(&conjunto2);
			FD_SET(listenfd, &conjunto2);
			msg = (struct protocolo_t *) txBuf;
			int ok = 1;
			while (1) {
				copia2 = conjunto2;
				select(FD_SETSIZE, &copia2, NULL, NULL, NULL);

				if (FD_ISSET(listenfd, &copia2)) { // Recibe un cliente que se quiere conectar
					lon = sizeof(cliente);
					connfd = accept(listenfd, (struct sockaddr *) &cliente, &lon);
					//connfd = accept(listenfd,(struct sockaddr*) NULL, NULL);

					FD_SET(connfd, &conjunto2);

					//Responde q esta conectado
					msg->LEN = 14;
					//msg->ID_USER=(uint16_t) ~((unsigned int) sdc);
					msg->ID_USER = (uint8_t) connfd;
					msg->TYPE = 98;
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

					writeMsg(connfd, msg);
				} else {

					for (connfd = 1; connfd < FD_SETSIZE; connfd++) {
						if (FD_ISSET(connfd, &copia2) && (connfd != listenfd)) {
							msg = (struct protocolo_t *) buffer;
							if ((n = readMsg(connfd, msg)) > 0) {

								if (msg->TYPE == 44) {
									uint8_t lenTitle = msg->MSG[0];
									char title[50];

									for (int i = 0; i < lenTitle; i++) {
										title[i + 1] = '\0';
										title[i] = msg->MSG[i + 1];

									}

									char* folder = "/media/joaquin/Data/FUCK-ULTAD/IW/workspace/iw/IWTP-Client/Shared/";
									path = (char *) malloc(1 + strlen(folder) + strlen(title));
									strcpy(path, folder);
									strcat(path, title);

									msg->LEN = 14;
									//msg->ID_USER=(uint16_t) ~((unsigned int) sdc);
									msg->ID_USER = (uint8_t) connfd;
									msg->TYPE = 45;
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

									writeMsg(connfd, msg);
								} else {
									if (msg->TYPE == 55) {

										//______________________

										FILE *fp = fopen(path, "rb");
										if (fp == NULL) {
											printf("File opern error");
											ok = 0;
										}

										while (ok) {
											/*  256 bytes */
											unsigned char buff[256] = { 0 };
											int nread = fread(buff, 1, 256, fp);
											printf("Bytes leidos %d \n", nread);

											if (nread > 0) {
												printf("Enviando \n");
												write(connfd, buff, nread);
											}

											if (nread < 256) {
												if (feof(fp)) {
													printf("End of file\n");
													//close(connfd);
													//FD_CLR(sdc, &conjunto);
													ok = 0;
												}
												if (ferror(fp))
													printf("Error reading\n");
												//break;
											}
										}

										msg->LEN = 14;
										msg->ID_USER = (uint8_t) connfd;
										msg->TYPE = 88;
										msg->MSG[0] = '\0';

										writeMsg(connfd, msg);
										close(connfd);
										fclose(fp);

										sleep(1);
										//______________________

									}

								}
							}
							//Se cerro el socket
							else {
								//Cierra el socket cerrado en el otro extremo para que pueda ser reutilizado
								close(connfd);

								//Borra descriptor del set
								FD_CLR(connfd, &conjunto2);

								printf("El usuario se desconecto\n");

							}
						}
					}
				}
			}

		}
	} else { // fork failed
		printf("\n Fork failed, quitting!!!!!!\n");
		return 1;
	}

	//close(listenfd);
}

void *runRepro(void *data) {
	char * name = (char *) data;
	usleep(300);
	char * folder = "mplayer -vfm ffmpeg /media/joaquin/Data/FUCK-ULTAD/IW/workspace/iw/IWTP-Client/Shared/Recibidos/";

	char* command = (char *) malloc(1 + strlen(folder) + strlen(name));
	strcpy(command, folder);
	strcat(command, name);

	system(command);
}

void iniciarStreaming(content_t de, struct protocolo_t *msg) {
	pthread_t runReprothread;
	int sockfd = 0;
	int bytesReceived = 0;
	int interval = 30;
	char recvBuff[256];
	memset(recvBuff, '0', sizeof(recvBuff));
	struct sockaddr_in serv_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("\n Error : Could not create socket \n");
		exit(-1);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(de.propietario.puerto); // port
	serv_addr.sin_addr.s_addr = inet_addr(de.propietario.ip);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("\n Error : Connect Failed \n");
		exit(-1);
	}

	int act = 1;
	uint8_t posTitle = msg->MSG[1] + 4;
	uint8_t longTitle = msg->MSG[posTitle];
	posTitle++;
	act = posTitle;
	char bufferTitle[50];
	msg->MSG[0] = longTitle;
	bufferTitle[0] = longTitle;
	for (int i = 0; i < longTitle; i++) {
		bufferTitle[i + 1] = msg->MSG[act];
		act++;
	}
	bufferTitle[longTitle + 1] = '\0';

	int ok = 1;
	while (ok) {
//		if (readMsg(sockfd, msg) > 0 || (bytesReceived = read(sockfd, recvBuff, 256)) > 0) {
		if (readMsg(sockfd, msg) > 0) {
			if (msg->TYPE == 98) {
				printf("mensaje %s", msg->MSG);
				msg->LEN = 14;

				msg->ID_USER = (uint8_t) sockfd;
				msg->TYPE = 44;
				memcpy(msg->MSG, bufferTitle, longTitle + 2);

				writeMsg(sockfd, msg);
			}
			if (msg->TYPE == 88) {
				printf("mensaje fin");
				close(sockfd);
			}
			if (msg->TYPE == 45) {
				msg->LEN = 14;

				msg->ID_USER = (uint8_t) sockfd;
				msg->TYPE = 55;
				memcpy(msg->MSG, bufferTitle, longTitle + 2);

				writeMsg(sockfd, msg);

				char title[50];
				memcpy(title, bufferTitle + sizeof(char), longTitle + 2);

				char* folder = "/media/joaquin/Data/FUCK-ULTAD/IW/workspace/iw/IWTP-Client/Shared/Recibidos/";
				char* path = (char *) malloc(1 + strlen(folder) + strlen(title));
				strcpy(path, folder);
				strcat(path, title);

				FILE *fp;
				fp = fopen(path, "ab");
				if (NULL == fp) {
					perror("Error opening file");
					exit(-1);
				}

				/*  256 bytes */
				int flagThread = 1;
				while ((bytesReceived = read(sockfd, recvBuff, 256)) > 0) {
					printf("Bytes recibidos %d\n", bytesReceived);

					fwrite(recvBuff, 1, bytesReceived, fp);
					if (flagThread) {
						pthread_create(&runReprothread, NULL, runRepro, &title);
						flagThread = 0;
					}

				}

				if (bytesReceived < 0) {
					printf("\n Read Error \n");
				}

				ok = 0;
				fclose(fp);
				close(sockfd);
			}

			/*if (bytesReceived > 0) {


			 }
			 }*/
		}
	}
}

void loadConfiguration() {

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char cwd[1024];
	/*getcwd(cwd, sizeof(cwd));
	 PathFolder = cwd;

	 char* title = "config";

	 PathFolder = (char *) malloc(1 + strlen(cwd) +1);
	 strcpy(PathFolder, cwd);
	 strcat(PathFolder, "/");

	 strcpy(PathFolder,  "/media/joaquin/Data/FUCK-ULTAD/IW/workspace/iw/IWTP-Client/");

	 char* path = (char *) malloc(1 + strlen(PathFolder) + strlen(title));
	 strcpy(path, PathFolder);
	 strcat(path, title);
	 fp = fopen(path, "r");*/
	fp = fopen("/media/joaquin/Data/FUCK-ULTAD/IW/workspace/iw/IWTP-Client/config", "r");
	while ((read = getline(&line, &len, fp)) != -1) {

		if (strlen(line) > 6) {
			memcpy(CliServer.ip, line, strlen(line) - 1);
		} else {
			CliServer.puerto = (uint16_t) line;
		}
	}

	fclose(fp);
	if (line) {
		free(line);
	}

	/*CliServer.puerto = 4455; */
}
