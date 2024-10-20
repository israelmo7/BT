/*
My Code
*/
#include <stdio.h>
#include <stdlib.h>
#include "molib.h"
#include "btlib.h"
#include <time.h>

#define TARGET_NODE 4
#define IS_NUMBER(x) ((x >= '0') && (x <= '9'))
//#define CHUNCK 1024
#define PAYLOAD "^720:2555!L.c 0:-1444!O.c ^380:2333! Lc0:-613!O.310:313!O 310:-313!O. 0:613!O.c ^740:2940!L.c. 0:-222!Oc. 740:2940!Lc. 0:-222!Oc. ^150:50!Lc 0:1500!Oc 133:230!LC. 780:2000!L."
#define Color_Red "\x1b[31m" // Color Start
#define Color_End "\x1b[0m" 
#define MAX_SIZE 26
#define DELAY_TIME 2

#define ABS(x) ((x < 0)? x*-1: x)

typedef struct Payload_Node
{
	int _x;
	int _y;
	char _info[5];
	int _segment;
	
	struct Payload_Node* _next;
	struct Payload_Node* _back;
	
	
}Node;

void setPayloads(char** payloads)
{
	int size= mStrlen(PAYLOAD);
	payloads[0] = (char*)malloc(sizeof(char)*size+1);
	//printf("p.len= %d\n",size);
	mStrcpy(payloads[0],PAYLOAD,mStrlen(PAYLOAD));
	puts("Payload[0] = ");
	puts(payloads[0]);
	
	payloads[1] = NULL;
	payloads[2] = NULL;
}
/*
 * 
 * 
 * 
 * The Function calculate the CheckSum
 * 	-Like That:
 * 		cksm=	X + Y + INFO[..]
 * 
 * 	Input:
 * 		NODE n - contains the data {x,y,info)
 * 
 * 	Output:
 * 		INT - the checksum
 * 
 */
 
 int checkCheckSum(Node n)
{
	int i=0,
		sumInfo = (ABS(n._x) + ABS(n._y));
	
	if(n._info[0])
	{
		sumInfo += (int)n._info[0];
	}
	return (sumInfo);
	
}
void setNode(Node* n, int x, int y, char* info, Node* back)
{
	
		n->_x = x;
		n->_y = y;
		mStrcpy(n->_info,info, mStrlen(info));
		//puts(n->_info);
		//sleep(2);
		n->_next = NULL;
		n->_back = back;
		n->_segment = -1;
}
char* nodeToStr(Node n)
{
	char* buff = (char*)malloc(sizeof(char)*20);
	sprintf(buff, "%d:%d!%s",n._x,n._y, n._info);
	
	return buff;
}

void displayNodes(Node* head, Node* target, int segRequird)
{
	int oldSeg =0;
	char toPrint = 0;

	while (head)
	{
		toPrint = (!segRequird || segRequird == head->_segment);
		if(toPrint)
		{
			if(head->_segment != oldSeg)
			{
				puts("\n---------------------------------------------\n");
				printf("---------------SEGMENT %d----------------\n",head->_segment);// << endl;
			}
			if(head == target)
			{
				printf("|%s[%d:%d](%s)%s|--> ",Color_Red,head->_x, head->_y, head->_info,Color_End);
			
			}
			else
			{
				printf("|[%d:%d](%s)|--> ",head->_x, head->_y, head->_info);
			
			}
		}

		oldSeg = head->_segment;	
		head = head->_next;
	}
	puts("NULL\n");
}
Node* moveToSegment(Node* head, int* seg)
{
	int len = 0;
	while(head && *seg != head->_segment)
	{
		len++;
		head = head->_next;
	}
	*seg = len;
	return head;
	
}
char nread[10] = {0};
int sendRaw(char* payload)
{
	int i=0,response=0;

	while(payload[i])
	{
		//printf("payload[%d] = %c\n",i, payload[i]);
		if(write_node(TARGET_NODE,(&payload[i]),1) == 1)
		{
			printf("[Sent]: %c\n",payload[i]);
			//Response
			//printf("%c|",*(payload+i*sizeof(char)));
			response = read_node_count(TARGET_NODE,nread,26,EXIT_TIMEOUT,1000);
			if(response)
			{
				
				printf("Recived: %s\n",nread[0],nread);
				if(nread[0] == payload[i])
				{
					i++;
					//printf("@");
				}
			}
		}
	}
	
	return i;
}
void sendNode(Node n)
{
	char* checkSum = NULL;
	char* toPayload = nodeToStr(n);
	char* toSend = NULL;
	int lenPayload=0,lenCheckSum = 0;
	
	checkSum = mItoa(checkCheckSum(n));
	printf("CheckSum: %d - %s\n",checkCheckSum(n),checkSum);
	lenPayload=mStrlen(toPayload);
	lenCheckSum=mStrlen(checkSum);
	
	toSend = (char*)malloc(sizeof(char)* (lenCheckSum+lenPayload+2));
	
	mStrcpy(toSend,"{",1);
	mStrcat(toSend,checkSum,lenCheckSum);
	mStrcat(toSend,"}",1);
	mStrcat(toSend,toPayload,lenPayload);
	printf("Sending: %s\n",toSend);

	/*lenPayload = */sendRaw(toSend);
	free(toSend);
	free(toPayload);
	free(checkSum);
	/*return lenPayload;*/
}

