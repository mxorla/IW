//FUNCTIONS PROTOTYPES
void agregarUsuario(uint16_t idUsuario, int socketNbr);
int buscarGrupoUsuario(int aux);
int buscarPosicionUsuario(int aux);
int buscarPosicionPorSocket(int aux);
void publicarContenido(struct protocolo_t *msg);
void consultarContenido(int sd,struct protocolo_t *msg);
void consultarInformacionContenido(int sd,struct protocolo_t *msg);
int getPosContentArray();


//------------------------------------------------------------------------------
// Si el usuario no esta en el array lo agrega.
//------------------------------------------------------------------------------
void agregarUsuario(uint16_t idUsuario, int socketNbr) {
	int pos=0, userPos=0;

	userPos = buscarPosicionUsuario(idUsuario);
	//Si el usuario aun no fue agregado lo agrega
	if(userPos == -1) {
		pos = getPosUserArray();
		usersArray[pos].socketNumber = socketNbr; //agregarlo en la ultima pos disponible
		usersArray[pos].idUsuario = idUsuario;
	}
	//Si ya existe es una reconexion
	else {
		usersArray[userPos].socketNumber = socketNbr;	//Solo actualiza el socket
	}

}



//------------------------------------------------------------------------------
// Busca usuario por id, si lo encuentra retorna posicion en el array, sino -1.
//------------------------------------------------------------------------------
int buscarPosicionUsuario(int aux) {
	int i, ret=-1;

	for(i=0;i< MAX_CONNECTIONS;i++) {
		if(usersArray[i].idUsuario == aux) {
			ret = i;
			break;
		}
	}
	return ret;
}

//------------------------------------------------------------------------------
// Busca usuario por socket, si lo encuentra retorna posicion en el array, sino -1.
//------------------------------------------------------------------------------
int buscarPosicionPorSocket(int aux) {
	int i, ret=-1;

	for(i=0;i< MAX_CONNECTIONS;i++) {
		if(usersArray[i].socketNumber == aux) {
			ret = i;
			break;
		}
	}
	return ret;
}

int getPosUserArray()
{
	int i, ret=-1;
	for(i=0;i< MAX_CONNECTIONS;i++) {
		if(usersArray[i].socketNumber == 0 && usersArray[i].idUsuario == 0) {
			ret = i;
			break;
		}
	}
	return ret;
}

void publicarContenido(struct protocolo_t *msg)
{
	int i, j,act, posContent=0;
	data_t data;
	data.count = msg->MSG[0];
	act = 1;
	for(j=0; j <  data.count ;j++){
		posContent = getPosContentArray();

		data.det.lent = msg->MSG[act];
		act++;
		for(i=0; i <  data.det.lent;i++){
			data.det.title[i]=msg->MSG[i+act];
		}
		act += data.det.lent;
		data.det.lena=msg->MSG[act];
		act++;

		for(i=0; i <  data.det.lena;i++){
			data.det.aut[i]=msg->MSG[i+act];
		}
		act += data.det.lena;
		data.det.lend=msg->MSG[act];
		act++;

		for(i=0; i <  data.det.lend;i++){
			data.det.desc[i]=msg->MSG[i+act];
		}
		act += data.det.lend;

		contentsArray[posContent].id_content=posContent + 1;
		contentsArray[posContent].propietario=msg->ID_USER;
		contentsArray[posContent].det=data.det;
	}
}

int getPosContentArray()
{
	int i, ret=-1;
	for(i=0;i< MAX_CONTENTS;i++) {
		if(contentsArray[i].propietario == 0) {
			ret = i;
			break;
		}
	}
	return ret;
}

void consultarContenido(int sd,struct protocolo_t *msg)
{

	int i,j, cant=0, act;
	char dataMessage[200];
	msg->LEN=4;
	msg->ID_USER=(uint16_t) ~((unsigned int) sd);
	msg->TYPE=1;
	act = 1;
	for(j=0;j< MAX_CONTENTS;j++) {
		if(contentsArray[j].propietario == 0) {
			break;
		}
		cant++;

		dataMessage[act] = contentsArray[j].id_content;
		act++;

		dataMessage[act] = contentsArray[j].det.lent;
		act++;
		for(i=0; i <  contentsArray[j].det.lent;i++){
			dataMessage[i+act]=contentsArray[j].det.title[i];
		}
		act += contentsArray[j].det.lent;
		dataMessage[act]=contentsArray[j].det.lena;
		act++;

		for(i=0; i <  contentsArray[j].det.lena;i++){
			dataMessage[i+act]=contentsArray[j].det.aut[i];
		}
		act += contentsArray[j].det.lena;
		dataMessage[act]=contentsArray[j].det.lend;
		act++;

		for(i=0; i <  contentsArray[j].det.lend;i++){
			dataMessage[i+act]=contentsArray[j].det.desc[i];
		}
		act += contentsArray[j].det.lend ;



	}
	dataMessage[0] = cant;
	dataMessage[act]='\0';
	memcpy(msg->MSG, dataMessage, act);

	writeMsg(sd, msg);
}

void consultarInformacionContenido(int sd,struct protocolo_t *msg)
{
	char title[50];
	int j;
/*
	memcpy(title,msg->MSG, strlen(msg->MSG));

	for(j=0;j< MAX_CONTENTS;j++) {
		int i;
		for(i=0;i< strlen(title);i++) {
			if(contentsArray[j].det.title[i] != title[i]) {
				break;
			}
		}
	}

	msg->TYPE = 2;

	memcpy(msg->MSG,contentsArray[j].propietario, strlen(contentsArray[j].propietario));


	writeMsg(sd, msg);
*/

}






