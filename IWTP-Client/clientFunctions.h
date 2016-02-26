//FUNCTIONS PROTOTYPES
void conectar(int sd, struct protocolo_t *msg);
void publicarContenido(int sd, uint8_t userIdAssigned, struct protocolo_t *msg);
void consultarContenido(int sd, uint8_t userIdAssigned, struct protocolo_t *msg);
void consultarInformacionContenido(int sd, uint8_t id, uint8_t userIdAssigned, struct protocolo_t *msg);
void desconectar(int sd, uint8_t userIdAssigned, struct protocolo_t *msg);
void bloquearContenido(int sd, struct protocolo_t *msg);
void desbloquearContenido(int sd, struct protocolo_t *msg);
void lookUpContent(int sd, struct protocolo_t *msg);
int buscarPosicionPorSocket(int aux);
void SendCliServerData(int sd, uint8_t userIdAssigned, prop_t propietario, struct protocolo_t *msg);
void guardarBuffer(struct protocolo_t *msg);
void solicitarFile(int sd, struct protocolo_t *msg);
void *runRepro(void *data);
void iniciarStreaming(content_t de, struct protocolo_t *msg);

//------------------------------------------------------------------------------
// Conecta al servidor
//------------------------------------------------------------------------------
void conectar(int sd, struct protocolo_t *msg) {

	msg->LEN = 4;
	msg->ID_USER = (uint8_t) sd;
	msg->TYPE = 1;
	msg->MSG[0] = '\0';

	writeMsg(sd, msg);
}

void publicarContenido(int sd, uint8_t userIdAssigned, struct protocolo_t *msg) {
	//msg->LEN=14;
	msg->LEN = 2;
	msg->ID_USER = userIdAssigned;
	msg->TYPE = 2;
	data_t data;
	int i, act;
	char opt = 's';
	act = 1;
	i = 1;
	while (act < 191 && (opt == 's' || opt == 'S')) {
		printf("Ingrese Titulo\r\n");
		scanf("%s", data.det.title);
		data.det.lent = strlen(data.det.title);

		printf("Ingrese Autor\r\n");
		scanf("%s", data.det.aut);
		data.det.lena = strlen(data.det.aut);

		printf("Ingrese Descripcion\r\n");
		scanf("%s", data.det.desc);
		scanf("%c", &opt); /* avance de línea */
		data.det.lend = strlen(data.det.desc);

		data.count = (uint8_t) i;
		i++;

		DataToBytes(act, data, msg);

		act += data.det.lent + data.det.lena + data.det.lend + 3;

		msg->LEN += strlen(msg->MSG);

		printf("Ingresar otro contenido? s/n \r\n");
		scanf("%c", &opt);

	}

	writeMsg(sd, msg);

}

void consultarContenido(int sd, uint8_t userIdAssigned, struct protocolo_t *msg) {
	msg->LEN = 4;
	msg->ID_USER = userIdAssigned;
	msg->TYPE = 3;
	msg->MSG[0] = '\0';

	writeMsg(sd, msg);
}

void consultarInformacionContenido(int sd, uint8_t id, uint8_t userIdAssigned, struct protocolo_t *msg) {

	msg->LEN = 5;
	msg->ID_USER = userIdAssigned;
	msg->TYPE = 4;
	msg->MSG[0] = id;
	msg->MSG[1] = '\0';

	writeMsg(sd, msg);
}

void desconectar(int sd, uint8_t userIdAssigned, struct protocolo_t *msg) {
	msg->LEN = 4;
	msg->ID_USER = userIdAssigned;
	msg->TYPE = 5;

	writeMsg(sd, msg);
}

void bloquearContenido(int sd, struct protocolo_t *msg) {
	msg->LEN = 18;
	msg->ID_USER = (uint16_t) ~((unsigned int) sd);
	msg->TYPE = 6;
	msg->MSG[0] = 'I';
	msg->MSG[1] = 'd';
	msg->MSG[2] = '.';
	msg->MSG[3] = ' ';
	msg->MSG[4] = 'C';
	msg->MSG[5] = 'o';
	msg->MSG[6] = 'n';
	msg->MSG[7] = 't';
	msg->MSG[8] = 'e';
	msg->MSG[9] = 'n';
	msg->MSG[10] = 'i';
	msg->MSG[11] = 'd';
	msg->MSG[12] = 'o';
	msg->MSG[13] = '\0';

	writeMsg(sd, msg);
}

void desbloquearContenido(int sd, struct protocolo_t *msg) {
	msg->LEN = 18;
	msg->ID_USER = (uint16_t) ~((unsigned int) sd);
	msg->TYPE = 7;
	msg->MSG[0] = 'I';
	msg->MSG[1] = 'd';
	msg->MSG[2] = '.';
	msg->MSG[3] = ' ';
	msg->MSG[4] = 'C';
	msg->MSG[5] = 'o';
	msg->MSG[6] = 'n';
	msg->MSG[7] = 't';
	msg->MSG[8] = 'e';
	msg->MSG[9] = 'n';
	msg->MSG[10] = 'i';
	msg->MSG[11] = 'd';
	msg->MSG[12] = 'o';
	msg->MSG[13] = '\0';

	writeMsg(sd, msg);
}

//------------------------------------------------------------------------------
// Busca usuario por socket, si lo encuentra retorna posicion en el array, sino -1.
//------------------------------------------------------------------------------
int buscarPosicionPorSocket(int aux) {
	int i, ret = -1;

	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (usersArray[i].socketNumber == aux) {
			ret = i;
			break;
		}
	}
	return ret;
}

void SendCliServerData(int sd, uint8_t userIdAssigned, prop_t propietario, struct protocolo_t *msg) {

	msg->LEN = 5;
	msg->ID_USER = userIdAssigned;
	msg->TYPE = 99;

	uint8_t lowPort = propietario.puerto & 0xff;
	uint8_t highPort = (propietario.puerto >> 8);

	msg->MSG[0] = lowPort;
	msg->MSG[1] = highPort;

	msg->MSG[3] = '\0';

	writeMsg(sd, msg);
}

void solicitarFile(int sd, struct protocolo_t *msg) {

	msg->LEN = 5;
	msg->ID_USER = 1; //TODO:
	msg->TYPE = 1;

	//msg->MSG[0] = 'Y';
	//msg->MSG[1] = '\0';

	writeMsg(sd, msg);

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
		}
	}
}
