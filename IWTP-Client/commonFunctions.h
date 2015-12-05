//------------------------------------------------------------------------------
// Lee un mensaje de aplicacion completo. Recibe en auxBuf y lo copia a
// la estructura msg. Retorna la cantidad de bytes leidos o 0 si se cerro el socket.
//------------------------------------------------------------------------------
int readMsg(int sd, struct protocolo_t *msg) {
	unsigned char totalLen, pos=0, userPos, groupPos, msgPos;
	char auxBuf[512];
	char *ptr=0;
	int j, counter=0, bytes=1, buffered=0;	
		
	for(j=0;j<512;j++)
		auxBuf[j] = 0x00;
		
	ptr = auxBuf;
	
	//Recibe el primer byte (longitud total del mensaje)
	if(recv(sd, ptr, 1, 0)==0)	//si recv retorna 0: indica que el socket fue cerrado
		return 0;

	//Evalua longitud total del mensaje
	totalLen = auxBuf[0];
	ptr++;
	pos++;
		
	//Se asegura de recibe lo que resta del mensaje
	while ((buffered < totalLen - 1) && (bytes > 0)) {
		bytes = recv(sd, ptr + buffered , totalLen - buffered, 0);
		buffered = buffered + bytes;
		if(bytes == 0)			//si recv retorna 0: indica que el socket fue cerrado
			return 0;
	}
	
		
	totalLen = auxBuf[0];
	memcpy(&msg->LEN, &auxBuf[0], sizeof(char));
	memcpy(&msg->ID_USER, &auxBuf[1], sizeof(msg->ID_USER));
	memcpy(&msg->TYPE, &auxBuf[2], sizeof(char));

	memcpy(&msg->MSG, &auxBuf[3],totalLen-1);
		
    return totalLen;
}

//------------------------------------------------------------------------------
// Escribe un mensaje de aplicacion completo. Recibe en msg y lo copia 
// al auxBuf. Retorna el resultado del send.
//------------------------------------------------------------------------------
int writeMsg(int sd, struct protocolo_t *msg) {
	char txBuf[P_SIZE];
	unsigned char typeLen, msgLen, totalLen, idUserLength;
	
	//Calcula longitud del mensaje

	msgLen = strlen(msg->MSG);

	totalLen = 4 +  msgLen;
	idUserLength=sizeof(msg->ID_USER);
		
	//Arma el mensaje de longitud variable en el buffer de transmisi�n
		txBuf[0] = totalLen;											//Escribe longitud en el buffer
		memcpy(&txBuf[1], &msg->ID_USER, idUserLength);
		memcpy(&txBuf[2], &msg->TYPE, sizeof(char));
		memcpy(&txBuf[3], &msg->MSG, msgLen);
		txBuf[totalLen+1] = '\0';										//Escribe fin


	return send(sd, txBuf, totalLen, 0);
}


data_t BytesToData(int *act,struct protocolo_t *msg)
{
	data_t de;

	int i,j;


		de.det.lent = msg->MSG[*act];
		*act= *act +1;
		for(i=0;i<de.det.lent;i++)
		{
			de.det.title[i] = msg->MSG[i+*act];
		}
		*act = *act + de.det.lent;
		de.det.lena = msg->MSG[*act];
		*act= *act +1;
		for(i=0;i<de.det.lena;i++)
		{
			de.det.aut[i] = msg->MSG[i+*act];
		}

		*act  = *act + de.det.lena;
		de.det.lend = msg->MSG[*act];
		*act= *act +1;
		for(i=0;i<de.det.lend;i++)
		{
			de.det.desc[i] = msg->MSG[i+*act];
		}
		*act  = *act +  de.det.lend;
		*act= *act +1;


	return de;
}


void DataToBytes(int act, data_t data, struct protocolo_t *msg)
{
	int i;
	char dataMessage[200];
	dataMessage[0] = data.count;
	dataMessage[act] = data.det.lent;
	act += 1;

	for(i=0; i <  data.det.lent;i++){
		dataMessage[i+act]=data.det.title[i];
	}
	act += data.det.lent;
	dataMessage[act]=data.det.lena;
	act++;

	for(i=0; i <  data.det.lena;i++){
		dataMessage[i+act]=data.det.aut[i];
	}
	act += data.det.lena;
	dataMessage[act]=data.det.lend;
	act++;

	for(i=0; i <  data.det.lend;i++){
		dataMessage[i+act]=data.det.desc[i];
	}
	act += data.det.lend ;
	dataMessage[act]='\0';

	memcpy(msg->MSG, dataMessage, act);
}


