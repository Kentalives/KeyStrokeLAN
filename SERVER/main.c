#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>

//#include <winsock.h>
#include <winsock2.h>
//#include <ws2ipdef.h>
//#include <windows.h>
#include <ws2tcpip.h>


#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>


#define BUFFER_SIZE 1000

typedef struct{
    DWORD vkCode;
    WPARAM pulse;
}pulseInfo;




/*
 * ARGUMENTOS: nombre puerto
 */
int main(int argc, char * argv[]){   //Puertos a usar por encima del 5000
    //Cliente facil "telnet IP Puerto"

    SOCKET socketS, socketC;
    int rec;
    socklen_t tamano = sizeof(struct sockaddr_in);
    uint16_t puerto;
    char IP[INET_ADDRSTRLEN];

    struct sockaddr_in saddrS, saddrC;

    WSADATA wsaData;
    WORD nVersionRequired = MAKEWORD(2, 2);


    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    pulseInfo pulse;

    WSAStartup(nVersionRequired, &wsaData);

    if(argc != 2){
        perror("ERROR: Llamada a la función sin argumentos suficientes: Añade como argumento un número de puerto en el que escuchar peticiones\n");
        printf("%d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }


    //Crear el SOCKET
    if((socketS =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        perror("No se puedo crear el socket\n");
        printf("%d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }


    //Asignar valores al STRUCT del SOCKET
    saddrS.sin_family = AF_INET;
    saddrS.sin_addr.s_addr = htonl(INADDR_ANY);


    sscanf(argv[1], "%hu", &puerto);
    saddrS.sin_port = htons(puerto);


    //Hacemos BIND()
    if(bind(socketS, (struct sockaddr *) &saddrS, sizeof(struct sockaddr_in)) == -1){
        closesocket(socketS);
        perror("No se pudo asignar dirección\n");
        printf("%d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }


    //Hacemos LISTEN()
    if(listen(socketS, 4) == -1){
        closesocket(socketS);
        perror("No se pudo marcar como pasivo el socket\n");
        printf("%d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("Server listo, esperando conexión (Puerto %hu)...\n", puerto);

    //Hacemos el ACCEPT() de un cliente

    if((socketC = accept(socketS, (struct sockaddr *) &saddrC, &tamano)) == -1){
        closesocket(socketC);
        closesocket(socketS);
        perror("No se pudo aceptar la conexión\n");
        printf("%d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }


    //saddrC.sin_addr.s_addr = ntohl(saddrC.sin_addr.s_addr);    Nota: No hace falta porque el inet_ntop asume que esta en orden de red
    //Obetenemos el PUERTO E IP del cliente para imprimir

    puerto = ntohs(saddrC.sin_port);
    if((inet_ntop(AF_INET, (const void *) &saddrC.sin_addr, IP, INET_ADDRSTRLEN) == NULL)){
        closesocket(socketC);
        closesocket(socketS);
        perror("ERROR de conversión a string de IP\n");
        printf("%d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("IP conectada: %s\nPuerto conectado: %hu\n", IP, puerto);

    printf("Esperando datos...\n");

    //Hacemos el SEND() con el mensaje
    while(1){
        if((rec = recv(socketC, (char*)(&pulse), sizeof(pulseInfo), 0)) == -1){
            perror("No se pudo recibir\n");
            printf("%d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        //printf("Nº de bytes recibidos: %d\n", rec);
        //printf("Tecla: %s | pulsada\n", buffer);

        //Simular la pulsación entrante
        input.ki.wVk = pulse.vkCode;
        switch (pulse.pulse) {
            case WM_KEYDOWN:
                input.ki.dwFlags = 0;
                break;
            case WM_KEYUP:
                input.ki.dwFlags = KEYEVENTF_KEYUP;
                break;
            default:
                printf("ERROR: Tipo de envío no detectado\n");
        }

        SendInput(1, &input, sizeof(INPUT));
    }

    closesocket(socketC);
    closesocket(socketS);
}


///CODIGO THREADS
//#define PARALLEL_KEY_ENTRIES_COUNT 6

/*
 * typedef struct{
    SOCKET socketC;
    INPUT input;
}args;


int totalParallelKeyEtries = 0;

void *keyThread(void * arg){

    args arguments = *((args*)arg);
    int rec;
    char buffer[BUFFER_SIZE];

    printf("Esperando datos...\n");
    while(1){
        if((rec = recv(arguments.socketC, buffer, BUFFER_SIZE, 0)) == -1){
            perror("No se pudo recibir\n");
            printf("%d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        //printf("Nº de bytes recibidos: %d\n", rec);
        printf("Tecla: %s | pulsada\n", buffer);
        //printf("%s\n", buffer);

        //Simular pulsar D
        arguments.input.ki.wVk = (WORD)buffer[0];
        arguments.input.ki.dwFlags = 0;
        SendInput(1, &(arguments.input), sizeof(INPUT));

    }
    closesocket(arguments.socketC);
    --totalParallelKeyEtries;
}
 */
//Aqui se hace el connect() del cliente

//pthread_t TPID[PARALLEL_KEY_ENTRIES_COUNT];
/*
 * //Bucle de clientes secuenciales
while(totalParallelKeyEtries < PARALLEL_KEY_ENTRIES_COUNT){

    //Hacemos el ACCEPT() de un cliente

    if((socketC = accept(socketS, (struct sockaddr *) &saddrC, &tamano)) == -1){
        closesocket(socketC);
        closesocket(socketS);
        perror("No se pudo aceptar la conexión\n");
        printf("%d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }


    //saddrC.sin_addr.s_addr = ntohl(saddrC.sin_addr.s_addr);    Nota: No hace falta porque el inet_ntop asume que esta en orden de red
    //Obetenemos el PUERTO E IP del cliente para imprimir

    puerto = ntohs(saddrC.sin_port);
    if((inet_ntop(AF_INET, (const void *) &saddrC.sin_addr, IP, INET_ADDRSTRLEN) == NULL)){
        closesocket(socketC);
        closesocket(socketS);
        perror("ERROR de conversión a string de IP\n");
        printf("%d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("IP conectada: %s\nPuerto conectado: %hu\n", IP, puerto);


    args arg;
    arg.socketC = socketC;
    arg.input = input;

    /*
     *pthread_create(&(TPID[totalParallelKeyEtries]), NULL, &keyThread, &arg);
    ++totalParallelKeyEtries;

    closesocket(socketC);



printf("Esperando datos...\n");

//Hacemos el SEND() con el mensaje
while(1){
    if((rec = recv(socketC, buffer, BUFFER_SIZE, 0)) == -1){
        perror("No se pudo recibir\n");
        printf("%d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    //printf("Nº de bytes recibidos: %d\n", rec);
    printf("Tecla: %s | pulsada\n", buffer);
    //printf("%s\n", buffer);

    //Simular pulsar D
    input.ki.wVk = (WORD)buffer[0];
    input.ki.dwFlags = 0;
    SendInput(1, &input, sizeof(INPUT));

}
/*
 *

}
for(int i = 0 ; i < PARALLEL_KEY_ENTRIES_COUNT ; ++i){
pthread_join(TPID[i], NULL);
}
 */