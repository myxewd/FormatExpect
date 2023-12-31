#pragma once
#ifndef main_h
#define main_h
#define nestDepth 16

struct LIMIT {
	int min;
	int max;
};

struct ENODE {
	int echar;
	int next[nestDepth];
};
int fexp_match_text(char* ystr, int length, ENODE* ytree);
ENODE* fexp_build_tree(char* fexpr);
int enode_add_option(ENODE* ynode, int yoption);
int if_not_leaf(ENODE* ynode);
void fexp_free_tree(ENODE* fexpr);
int if_can_free(int emu_iter, int mptr);
void __dumpTree(ENODE* ytree);
#endif