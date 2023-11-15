
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
	printf("%d\n", fexp_match_text(teststr, mytree));
	fexp_free_tree(mytree);
	//printf("malocc=%d\n", malocc);
	return 0;
}
/* 
* ret code
* 0 : match success
* -1 : match fail
* 1 : good string but too long
* 2 : good string but too short
*/

int fexp_match_text(char* ystr, ENODE* ytree) {
	LIMIT alim;
	ENODE* cur = NULL;
	int i, j, flag, options[nestDepth] = { (int)ytree ,0 }, nextlim;
	for (i = 0; ystr[i] != 0x00; i++) {
		flag = 0;
		for (j = 0; options[j] != 0; j++) {
			cur = (ENODE*)(options[j]);
			nextlim = cur->echar;
			if (nextlim == 0)
				continue;
			do {
				alim = *(LIMIT*)(nextlim);
				//printf("lim(min=%d,max=%d),%d,%c\n", alim.min, alim.max, ystr[i], ystr[i]);
				if ((unsigned char)ystr[i] >= alim.min && (unsigned char)ystr[i] <= alim.max) {
					flag = 1;
					break;
				}
			} while ((nextlim = *(int*)(nextlim + sizeof(LIMIT))) != 0);
			if (flag) {
				if (if_not_leaf(cur) == 0 && ystr[i+1]!=0x00)
					return 1;
				memcpy(options, cur->next, sizeof(int) * nestDepth);
				break;
			}
		}
		if (flag == 0)
			return -1;
	}
	if (if_not_leaf(cur) > 0)
		return 2;
	else
		return 0;
}

ENODE* fexp_build_tree(char* fexpr) {
	ENODE* ret = (ENODE*)malloc(sizeof(ENODE));
	ENODE* cur = ret, * lst = NULL;
	Stack loopStruct;
	LIMIT alim;
	char intcache[5] = { '0','\0' }, gint[2] = { 0x00,'\0' };
	int status = 0, i, limlist = 0, curlim = limlist, flag = 0, loopHead = 0, temp;
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
				* (LIMIT*)curlim = alim;
			if (fexpr[i] == ']') {
				*(int*)(curlim + sizeof(LIMIT)) = 0;
				cur->echar = limlist;
				temp = enode_add_option(cur, (int)malloc(sizeof(ENODE)));
				while ((loopHead--) != 0)
					push(&loopStruct, (int)cur);
				loopHead = 0;
				lst = cur;
				cur = (ENODE*)(cur->next[temp]);
				cur->echar = 0;
				memset(cur->next, 0, sizeof(int) * nestDepth);
			}
			else if (fexpr[i] == ';') {
				*(int*)(curlim + sizeof(LIMIT)) = (int)malloc(sizeof(LIMIT) + sizeof(int));
				curlim = *(int*)(curlim + sizeof(LIMIT));
			}
			continue;
		}
		else if (fexpr[i] >= 48 && fexpr[i] <= 57) {
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
	int node_list[nestDepth] = { (int)fexpr,0 }, emu_iter = cs_create(), rec_cache = cs_create(), i, temp,next_node,curlim,curnode;
	for (;;) {
		for (i = 0; node_list[i] != 0; i++) {
			cs_free(&rec_cache);
			rec_cache = cs_create();
			int flag = 0;
			while ((temp = cs_pop(&emu_iter)) != -1) {
				cs_push(&rec_cache, temp);
				if (temp == node_list[i]) {
					flag = 1;
					break;
				}
			}
			while ((temp = cs_pop(&rec_cache)) != -1)
				cs_push(&emu_iter, temp);
			if (flag == 0) {
				cs_push(&emu_iter, node_list[i]);
				next_node = cs_peek(emu_iter);
			}
		}
		if (i == 0)
			break;
		memcpy(node_list, ((ENODE*)next_node)->next, sizeof(int) * nestDepth);
	}
	while ((curnode = cs_pop(&emu_iter)) != -1) {
		curlim = ((ENODE*)curnode)->echar;
		while (curlim != 0) {
			temp = curlim;
			curlim = *(int*)(curlim + sizeof(LIMIT));
			free((void*)temp);
		}
		free((void*)curnode);
	}
	cs_free(&emu_iter);
	cs_free(&rec_cache);
	return;
}


/*
int if_can_free(int emu_iter, int mptr) {
	int stacki=0,i,cache = cs_create(),temp,ret=1;
	for (temp = cs_pop(&emu_iter); ; temp = cs_pop(&emu_iter)) {
		cs_push(&cache, temp);
		stacki++;
		if (cs_peek(emu_iter) == -1)
			break;
		for (i = 0; ((ENODE*)cs_peek(emu_iter))->next[i] != 0; i++)
			if (((ENODE*)cs_peek(emu_iter))->next[i] == mptr)ret= 0;
	}
	for (i = 0; i < stacki; i++)
		cs_push(&emu_iter, cs_pop(&cache));
	cs_free(cache);
	return ret;
}
*/

int enode_add_option(ENODE* ynode, int yoption) {
	int i;
	for (i = 0; ynode->next[i] != 0; i++);
	ynode->next[i] = yoption;
	return i;
}

/*
* ret code
* N* : not leaf with available another node
* 0 : common leaf node
* -1 : leaf node with loop struct
*/

int if_not_leaf(ENODE* ynode) {
	int i, j, ret = 0, flag = 0;
	for (i = 0; ynode->next[i] != 0; i++) {
		for (j = 0; (ENODE*)(ynode->next[i]) != ynode && ((ENODE*)(ynode->next[i]))->next[j] != 0; j++) {
			ret++;
		}
		if ((ENODE*)(ynode->next[i]) == ynode)
			flag = 1;
	}
	if (ret == 0 && flag)
		return -1;
	return ret;
}
void __dumpTree(ENODE* ytree) { //fordebug
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