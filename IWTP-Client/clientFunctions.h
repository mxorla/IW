
//FUNCTIONS PROTOTYPES
void conectar(int sd,struct protocolo_t *msg);
void publicarContenido(int sd, struct protocolo_t *msg);
void consultarContenido(int sd, struct protocolo_t *msg);
void consultarInformacionContenido(int sd, struct protocolo_t *msg);
void desconectar(int sd, struct protocolo_t *msg);
void bloquearContenido(int sd, struct protocolo_t *msg);
void desbloquearContenido(int sd, struct protocolo_t *msg);


//------------------------------------------------------------------------------
// Conecta al servidor
//------------------------------------------------------------------------------
void conectar(int sd, struct protocolo_t *msg) {

	msg->LEN=4;
	msg->ID_USER= (uint16_t) ~((unsigned int) sd);
	msg->TYPE=1;
	msg->MSG[0]='\0';

	writeMsg(sd, msg);
}

void publicarContenido(int sd, struct protocolo_t *msg)
{
	    //msg->LEN=14;
	    msg->LEN=2;
		msg->ID_USER=(uint16_t) ~((unsigned int) sd);
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

void consultarContenido(int sd, struct protocolo_t *msg)
{
	    msg->LEN=4;
		msg->ID_USER=(uint16_t) ~((unsigned int) sd);
		msg->TYPE=3;
		msg->MSG[0]='\0';

		writeMsg(sd, msg);
}

void consultarInformacionContenido(int sd, struct protocolo_t *msg)
{
		uint8_t id;
		msg->ID_USER=(uint16_t) ~((unsigned int) sd);
		msg->TYPE=4;

		printf("Ingrese id que desea obtener info\r\n");
							scanf("%d",&id);

		msg->LEN= 4 + sizeof(id);
		memcpy(msg->MSG, &id, sizeof(id));
		msg->MSG[sizeof(id)]='\0';

		writeMsg(sd, msg);
}

void desconectar(int sd, struct protocolo_t *msg)
{
	    msg->LEN=4;
		msg->ID_USER=(uint16_t) ~((unsigned int) sd);
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