Node* watchAndChange(Node* head, int rt_mode)
{
	char* buff = NULL;
	Node* realHead = head;
	int tmp = 0,fast =0,choice = 0;
	unsigned int index = 0;
	Node* tempN = NULL;
	sendRaw("T");
	read_node_count(TARGET_NODE,nread,26,EXIT_TIMEOUT,1000);

	while (head)
	{
		displayNodes(realHead,head,0);
		puts("---------------------------------------------\n");// << endl;
		printf("%s[Node %d/%d]:\nX: %d\nY: %d\nInfo: %s\nCheckSum: %d%s\n",Color_Red,index,head->_segment,head->_x,head->_y,head->_info,checkCheckSum(*head),Color_End);
		puts("---------------------------------------------\n");// << endl;
		if(rt_mode)
		{
			puts("Real-Time Mode: Press 8 for Options\n");
		}
		puts("1)Change\n2)Add\n3)Remove\n4)Move Segment\n5)Next\n6)Back\n9)Exit\n");
		scanf("%d", &choice);
		/////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////
		switch (choice)
		{
		case 1111:
			puts(">>: ");
			buff = (char*)malloc(sizeof(char)*MAX_SIZE);
			gets(buff);
			sendRaw(buff);
			free(buff);
			break;
		case 11://Reset
				sendRaw("T");
				break;
		case 1://CHANGE
			puts("1)X axis\n2)Y axis\n3)Info\n:>>: ");
			scanf("%d", &choice);
			
			printf("Picked: %d\n", choice);
			//tmp = stoi(choice);
			puts("Enter new Value: ");
			if (choice == 1)
				scanf("%d",&head->_x);
			else if (choice == 2)
				scanf("%d",&head->_y);
			else if (choice == 3)
				gets(head->_info);
			
			choice = 0;
			break;
		case 2:// ADD
			puts("Enter Value: \n");
			scanf("%d",&tmp);
			//addNode(head, tmp, false);
			
			printf("Picked: %d\n", tmp);
			break;
		case 3: //REMOVE
			//realHead = deleteNode(realHead);
			//head = realHead;
			//index = 0;
			break;
		case 4:// Move Segment
			puts("\nSegment: ");
			scanf("%d",&tmp);
			
			printf("Picked: %d\n", tmp);
			tempN = moveToSegment(realHead, &tmp);
			if (tempN)
			
			{
				head = tempN;
				index = tmp;
			}
			tempN = NULL;
			break;
		case 5:
			index++;
			if(fast)
			{
				//buff = nodeToStr(*head);
				sendNode(*head);
				//free(buff);
			}
			head = head->_next;
			break;
		case 6:
			index--;
			head = head->_back;
			break;
		case 8:
			if(rt_mode)
			{
				puts("1)Send\n2)Send Segment\n3)Send All\n4)Just Send\n>> ");
				scanf("%d", &choice);
				
				printf("Picked: %d\n", choice);
				if(choice == 1)
				{
					//buff = nodeToStr(*head);
					//sendRaw(buff);
					//free(buff);
					sendNode(*head);
	
				}
				else if(choice == 2)
				{
					int oldSeg = head->_segment;
					while(head && (head->_segment == oldSeg) )
					{
						system("clear");
						displayNodes(realHead,head,oldSeg);
						sendNode(*head);
						sleep(DELAY_TIME*2);
						head = head->_next;
						index++;
					}
				}
				else if(choice == 3)
				{
					while(head)
					{
						system("clear");
						displayNodes(realHead,head,0);
						sendNode(*head);
						sleep(DELAY_TIME*2);
						head = head->_next;
						index++;	
					}
				}
				else if(choice == 4)
				{
					
					fast ^= 1;
				}
				choice = 0;
			}
			break;
		case 9:
			head = NULL;
			break;
		default:
			puts("Error: only 1-4\n");
			break;
			
		system("clear");

		}
		
	}
	return realHead;
}
void printNode(Node n)
{
	
	printf("%d:%d!%s\n",n._x,n._y, n._info);
}
Node* payloadToList(char* payload)
{
	puts("Set things up .. (1/4)\n");
	int i = 0,seg=0, cnt = 0, len = mStrlen(payload);
	char tav = 0;
	puts("Set things up .. (2/4)\n");
	char buff[20] = {0};
	puts("Set things up .. (3/4)\n");
	int temp = 0, indexBuff = 0;
	Node *node = NULL,
		 *head = NULL,
		 *oldOne = NULL;
		 
	puts("Set things up .. (4/4)\n");
	head = (Node*)malloc(sizeof(Node));	 
	setNode(head,0,0,"",NULL);
	printNode(*head);
	oldOne = head;
	printf("Len: %d\n",len);
	puts("Start to convert ..\n");
	for (i = 0; i < len; i++)
	{
		tav = payload[i];
		if ((tav >= '0' && tav <= '9') || tav == '-')
		{
			buff[indexBuff++] = tav;
			buff[indexBuff] = 0;
		}
		else if (tav == ':')
		{
			node = (Node*)malloc(sizeof(Node));
			setNode(node,0,0,"",oldOne);
			oldOne->_next = node;

			temp = atoi(buff);
			buff[0] = 0;
			indexBuff =0;
			node->_x = temp;
		}
		else if (tav == '!')
		{
			cnt = 0;
			oldOne = node;
			temp = atoi(buff);
			buff[0] = 0;
			indexBuff=0;
			node->_y = temp;
			node->_segment = seg;
		}
		else if (tav == '^')
		{
			seg++;
		}
		else if (tav == 'v')
		{
			seg--;
		}
		else
		{
			switch (tav)
			{
			case 'O':
			case 'L':
			case '.':
			case 'c':
			case 'C':
				node->_info[cnt++] = tav;
				node->_info[cnt] = 0;
			default:
				break;
			}
		}
	}
	puts("Done converting!\n");
	return head;
}
void freePayloads(char** p)
{
	int i =0;
	
	for(i=0; i < 3; i++)
	{
		if(p[i])
		{
			free(p[i]);
		}
	}
}
void deleteNodes(Node* head)
{
	Node* pt = head;
	while(head)
	{
		pt= head->_next;
		free(head);
		head=pt;
	}
	puts("Deleted.\n");
	
}

