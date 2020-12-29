//
//  block_chain.h
//  BlockChain
//
//  Created by panliang on 2020/12/25.
//  Copyright © 2020 panliang. All rights reserved.
//

#ifndef block_chain_h
#define block_chain_h

#include <stdio.h>

typedef union{
    unsigned char byte[104];
    struct{
        unsigned char sha_all[32];
        unsigned long nonce;             // 64 bit nonce
        unsigned char sha_prev[32];      // 256 bit sha_prev
        unsigned char sha_block[32];     // 256 bit sha_block
    };
} block_chain_head;

typedef struct{
    char body[1000];
} block_chain_body;

typedef struct{
    block_chain_head head;
    block_chain_body body;
} block;

void test(void);
void printBlockHead(block_chain_head* head);
void init(void);
block find_last_block(void);
void read_block(block *blockitem,const char* filename);
void new_block(const char* block_name,char* content);
#endif /* block_chain_h */
