//LIBRARIES INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

//CONSTANTS DEFINITIONS
#define P_SIZE 				sizeof(struct protocolo_t)
#define MSG_LEN				200
#define MAX_CONNECTIONS				10
#define MAX_CONTENTS			20

//STRUCTURE DEFINITIONS
struct protocolo_t {
	unsigned char LEN;
	uint8_t ID_USER;
	char TYPE; 	
	char MSG[MSG_LEN];  			//valor maximo 200 bytes
};

typedef struct {
	uint8_t idUsuario;
	int socketNumber;
}users_t;

typedef struct {
	uint8_t lent;
	char title[50];
	uint8_t lena;
	char aut[50];
	uint8_t lend;
	char desc[96];
}det_t;

typedef struct {
	uint8_t count;
	det_t det;
}data_t;



typedef struct {
	uint8_t id;
	char ip[15];
	char puerto[4];
}prop_t;

typedef struct {
uint8_t id_content;
prop_t propietario;
det_t det;
}content_t;



//VARs
users_t usersArray[MAX_CONNECTIONS];
content_t contentsArray[MAX_CONTENTS];
int currentPosition=0;

//FUNCTIONS PROTOTYPES
int readMsg(int sd, struct protocolo_t *msg);
int writeMsg(int sd, struct protocolo_t *msg);
data_t BytesToData(struct protocolo_t *msg);
void DataToBytes(data_t data, struct protocolo_t *msg);


