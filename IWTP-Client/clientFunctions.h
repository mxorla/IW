
//FUNCTIONS PROTOTYPES
void conectar(int sd,struct protocolo_t *msg);
void publicarContenido(int sd, uint8_t userIdAssigned,struct protocolo_t *msg);
void consultarContenido(int sd, uint8_t userIdAssigned,struct protocolo_t *msg);
void consultarInformacionContenido(int sd,uint8_t id,uint8_t userIdAssigned, struct protocolo_t *msg);
void desconectar(int sd, uint8_t userIdAssigned,struct protocolo_t *msg);
void bloquearContenido(int sd, struct protocolo_t *msg);
void desbloquearContenido(int sd, struct protocolo_t *msg);
void lookUpContent(int sd, struct protocolo_t *msg);
int buscarPosicionPorSocket(int aux);


//------------------------------------------------------------------------------
// Conecta al servidor
//------------------------------------------------------------------------------
void conectar(int sd, struct protocolo_t *msg) {

	msg->LEN=4;
	msg->ID_USER= (uint8_t) sd;
	msg->TYPE=1;
	msg->MSG[0]='\0';

	writeMsg(sd, msg);
}

void publicarContenido(int sd, uint8_t userIdAssigned,struct protocolo_t *msg)
{
	    //msg->LEN=14;
	    msg->LEN=2;
		msg->ID_USER=userIdAssigned;
		msg->TYPE=2;
		data_t data;
		int i, act;
		char opt = 's';
        act = 1;
        i=1;
		while (act < 191 && (opt == 's' || opt =='S')){
				printf("Ingrese Titulo\r\n");
				scanf("%s", data.det.title);
				data.det.lent =  strlen(data.det.title);

				printf("Ingrese Autor\r\n");
				scanf("%s", data.det.aut);
				data.det.lena =  strlen(data.det.aut);

				printf("Ingrese Descripcion\r\n");
				scanf("%s", data.det.desc);
				scanf("%c",&opt); /* avance de línea */
				data.det.lend =  strlen(data.det.desc);

				data.count = (uint8_t) i;
				i++;

				DataToBytes(act, data,msg);

				act += data.det.lent + data.det.lena +data.det.lend +3;

				msg->LEN += strlen(msg->MSG);

				printf("Ingresar otro contenido? s/n \r\n");
				scanf("%c", &opt);

		}


		writeMsg(sd, msg);

}

void consultarContenido(int sd, uint8_t userIdAssigned,struct protocolo_t *msg)
{
	    msg->LEN=4;
		msg->ID_USER=userIdAssigned;
		msg->TYPE=3;
		msg->MSG[0]='\0';

		writeMsg(sd, msg);
}

void consultarInformacionContenido(int sd,uint8_t id, uint8_t userIdAssigned,struct protocolo_t *msg)
{



		msg->LEN= 5;
		msg->ID_USER=userIdAssigned;
		msg->TYPE=4;
		msg->MSG[0]=id;
		msg->MSG[1]='\0';

		writeMsg(sd, msg);
}

void desconectar(int sd, uint8_t userIdAssigned, struct protocolo_t *msg)
{
	    msg->LEN=4;
	    msg->ID_USER=userIdAssigned;
		msg->TYPE=5;


		writeMsg(sd, msg);
}

void bloquearContenido(int sd, struct protocolo_t *msg)
{
	    msg->LEN=18;
		msg->ID_USER=(uint16_t) ~((unsigned int) sd);
		msg->TYPE=6;
		msg->MSG[0]='I';
		msg->MSG[1]='d';
		msg->MSG[2]='.';
		msg->MSG[3]=' ';
		msg->MSG[4]='C';
		msg->MSG[5]='o';
		msg->MSG[6]='n';
		msg->MSG[7]='t';
		msg->MSG[8]='e';
		msg->MSG[9]='n';
		msg->MSG[10]='i';
		msg->MSG[11]='d';
		msg->MSG[12]='o';
		msg->MSG[13]='\0';

		writeMsg(sd, msg);
}

void desbloquearContenido(int sd, struct protocolo_t *msg)
{
	    msg->LEN=18;
		msg->ID_USER=(uint16_t) ~((unsigned int) sd);
		msg->TYPE=7;
		msg->MSG[0]='I';
		msg->MSG[1]='d';
		msg->MSG[2]='.';
		msg->MSG[3]=' ';
		msg->MSG[4]='C';
		msg->MSG[5]='o';
		msg->MSG[6]='n';
		msg->MSG[7]='t';
		msg->MSG[8]='e';
		msg->MSG[9]='n';
		msg->MSG[10]='i';
		msg->MSG[11]='d';
		msg->MSG[12]='o';
		msg->MSG[13]='\0';

		writeMsg(sd, msg);
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

void lookUpContent(int sd, struct protocolo_t *msg)
{
	/*
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("/home/joaquin/tmp/test.txt", "r");
	while ((read = getline(&line, &len, fp)) != -1) {
		printf("Retrieved line of length %zu :\n", read);
		printf("%s", line);
	}

	fclose(fp);
	if (line) {
		free(line);
	}
	*/
	//TODO hacer streaming de archivo
	writeMsg(sd, msg);

}



