#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX	500000

struct code{
	char ch;
	char *pat;
	struct code *next;
};

struct node{
	char ch;
	int count;
	struct node *next;
	struct node *left;
	struct node *right;
};

int length(struct node *node);
void printCode(struct code *node);
void printList(struct node *head);
void sort(struct node *a[],int len);
void sortFreq(struct node **headref);
void buildTree(struct node **headref);
char *copy(char *s,int index,char ch);
void swap(struct node *a[],int i,int j);
char *encode(char *msg,struct code *code);
char *decode(struct node *head,char *emsg);
char *returnCode(struct code *node,char m);
struct node *frequency(struct node *p, char ch);
struct node *combineNode(struct node *leastTwo);
struct node *append(struct node *a,struct node *b);
void push(struct code **coderef,char *pat,char ch);
void sortedInsert(struct node **headref,struct node *newnode);
void split(struct node *head,struct node **aref,struct node **bref);
void assignCodes(struct node *node,struct code **coderef,char *pat,int l);


main()
{
	char *ch;
	char pat[MAX];
	char msg[MAX];
	char *emsg,*dmsg;
	struct node *head = NULL;
	struct code *code = NULL;

	printf("\n\nenter the message : ");
	fgets(msg,sizeof(msg),stdin);
	msg[strlen(msg)-1] = '\0';		/* to remove the '\n' */

	for(ch=msg;*ch;++ch)			/* determining relative frequencies */
		head = frequency(head,*ch);

	printf("\n\nrelative frequencies...\n");
	printList(head);

	sortFreq(&head);

	printf("\n\nafter sorting relative frequencies...\n");
	printList(head);

	//printf("\n\nlength of linked list %d\n",length(head));

	printf("\n\nbuilding tree...\n");
	buildTree(&head);

	printf("\n\nassigning codes...\n");
	assignCodes(head,&code,pat,0);
	printCode(code);

	printf("\n\nencoding msg with codes...\n");
	emsg = encode(msg,code);
	printf("\n\tencoded msg is :\n%s\n\n",emsg);

	printf("\n\ndecoding msg...\n");
	dmsg = decode(head,emsg);
	printf("\n\tdecoded msg is :\n%s\n\n",dmsg);
}

struct node *frequency(struct node *p, char ch)
{
	if(p == NULL){
		p = (struct node *)malloc(sizeof(struct node));
		p->ch = ch;
		p->count = 1;
		p->next = NULL;
		p->left = NULL;
		p->right = NULL;
	}
	else if(p->ch == ch)
		p->count += 1;
	else
		p->next = frequency(p->next,ch);

	return p;
}

void printList(struct node *head)
{
	printf("\n\n");
	while(head != NULL){
		printf("\n%d : '%c'",head->count,head->ch);
		head = head->next;
	}
	printf("\n\n");
}

void sortFreq(struct node **headref)		/* accept a unsorted list and returns a sorted list in ascending order */
{
	struct node *current,*sortlist,*nextNode;

	sortlist=NULL;
	current=*headref;

	while(current!=NULL){
		nextNode = current->next;
		sortedInsert(&sortlist,current);
		current=nextNode;
	}

	*headref=sortlist;
}
		
void sortedInsert(struct node **headref,struct node *newnode)	/* to insert node in a ascending order */
{
	struct node *current,*prev_node;

	current=*headref;
	prev_node=NULL;

	while(current!=NULL){
		if( (current->count > newnode->count) || ((current->count == newnode->count) && (current->ch > newnode->ch)) )
			break;
		prev_node=current;
		current=current->next;
	}

	if(prev_node == NULL){					/* if at head position */
			newnode->next=*headref;
			*headref=newnode;
	}
	else{							/* else where */
		newnode->next = current;
		prev_node->next = newnode;
	}
}

void buildTree(struct node **headref)
{
	struct node *leastTwo,*theRest;
	struct node *newNode;

	while(length(*headref) > 1){
		split(*headref,&leastTwo,&theRest);
		newNode = combineNode(leastTwo);
		*headref = append(theRest,newNode);
		sortFreq(headref);
	}
}

int length(struct node *node)
{
	int len = 0;

	while(node != NULL){
		len++;
		node = node->next;
	}
	return len;
}

void split(struct node *head,struct node **aref,struct node **bref)
{
	*aref = head;
	*bref = (*aref)->next->next;
	(*aref)->next->next = NULL;
}

struct node *combineNode(struct node *leastTwo)
{
	struct node *newNode = (struct node *)malloc(sizeof(struct node));

	newNode->count = leastTwo->count + leastTwo->next->count;
	newNode->ch = '~';	/* char with largest ascii value */
	newNode->next = NULL;
	newNode->left = leastTwo;
	newNode->right = leastTwo->next;

	leastTwo->next->next = NULL;

	return newNode;
}
	
struct node *append(struct node *a,struct node *b)
{
	struct node *current = a;

	if(current == NULL)
		return b;

	while(current->next != NULL)
		current = current->next;
	current->next = b;

	return a;
}

void assignCodes(struct node *node,struct code **coderef,char *pat,int l)
{
	if( (node->left == NULL) || (node->right == NULL) )
		push(coderef,copy(pat,l,'\0'),node->ch);
	else{
		assignCodes(node->left,coderef,copy(pat,l,'0'),l+1);
		assignCodes(node->right,coderef,copy(pat,l,'1'),l+1);
	}
	
}

void push(struct code **coderef,char *pat,char ch)
{
	struct code *newNode = (struct code *)malloc(sizeof(struct code));

	newNode->pat = strdup(pat);
	newNode->ch = ch;
	newNode->next = *coderef;
	*coderef = newNode;
}

char *copy(char *s,int index,char ch)
{
	*(s+index) = ch;
	return s;
}

void printCode(struct code *node)
{
	printf("\n\n");
	while(node != NULL){
		printf("\n%c : %s",node->ch,node->pat);
		node = node->next;
	}
	printf("\n\n");
}


char *encode(char *msg,struct code *code)
{
	char*m;
	char emsg[MAX]={'\0'};

	for(m=msg;*m;++m)
		strcat(emsg,returnCode(code,*m));
	return strdup(emsg);
}

char *returnCode(struct code *node,char m)
{
	while(node != NULL){
		if(node->ch == m)
			return node->pat;
		node = node->next;
	}
}

char *decode(struct node *head,char *emsg)
{
	char *m;
	char *outp;
	char output[MAX];
	struct node *node;

	node = head;
	outp = output;
	for(m=emsg;*m;++m){
		if(*m == '0')
			node = node->left;
		else
			node = node->right;

		if( (node->left == NULL) || (node->right == NULL) ){
			*outp++ = node->ch;
			node = head;
		}
	}
	*outp = '\0';

	return strdup(output);
}
