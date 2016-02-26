//FUNCTIONS PROTOTYPES
void agregarUsuario(struct sockaddr_in client, int socket);
int buscarGrupoUsuario(int aux);
int buscarPosicionUsuario(int aux);
void publicarContenido(struct protocolo_t *msg);
void consultarContenido(int sd, struct protocolo_t *msg);
void consultarInformacionContenido(int sd, struct protocolo_t *msg);
int getPosContentArray();
users_t getUserById(uint8_t userId);
void guardarCliServer(struct sockaddr_in client, int socket, struct protocolo_t *msg);
void deleteContentArray(uint8_t id, struct protocolo_t *msg);

//------------------------------------------------------------------------------
// Si el usuario no esta en el array lo agrega.
//------------------------------------------------------------------------------
void agregarUsuario(struct sockaddr_in client, int socket) {
	int userPos;

	userPos = buscarPosicionUsuario(socket);
	//Si el usuario aun no fue agregado lo agrega
	if (userPos == -1) {
		userPos = getPosUserArray();
		usersArray[userPos].idUsuario = socket;
	}
	usersArray[userPos].socketNumber = socket;	//Solo actualiza el socket
	strcpy(usersArray[userPos].address.ip, inet_ntoa(client.sin_addr));
	//usersArray[userPos].address.puerto = ntohs(client.sin_port); es el puerto del clienteCliente no del clienteServidor
}

void guardarCliServer(struct sockaddr_in client, int socket, struct protocolo_t *msg) {
	int userPos;

	userPos = buscarPosicionUsuario(socket);
	//Si el usuario aun no fue agregado lo agrega
	if (userPos == -1) {
		userPos = getPosUserArray();
		usersArray[userPos].idUsuario = socket;
	}
	usersArray[userPos].socketNumber = socket;	//Solo actualiza el socket
	strcpy(usersArray[userPos].address.ip, inet_ntoa(client.sin_addr));

	uint8_t lowPort = msg->MSG[0];
	uint8_t highPort = msg->MSG[1];

	uint16_t port = ((uint16_t) highPort << 8) | lowPort;

	usersArray[userPos].address.puerto = port;
}

//------------------------------------------------------------------------------
// Busca usuario por id, si lo encuentra retorna posicion en el array, sino -1.
//------------------------------------------------------------------------------
int buscarPosicionUsuario(int aux) {
	int i, ret = -1;

	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (usersArray[i].idUsuario == aux) {
			ret = i;
			break;
		}
	}
	return ret;
}

int getPosUserArray() {
	int i, ret = -1;
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (usersArray[i].socketNumber == 0 && usersArray[i].idUsuario == 0) {
			ret = i;
			break;
		}
	}
	return ret;
}

users_t getUserById(uint8_t userId) {
	int i;
	users_t user;
	user.idUsuario = -1;
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (usersArray[i].idUsuario == userId) {
			return usersArray[i];
		}
	}
	return user;
}

void publicarContenido(struct protocolo_t *msg) {
	int i, j, act, posContent = 0;
	data_t data;
	data.count = msg->MSG[0];
	act = 1;
	for (j = 0; j < data.count; j++) {
		posContent = getPosContentArray();

		data.det.lent = msg->MSG[act];
		act++;
		for (i = 0; i < data.det.lent; i++) {
			data.det.title[i] = msg->MSG[i + act];
		}
		act += data.det.lent;
		data.det.lena = msg->MSG[act];
		act++;

		for (i = 0; i < data.det.lena; i++) {
			data.det.aut[i] = msg->MSG[i + act];
		}
		act += data.det.lena;
		data.det.lend = msg->MSG[act];
		act++;

		for (i = 0; i < data.det.lend; i++) {
			data.det.desc[i] = msg->MSG[i + act];
		}
		act += data.det.lend;

		contentsArray[posContent].id_content = posContent + 1;
		contentsArray[posContent].propietario.id = msg->ID_USER;
		contentsArray[posContent].det = data.det;
	}
}

int getPosContentArray() {
	int i, ret = -1;
	for (i = 0; i < MAX_CONTENTS; i++) {
		if (contentsArray[i].propietario.id == 0) {
			ret = i;
			break;
		}
	}
	return ret;
}

void deleteContentArray(uint8_t id, struct protocolo_t *msg) {
	int i, ret = -1;
	for (i = 0; i < MAX_CONTENTS; i++) {
		if (contentsArray[i].propietario.id == id) {
			contentsArray[i].propietario.id = 0;
		}
	}
	msg->LEN = 4;
	msg->ID_USER = (uint8_t) id;
	msg->TYPE = 4;
	writeMsg(id, msg);
}

void consultarContenido(int sd, struct protocolo_t *msg) {

	int i, j, cant = 0, act;
	char dataMessage[200];
	msg->LEN = 4;
	msg->ID_USER = (uint8_t) sd;
	msg->TYPE = 1;
	act = 1;
	for (j = 0; j < MAX_CONTENTS; j++) {
		if (contentsArray[j].propietario.id == 0) {
			continue;
		}
		cant++;

		dataMessage[act] = contentsArray[j].id_content;
		act++;

		dataMessage[act] = contentsArray[j].det.lent;
		act++;
		for (i = 0; i < contentsArray[j].det.lent; i++) {
			dataMessage[i + act] = contentsArray[j].det.title[i];
		}
		act += contentsArray[j].det.lent;
		dataMessage[act] = contentsArray[j].det.lena;
		act++;

		for (i = 0; i < contentsArray[j].det.lena; i++) {
			dataMessage[i + act] = contentsArray[j].det.aut[i];
		}
		act += contentsArray[j].det.lena;
		dataMessage[act] = contentsArray[j].det.lend;
		act++;

		for (i = 0; i < contentsArray[j].det.lend; i++) {
			dataMessage[i + act] = contentsArray[j].det.desc[i];
		}
		act += contentsArray[j].det.lend;

	}
	dataMessage[0] = cant;
	dataMessage[act] = '\0';
	memcpy(msg->MSG, dataMessage, act);

	writeMsg(sd, msg);
}

void consultarInformacionContenido(int sd, struct protocolo_t *msg) {
	uint8_t id = msg->MSG[0];

	int i, j, cant = 0, act;
	char dataMessage[200];
	act = 1; //en 0 cantidad de contenidos
	msg->LEN = 4;
	msg->ID_USER = (uint8_t) sd;
	msg->TYPE = 3; //TODO: ver tipo

	for (j = 0; j < MAX_CONTENTS; j++) {
		if (contentsArray[j].id_content == id) {
			users_t user = getUserById(contentsArray[j].propietario.id);
			//Longitud de ip
			dataMessage[act] = strlen(user.address.ip);
			act++;

			//Secuencia de ip
			for (i = 0; i < strlen(user.address.ip); i++) {
				dataMessage[act] = user.address.ip[i];
				act++;
			}

			uint8_t lowPort = user.address.puerto & 0xff;
			uint8_t highPort = (user.address.puerto >> 8);
			dataMessage[act] = lowPort;
			act++;
			dataMessage[act] = highPort;
			act++;

			//Longitud del titulo
			dataMessage[act] = contentsArray[j].det.lent;
			act++;

			//Secuencia del titulo
			for (i = 0; i < contentsArray[j].det.lent; i++) {
				dataMessage[act] = contentsArray[j].det.title[i];
				act++;
			}

			break;
		}
	}

	dataMessage[0] = 1; // envia un solo contenido
	dataMessage[act] = '\0';

	memcpy(msg->MSG, dataMessage, act);

	writeMsg(sd, msg);
}

