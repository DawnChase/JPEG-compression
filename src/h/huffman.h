#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "head.h"

struct HuffmanNode {
    int symbol;     // -1: internal node
                    // others: specific value
    int freq;       // frequency
    HuffmanNode *left;
    HuffmanNode *right;
    HuffmanNode(int s, int f) : symbol(s), freq(f), left(NULL), right(NULL) {}
};

struct compareNode {
    bool operator()(HuffmanNode *a, HuffmanNode *b) {
        return a -> freq > b -> freq;
    }
};

void huffman(FILE *fp, vector<pair<int, int> > EC, unsigned char HTInformation, int flag);
void reconstructHuffman(HuffmanNode *&root, unsigned char *BITS, int *HUFFVAL);

#endif