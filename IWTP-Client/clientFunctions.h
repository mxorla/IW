//FUNCTIONS PROTOTYPES
void conectar(int sd, struct protocolo_t *msg);
void publicarContenido(int sd, uint8_t userIdAssigned, struct protocolo_t *msg);
void consultarContenido(int sd, uint8_t userIdAssigned, struct protocolo_t *msg);
void consultarInformacionContenido(int sd, uint8_t id, uint8_t userIdAssigned,
		struct protocolo_t *msg);
void desconectar(int sd, uint8_t userIdAssigned, struct protocolo_t *msg);
void bloquearContenido(int sd, struct protocolo_t *msg);
void desbloquearContenido(int sd, struct protocolo_t *msg);
void lookUpContent(int sd, struct protocolo_t *msg);
int buscarPosicionPorSocket(int aux);
void SendCliServerData(int sd, uint8_t userIdAssigned, prop_t propietario,
		struct protocolo_t *msg);
void guardarBuffer(struct protocolo_t *msg);
void solicitarFile(int sd, struct protocolo_t *msg);

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

void consultarInformacionContenido(int sd, uint8_t id, uint8_t userIdAssigned,
		struct protocolo_t *msg) {

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

void lookUpContent(int sd, struct protocolo_t *msg) {

	int i, j;

	uint8_t lenTitle = msg->MSG[0];
	char title[50];

	for (i = 0; i < lenTitle; i++) {
		title[i] = msg->MSG[i + 1];

	}

	char* folder = "/media/joaquin/Data/FUCK-ULTAD/IW/workspace/iw/IWTP-Client/";
	char * path = (char *) malloc(1 + strlen(folder) + strlen(title));
	strcpy(path, folder);
	strcat(path, title);

	FILE *fileptr;
	char *buffer;
	long filelen;

	fileptr = fopen(path, "rb"); // Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);         // Get the current byte offset in the file
	rewind(fileptr);                   // Jump back to the beginning of the file

	buffer = (char *) malloc((filelen + 1) * sizeof(char)); // Enough memory for file + \0

	fread(buffer, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr); // Close the file

	char destBuffer[1000];
	for (j = 0; j < sizeof(destBuffer); j++)
		destBuffer[j] = 0x00;

	memcpy(destBuffer, buffer, strlen(buffer));

	//TODO hacer streaming de archivo

	msg->LEN = strlen(destBuffer);
	msg->ID_USER = 1;
	msg->TYPE = 2;
	uint8_t length = strlen(title);
	msg->MSG[0] = length;
//	length += (sizeof(uint8_t) * sizeof(char));

	memcpy(msg->MSG + length, title, strlen(title));

	uint8_t lengthDestBuff = strlen(destBuffer);
	length++;
	msg->MSG[length] = lengthDestBuff;
	memcpy(msg->MSG + length, &lengthDestBuff, sizeof(uint8_t));
	length += (sizeof(uint8_t) * sizeof(char));

	memcpy(msg->MSG + length, destBuffer, strlen(destBuffer));

	length = (strlen(title) + lengthDestBuff + 2);
	msg->MSG[length] = '\0';
	writeMsg(sd, msg);

}

void SendCliServerData(int sd, uint8_t userIdAssigned, prop_t propietario,
		struct protocolo_t *msg) {

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

void guardarBuffer(struct protocolo_t *msg) {
	/*
	 char buffer[MSG_LEN - 2];
	 uint8_t sec = msg->ID_USER;
	 if (sec != -1) {
	 uint8_t len = msg->LEN;
	 for (i = 0; i < len; i++) {
	 buffer[i] = msg->MSG[i];
	 }
	 }
	 */
	int i;
	uint8_t lenTitle = msg->MSG[0];
	char title[50];

	for (i = 0; i < lenTitle; i++) {
		title[i] = msg->MSG[i + 1];

	}

	char* folder = "/media/joaquin/Data/FUCK-ULTAD/IW/workspace/iw/IWTP-Client/";
	char * path = (char *) malloc(1 + strlen(folder) + strlen(title));
	strcpy(path, folder);
	strcat(path, title);

	FILE *f = fopen(path, "wb");
	fwrite(msg->MSG + lenTitle + 2, sizeof(char),
			strlen(msg->MSG) - lenTitle - 2, f);
	fclose(f);
}

void solicitarFile(int sd, struct protocolo_t *msg) {

	msg->LEN = 5;
	msg->ID_USER = 1; //TODO:
	msg->TYPE = 1;

	//msg->MSG[0] = 'Y';
	//msg->MSG[1] = '\0';

	writeMsg(sd, msg);

}
