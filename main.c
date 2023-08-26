/* 
Network Working Group                                           M. Leech
Request for Comments: 1928                    Bell-Northern Research Ltd
Category: Standards Track                                       M. Ganis
                                         International Business Machines
                                                                  Y. Lee
                                                  NEC Systems Laboratory
                                                                R. Kuris
                                                       Unify Corporation
                                                               D. Koblas
                                                  Independent Consultant
                                                                L. Jones
                                                 Hewlett-Packard Company
                                                              March 1996


                        SOCKS Protocol Version 5

								RFC1928

Status of this Memo

   This document specifies an Internet standards track protocol for the
   Internet community, and requests discussion and suggestions for
   improvements.  Please refer to the current edition of the "Internet
   Official Protocol Standards" (STD 1) for the standardization state
   and status of this protocol.  Distribution of this memo is unlimited.

Acknowledgments

   This memo describes a protocol that is an evolution of the previous
   version of the protocol, version 4 [1]. This new protocol stems from
   active discussions and prototype implementations.  The key
   contributors are: Marcus Leech: Bell-Northern Research, David Koblas:
   Independent Consultant, Ying-Da Lee: NEC Systems Laboratory, LaMont
   Jones: Hewlett-Packard Company, Ron Kuris: Unify Corporation, Matt
   Ganis: International Business Machines.
   
 */


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* CONFIG */
#define PROXY_SERVER_IP "174.77.111.196"
#define PROXY_SERVER_PORT 4145

#define DST_SERVER_IP "142.250.207.100"
#define DST_SERVER_PORT 80


/* MAIN */
int main(){
	struct sockaddr_in addr;
	puts(" [+] PORXY SETTING");
	puts(" [~] PROXY Server Connecting...");

	int SOCKET = socket(AF_INET,SOCK_STREAM,0);
	addr.sin_family       = AF_INET;
    addr.sin_port         = htons(PROXY_SERVER_PORT);                
    addr.sin_addr.s_addr  = inet_addr(PROXY_SERVER_IP);    
	
	int conection_status = connect(SOCKET, (struct sockaddr *)&addr, sizeof(struct sockaddr));
	
	puts(" [*] Proxy connection");

	char Req1[3] = {
		0x05,0x01,0x00
	}; // 0x05 : SOCK5

	send(SOCKET, Req1, 0x03 , MSG_NOSIGNAL);
	
	char Resq1[2];
	recv(SOCKET, Resq1, 0x02, 0x00);
	
	/*
	 Replies

   The SOCKS request information is sent by the client as soon as it has
   established a connection to the SOCKS server, and completed the
   authentication negotiations.  The server evaluates the request, and
   returns a reply formed as follows:

        +----+-----+-------+------+----------+----------+
        |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
        +----+-----+-------+------+----------+----------+
        | 1  |  1  | X'00' |  1   | Variable |    2     |
        +----+-----+-------+------+----------+----------+

     Where:

          o  VER    protocol version: X'05'
          o  REP    Reply field:
             o  X'00' succeeded
             o  X'01' general SOCKS server failure
             o  X'02' connection not allowed by ruleset
             o  X'03' Network unreachable
             o  X'04' Host unreachable
             o  X'05' Connection refused
             o  X'06' TTL expired
             o  X'07' Command not supported
             o  X'08' Address type not supported
             o  X'09' to X'FF' unassigned
          o  RSV    RESERVED
          o  ATYP   address type of following address
	*/
	
	if(Resq1[1] != 0x00)
	{
		printf("%s:%d\n" ," [*] Error Authenticating", Resq1[1]);
		return -1;
	}
	
	puts(" [~] Fetching ...");

	char Domain[] = DST_SERVER_IP;
	char DomainLen = (char)strlen(Domain);
	short Port = htons(DST_SERVER_PORT);
	
	char TmpReq[4] = {
		0x05,0x01,0x00,0x03
	}; // 0x05 : SOCK5 

	char* Req2 = (char*)malloc(4 + 1 + DomainLen + 2);

	memcpy(Req2, TmpReq, 0x04);
	memcpy(Req2 + 4, &DomainLen, 0x01);
	memcpy(Req2 + 5, Domain, DomainLen);
	memcpy(Req2 + 5 + DomainLen, &Port, 0x02);
	send(SOCKET, (char*)Req2, 4 + 1 + DomainLen + 2, MSG_NOSIGNAL);
		
	free(Req2);
	char* Resp2[10];
	recv(SOCKET, Resp2, 10,0);
	
	if(Resp2[1] != 0x00)
	{
		puts(" [*] dst server disconection");
		return -1;
	}

	puts(" [~] dst server conection...");
	char requests[] = "GET /\r\n";
	send(SOCKET, requests, strlen(requests), MSG_NOSIGNAL);
	char RecvBuffer[2000];
	recv(SOCKET, RecvBuffer, 128, 0);
	
	printf("--------------- \n %s \n--------------- ",RecvBuffer);
	return 0;
}
