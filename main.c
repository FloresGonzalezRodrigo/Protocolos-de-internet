#include <sys/socket.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include "pila.h"
#define BUFFER 1024

int main(void) {
    char* buffer=malloc(1024);
    Pila *pdest=NULL, *psource=NULL;
    char nombre_fichero[32];
    unsigned short ntramas_ip=0,ntramas_ipv6=0;
    int sockfd;
    ssize_t ttrama;
    unsigned short ntramas;
    // ETHernetHeaDeR
    struct ethhdr* trama;
    socklen_t  tdireccion=sizeof(struct sockaddr);
    // InterFaceREQuest
    struct ifreq nic;

    printf("Introduce el nombre de tu interfaz de red: ");
    // %15s LIMITA LA LONGITUD DE LA CADENA DE ENTRADA A 15 CARÁCTERES
    scanf("%15s",nic.ifr_name);

    printf("Introduce el número de tramas que deseas interceptar: ");
    scanf("%hu",&ntramas);

    sockfd=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(sockfd==-1) {
        perror("ERROR AL CREAR EL SOCKET");
        exit(EXIT_FAILURE);
    }

    // OBTENER LA INFORMACIÓN DE LAS TARJETAS
    // SIOCGIFFLAGS: Socket Input/Output Control Get InterFace Flags
    if(ioctl(sockfd,SIOCGIFFLAGS,&nic)==-1) {
        printf("Error al leer la información de la NIC\n");
        exit(EXIT_FAILURE);
    }

    // ifreq.ifr_flags: CONTIENE LAS BANDERAS ACTUALES DE CONFIGURACIÓN DE LA INTERFAZ DE RED
    // IFF_PROMISC: REPRESENTA A LA 1 BANDERA (LA DE MODO PROMISCUO)
    // AÑADIR LA BANDERA DE MODO PROMISCUO, | ASEGURA QUE NO SE MODIFIQUEN LAS DEMÁS BANDERAS YA CONFIGURADAS
    nic.ifr_flags|=IFF_PROMISC;

    // CONFIGURAR LAS TARJETAS EN MODO PROMISCUO
    // SIOCGIFFLAGS: Socket Input/Output Control Set InterFace Flags
    if(ioctl(sockfd,SIOCSIFFLAGS,&nic)==-1) {
        printf("Error al configurar en modo promiscuo\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while(ntramas) {
        ttrama=recv(sockfd,buffer,sizeof(struct ethhdr),0);
        if(ttrama==-1) {
            perror("ERROR EN RECVFROM");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        realloc(buffer,sizeof(buffer));
        trama=(struct ethhdr*)buffer;

        switch(ntohs(trama->h_proto)) {
            case ETH_P_IP:
                ntramas_ip++;
            break;
            case ETH_P_IPV6:
                ntramas_ipv6++;
            break;
            default:
                continue;
        }

        pdest=contar(pdest,trama->h_dest,ETH_ALEN);
        psource=contar(psource,trama->h_source,ETH_ALEN);

        printf("MAC fuente: %02x:%02x:%02x:%02x:%02x:%02x\n" , trama->h_source[0],trama->h_source[1],trama->h_source[2],trama->h_source[3],trama->h_source[4],trama->h_source[5]);
        printf("MAC destino: %02x:%02x:%02x:%02x:%02x:%02x\n" , trama->h_dest[0],trama->h_dest[1],trama->h_dest[2],trama->h_dest[3],trama->h_dest[4],trama->h_dest[5]);

        // NetworkTOHostShort: CONVIERTE UN VALOR DE 16 BITS BIG-ENDIAN (QUE ES EL ORDEN QUE USAN LAS REDES)
        //                     AL ORDEN DEL HOST
        printf("Protocolo: 0x%04x\n", htons(trama->h_proto));
        printf("Longitud de la trama: %zd\n", ttrama);

        // ETH_HLEN: LONGITUD DEL ENCABEZADO DE UNA TRAMA ETHERNET
        printf("Longitud de la carga útil: %zd\n\n", ttrama-ETH_HLEN);
        --ntramas;
    }
    close(sockfd);
    free(buffer);

    printf("Introduce el nombre del fichero dónde quieres guardar las estadisticas: ");
    scanf("%s",nombre_fichero);
    FILE* fichero=fopen(nombre_fichero,"w");
    fprintf(fichero,"Numero de tramas interceptadas: %hu\n", ntramas_ip + ntramas_ipv6);
    fprintf(fichero,"Numero de tramas ipv4: %hu\n",ntramas_ip);
    fprintf(fichero,"Numero de tramas ipv6: %hu\n",ntramas_ipv6);

    fprintf(fichero,"Direccion MAC destino | Numero de tramas\n");
    for(Pila* paux=pdest;paux;paux=paux->psig) {
        //h_dest y h_source son unsigned char*
        unsigned char* pdato=(unsigned char*)paux + sizeof(Pila);
        fprintf(fichero,"%02x:%02x:%02x:%02x:%02x:%02x     | %hu\n",pdato[0],pdato[1],pdato[2],pdato[3],pdato[4],pdato[5],paux->cuenta);
    }

    fprintf(fichero,"Direccion MAC fuente  | Numero de tramas\n");
    for(Pila* paux=psource;paux;paux=paux->psig) {
        unsigned char* pdato=(unsigned char*)paux + sizeof(Pila);
        fprintf(fichero,"%02x:%02x:%02x:%02x:%02x:%02x     | %hu\n",pdato[0],pdato[1],pdato[2],pdato[3],pdato[4],pdato[5],paux->cuenta);
    }

    destruir(pdest);
    destruir(psource);
    fclose(fichero);
    return 0;
}

