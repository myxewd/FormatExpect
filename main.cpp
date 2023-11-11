
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "stack.h"
#define gets(input) fgets(input, sizeof(input), stdin)
#define resetIntcache intcache[0] = '0', intcache[1] = '\0';
/*
int malocc = 0;
void* mallocEX(size_t size) {
	malocc++;
	void* ret = malloc(size);
	//printf("a=%d\n", (int)ret);
	return ret;
}
void freeEX(void* ptr) {
	malocc--;
	//printf("f=%d\n", (int)ptr);
	free(ptr);
}
#define malloc(size) mallocEX(size)
#define free(size) freeEX(size)
*/
int main(void) {
	char fexpr[1000],teststr[1000];
	gets(fexpr);
	gets(teststr);
	ENODE* mytree = fexp_build_tree(fexpr);
	__dumpTree(mytree);
	printf("%d\n", if_match(teststr, mytree));
	fexp_free_tree(mytree);
	return 0;

}

int if_match(char* ystr, ENODE* ytree) {
	LIMIT alim;
	int i,j, flag, options[nestDepth] = { (int)ytree ,0},nextlim;
	for (i = 0; ystr[i] != 0x00; i++) {
		flag = 0;
		for (j = 0; options[j] != 0; j++) {
			ENODE* cur = (ENODE*)(options[j]);
			nextlim = cur->echar;
			if (nextlim == 0)
				continue;
			do {
				alim = *(LIMIT*)(nextlim);
				//printf("lim(min=%d,max=%d),%d\n", alim.min, alim.max,ystr[i]);
				if (ystr[i] >= alim.min && ystr[i] <= alim.max) {
					flag = 1;
					break;
				}
			} while ((nextlim = *(int*)(nextlim + sizeof(LIMIT))) != 0);
			if (flag) {
				if (!if_not_leaf(cur)) 
					return 0;
				memcpy(options, cur->next, sizeof(int) * nestDepth);
				break;
			}
		}
		if (flag == 0) 
			return -1;
	}
	return 0;
}

ENODE* fexp_build_tree(char* fexpr) {
	ENODE* ret = (ENODE*)malloc(sizeof(ENODE));
	ENODE* cur = ret,*lst=NULL;
	Stack loopStruct;
	LIMIT alim;
	char intcache[5] = { '0','\0' }, gint[2] = {0x00,'\0' };
	int status = 0, i,limlist=0,curlim=limlist,flag=0,loopHead=0,temp;
	initialize(&loopStruct);
	memset(cur->next, 0, sizeof(int) * nestDepth);
	for (i = 0; fexpr[i] != 0x00; i++) {
		if (fexpr[i] == '{') {
			loopHead++;
			continue;
		}
		else if (fexpr[i] == '}') {
			enode_add_option(lst, pop(&loopStruct));
			continue;
		}
		else if (fexpr[i] == '[') {
			resetIntcache
			limlist = (int)malloc(sizeof(LIMIT) + sizeof(int)), curlim = limlist;
			continue;
		}
		else if (fexpr[i] == ',') {
			alim.min = atoi(intcache);
			resetIntcache
		}
		else if (fexpr[i] == ';' || fexpr[i] == ']') {
			alim.max = atoi(intcache);
			resetIntcache
			*(LIMIT*)curlim = alim;
			if (fexpr[i] == ']') {
				*(int*)(curlim + sizeof(LIMIT)) = 0;
				cur->echar = limlist;
				temp=enode_add_option(cur, (int)malloc(sizeof(ENODE)));
				while ((loopHead--) != 0)
					push(&loopStruct, (int)cur);
				loopHead = 0;
				lst = cur;
				cur = (ENODE*)(cur->next[temp]);
				cur->echar = 0;
				memset(cur->next, 0, sizeof(int) * nestDepth);
			}
			else if (fexpr[i] == ';'){
				*(int*)(curlim + sizeof(LIMIT)) = (int)malloc(sizeof(LIMIT) + sizeof(int));
				curlim = *(int*)(curlim + sizeof(LIMIT));
			}
			continue;
		}
		else if(fexpr[i]>=48 && fexpr [i]<=57){
			gint[0] = fexpr[i];
			strcat_s(intcache, gint);
		}
		else {
			;
		}
	}
	return ret;
}

