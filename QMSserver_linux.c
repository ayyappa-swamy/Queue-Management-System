#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>

#define SOCKET int

struct qnode
{
    SOCKET client;
    struct qnode *next;
};
typedef struct qnode* QNODE;
QNODE head = NULL;

int globalToken = 0;

int searchQueue(SOCKET client)
{
    //printf("\nSearching for client");
    int index = 0;
    QNODE temp = head;

    if(head == NULL)
        return 0;
    else index = 1;

    while(temp != NULL){
        if(temp->client == client)
            return index;
        else
            temp = temp->next;
        index++;
    }
    return 0;
}

void addToQueue(SOCKET newClient)
{
    QNODE temp = head;
    QNODE newNode = (QNODE)malloc(sizeof(QNODE));
    newNode->client = newClient;
    newNode->next = NULL;

    //printf("\nadding a client");
    if(head == NULL){
        head = newNode;
        return;
    }

    while(temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
    return;
}

int getToken(SOCKET s)
{
    int token;
    //printf("getting token number");
    if((token = searchQueue(s)) == 0){
        addToQueue(s);
        return ++globalToken;
    }
    else return token;
}

int getPassKey()
{
    return 12345;
}

int passKeyVerified(int passKey)
{
    int key = 0;
    while(1){
        printf("\nEnter your pass key : ");
        scanf("%d",&key);
        if(key == passKey)
          return 1;
        else {
            printf("\nPass key is Invalid. Try again.");
        }
    }
}

void * requestManager(void *lp)
{
	char *token = (char*)calloc(100, sizeof(char));
	SOCKET s = *((SOCKET *)lp);

	int prevTokenNumber,newTokenNumber,passKey;

	/*recv(s,token,100,0);
	printf("%s",token);

	token = "<html><h1>hai</h1></html>";
	send(s,token,strlen(token),0);*/

	prevTokenNumber = newTokenNumber = getToken(s);
	send(s,(int*)&newTokenNumber,sizeof(newTokenNumber),0);
	printf("\ncurrent token no is %d sent\n",newTokenNumber);
	sleep(5);

	while(prevTokenNumber != 1){
		if(((newTokenNumber = getToken(s)) != prevTokenNumber)){

			send(s,(int*)&newTokenNumber,sizeof(newTokenNumber),0);
			sleep(1);

			prevTokenNumber = newTokenNumber;
			printf("\ncurrent token no is %d sent\n",newTokenNumber);


			if(newTokenNumber == 1)
				break;
		}
	}
	if(newTokenNumber == 1){
            passKey = getPassKey();
            printf("\npass key is %d",passKey);
            send(s,(char*)&passKey,sizeof(passKey),0);

            if(passKeyVerified(passKey)){//waits till the user enters the correct pass key
				printf("\nverified");
				token = "verified";
				send(s,token,strlen(token),0);
				globalToken--;
				head = head->next;
            }
    }
    close(s);
    return 0;
}

int main()
{
	SOCKET s,new_Client;
	struct sockaddr_in server, client;
	pthread_t new_thread;

	int c;

	printf("Creating Socket...\n");
	s = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket created .\n");

	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);

	printf("Binding socket...\n");
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)	{
		printf("Binding Error.\n");
		return 1;
	}
	printf("Bound \n");

	printf("Listening ...\n");
	listen(s, 3);

	c = sizeof(struct sockaddr_in);

	while ((new_Client = accept(s, (struct sockaddr*)&client, &c)))	{
		printf("\nConnection accepted.\n");
		pthread_create(&new_thread,NULL,requestManager,(void *)&new_Client);
	}

	return 0;
}
