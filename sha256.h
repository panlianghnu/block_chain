//
//  sha256.h
//  BlockChain
//
//  Created by panliang on 2020/12/29.
//  Copyright © 2020 panliang. All rights reserved.
//

#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
    BYTE data[64];  // current 512-bit chunk of message data, just like a buffer
    WORD datalen;   // sign the data length of current chunk
    unsigned long long bitlen;  // the bit length of the total message
    WORD state[8];  // store the middle state of hash abstract
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const BYTE data[], int len);
void sha256_final(SHA256_CTX *ctx, BYTE hash[]);
void sha256_main(BYTE text[],int size_of_text,BYTE buf[SHA256_BLOCK_SIZE]); // 传入字符数组  以及接受数组
#endif   // SHA256_H