void fexp_free_tree(ENODE* fexpr) {
	int node_list[nestDepth],i,j,freerec,curlist,curlim,temp,flag=0;
	memcpy(node_list, fexpr->next, sizeof(int) * nestDepth);
	Stack emu_iter;
	ENODE* curnode;
	freerec = (int)malloc(2 * sizeof(int)), * (int*)freerec = 0, * (int*)(freerec + sizeof(int)) = 0, curlist = freerec;
	initialize(&emu_iter);
	push(&emu_iter, (int)fexpr);
	for (i = 0; node_list[i] != 0; ) {
		push(&emu_iter, node_list[i]);
		memcpy(node_list, ((ENODE*)(node_list[i]))->next, sizeof(int) * nestDepth);
	}
	while ( peek(&emu_iter) != -1) {
		for (j = 0; (int)(curnode = (ENODE*)((((ENODE*)peek(&emu_iter))->next)[j])) != 0; j++) {
			freeroot:
			for(curlist = freerec;*(int*)curlist!=0;curlist=*(int*)(curlist+sizeof(int)))
				if(*(int*)curlist==(int)curnode)goto freed;
			if (curnode == (ENODE*)peek(&emu_iter))
				goto freed;
			curlim = curnode->echar;
			while (curlim != 0) {
				temp = curlim;
				curlim = *(int*)(curlim + sizeof(LIMIT));
				free((void*)temp);
			}
			free((void*)curnode);
			for (curlist = freerec; *(int*)curlist != 0; curlist = *(int*)(curlist + sizeof(int)));
			*(int*)curlist = (int)curnode;
			*(int*)(curlist + sizeof(int)) = (int)malloc(2 * sizeof(int));
			*(int*)(*(int*)(curlist + sizeof(int))) = 0, * (int*)(*(int*)(curlist + sizeof(int)) + sizeof(int)) = 0;
			curnode = NULL;
			freed:
			if (flag) {
				while (freerec != 0) {
					temp = *(int*)(freerec + sizeof(int));
					free((void*)freerec);
					freerec = temp;
				}
				return;
			}
		}
		pop(&emu_iter);
	}
	flag = 1;
	curnode = fexpr;
	goto freeroot;
}


int enode_add_option(ENODE* ynode, int yoption) {
	int i;
	for (i = 0; ynode->next[i] != 0; i++);
	ynode->next[i] = yoption;
	return i;
}
int if_not_leaf(ENODE* ynode) {
	int i, j,ret=0;
	for (i = 0; ynode->next[i] != 0; i++) {
		for (j = 0; ((ENODE*)(ynode->next[i]))->next[j] != 0;j++) {
			ret++;
		}
	}
	return ret;
}
void __dumpTree(ENODE* ytree) {
	int flag = 0,rec_node[100],depth=0,i,j=0,chk=0;
	ENODE* cur=ytree,*temp=NULL;
	LIMIT* curlim;
	printf("\n--------TreeDumP--------\n");
	do {
		chk = 0;
		printf("\nDEPTH=%d,current=%d\n----NEXTNODE----\n", depth, (int)cur);
		rec_node[j] = (int)cur, j++;
		for (i = 0; cur->next[i] != 0; i++) {
			printf("%d\040\040", cur->next[i]);
			flag = 0;
			for (int k = 0; k < j; k++)
				if (rec_node[k] == cur->next[i])flag = 1;
			if (flag == 0)
				temp = (ENODE*)cur->next[i], chk = 1, depth++;
		}
		printf("\n----Expections----\n");
		curlim = (LIMIT*)cur->echar;
		while ((int)curlim != 0) {
			printf("min=%d,max=%d\n", curlim->min, curlim->max);
			curlim = (LIMIT*)(*(int*)((int)(curlim)+sizeof(LIMIT)));
		}
		cur = temp;
	} while (chk != 0);
	printf("\n--------TreeDumpOK--------\n");
	return;
}