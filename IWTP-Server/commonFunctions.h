int readMsg(int sd, struct protocolo_t *msg) {
	unsigned char totalLen, pos = 0, act=0;
	char auxBuf[512];
	char *ptr = 0;
	int j, bytes = 1, buffered = 0;

	for (j = 0; j < 512; j++)
		auxBuf[j] = 0x00;

	ptr = auxBuf;

	//Recibe el primer byte (longitud total del mensaje)
	if (recv(sd, ptr, 1, 0) == 0)//si recv retorna 0: indica que el socket fue cerrado
		return 0;

	//Evalua longitud total del mensaje
	totalLen = auxBuf[0];
	ptr++;
	pos++;

	//Se asegura de recibe lo que resta del mensaje
	while ((buffered < totalLen - 1) && (bytes > 0)) {
		bytes = recv(sd, ptr + buffered, totalLen - buffered, 0);
		buffered = buffered + bytes;
		if (bytes == 0)	//si recv retorna 0: indica que el socket fue cerrado
			return 0;
	}

	totalLen = auxBuf[0];
	memcpy(&msg->LEN, &auxBuf[act], sizeof(char));
	act += sizeof(char);
	memcpy(&msg->ID_USER, &auxBuf[act], sizeof(msg->ID_USER));
	act+= sizeof(msg->ID_USER);
	memcpy(&msg->TYPE, &auxBuf[act], sizeof(char));
	act+= sizeof(char);
	memcpy(&msg->MSG, &auxBuf[act], totalLen - 1);

	return totalLen;
}

//------------------------------------------------------------------------------
// Escribe un mensaje de aplicacion completo. Recibe en msg y lo copia
// al auxBuf. Retorna el resultado del send.
//------------------------------------------------------------------------------
int writeMsg(int sd, struct protocolo_t *msg) {
	char txBuf[P_SIZE];

	unsigned char typeLen, msgLen, totalLen, userIdLen;
	totalLen=0;
	//Calcula longitud del mensaje

	msgLen = strlen(msg->MSG);
	typeLen = sizeof(msg->TYPE);
	userIdLen = sizeof(msg->ID_USER);


	//Arma el mensaje de longitud variable en el buffer de transmisi�n

	totalLen++;
	memcpy(&txBuf[totalLen], &msg->ID_USER, sizeof(userIdLen));
	totalLen+=userIdLen;
	memcpy(&txBuf[totalLen], &msg->TYPE, typeLen);
	totalLen+=typeLen;
	memcpy(&txBuf[totalLen], &msg->MSG, msgLen);
	totalLen+=msgLen;
	txBuf[0] = totalLen;	//Escribe longitud en el buffer
	txBuf[totalLen] = '\0';									//Escribe fin

	return send(sd, txBuf, totalLen, 0);
}
//------------------------------------------------------------------------------

data_t BytesToData(struct protocolo_t *msg)
{
	data_t de;
	de.count = msg->MSG[0];


	int i,j, act;

	act = 1;
	for	(j=0; j< de.count;j++){
		//loop
		de.det.lent = msg->MSG[act];
		act++;
		for(i=0;i<de.det.lent;i++)
		{
			de.det.title[i] = msg->MSG[i+act];
		}
		act += de.det.lent;
		de.det.lena = msg->MSG[act];
		act++;
		for(i=0;i<de.det.lena;i++)
		{
			de.det.aut[i] = msg->MSG[i+act];
		}

		act += de.det.lena;
		de.det.lend = msg->MSG[act];
		act++;
		for(i=0;i<de.det.lend;i++)
		{
			de.det.desc[i] = msg->MSG[i+act];
		}
	}

	return de;
}


void DataToBytes(data_t data, struct protocolo_t *msg)
{
	int i, act;
	char dataMessage[200];
	dataMessage[0] = data.count;
	dataMessage[1] = data.det.lent;
	act=2;

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