int main()
{
	char* payloads[3];
	int channel,response = 0,choice=0,payloadIndex = 0,rt_mode=0;
	Node* head = NULL;

	if(init_blue("devices.txt") == 0)
	{
		return(0);
	}
	//printf("Node 4 must be TYPE=CLASSIC in devices.txt\n");  
	channel = 1;//find_channel(4,UUID_16,strtohex("FCF05AFD-67D8-4F41-83F5-7BEE22C03CDB",NULL));

	response = connect_node(TARGET_NODE,CHANNEL_NEW,channel);
	if(response == 1)
	{	
		printf("LeaRsi@rp: Connected!\n");
		response = device_connected(TARGET_NODE);
		//printf("Is It? %d\n",response);
		fflush(stdin);
		setPayloads(payloads);

		while(response && choice != 9)
		{
			
			choice = 0;
			/////// WRITE
			puts("1)Set Payload\n2)Edit Payload\n3)Send Payload\n4)Real-Time Debug\n9)Exit\n>> ");
			
			scanf("%d", &choice);

			if(choice == 1)//Set
			{
				//fflush(stdin);
				puts("Pick 1-3: ");
				scanf("%d", &choice);
				
				printf("Picked: %d\n",choice);
				if(choice >= 1 && choice <= 3)
				{
					payloadIndex = (choice-1);
					deleteNodes(head);
					head = payloadToList(payloads[payloadIndex]);
					displayNodes(head,NULL,0);
				}
				else
					puts("Error: not valid\n");
				choice =0;
			}
			else if(choice == 2)//Edit
			{
				if(head)
				{
					head = watchAndChange(head,rt_mode);
				}
				
			}
			else if(choice == 3)//Send
			{
				sendRaw(payloads[payloadIndex]);
			}
			else if(choice == 4)//
			{
				rt_mode ^= 1;
				printf("RT mode: %d\n", rt_mode);
			}
			else
			{
				puts("Error: not valid\n");
			}
			
			//write_node(TARGET_NODE, &tav,1);

			response = device_connected(TARGET_NODE);

		}
		disconnect_node(TARGET_NODE);
		deleteNodes(head);
		freePayloads(payloads);
	}
	else
	{
		printf("Couldnt Connect ..\n");
	  
	}

	close_all();
	return(1);
}
