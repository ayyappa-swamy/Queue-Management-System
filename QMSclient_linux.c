#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<time.h>
#include<string.h>

#define SOCKET int

int main()
{
	int size;
	SOCKET s;
	struct sockaddr_in server;
	char *tokenNo = (char*)calloc(10,sizeof(char));

	printf("Creating Socket..\n");
	s = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket created ...\n");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(1234);

	printf("Press 1 to GET TOKEN.");
	getc(stdin);

	if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0){
		printf("Connection Error.\n");
		return 1;
	}
	printf("Connected to Queue Management System Server.\n");

	while((size = recv(s,tokenNo,sizeof(int),0)) > 0){
        printf("\nYour current token number is : %d",*((int *)tokenNo));
        if(*((int *)tokenNo) == 1){
            printf("\nYour pass key is :");
            break;
        }
        sleep(5);
	}
	size = recv(s,tokenNo,sizeof(int),0);
	printf("%d\n",*((int *)tokenNo));

	printf("Please enter your pass key in the server..\n");

	size = recv(s,tokenNo,10,0);
	tokenNo[size] = '\0';
	printf("%s\n",tokenNo);

	close(s);
	return 0;
}
