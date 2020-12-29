//
//  sha256_test.c
//  BlockChain
//
//  Created by panliang on 2020/12/29.
//  Copyright © 2020 panliang. All rights reserved.
//

/*************************** HEADER FILES ***************************/
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "sha256.h"

/*********************** FUNCTION DEFINITIONS ***********************/
int sha256_test()
{
    // test data
    BYTE text1[] = {"学习区块链^_^"};
    BYTE text2[] = {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"};
    BYTE text3[] = {"aaaaaaaaaa"};
    // the true SHA256 result of test data
    // get from the online varify website
    BYTE hash1[SHA256_BLOCK_SIZE] = {0x21,0xf7,0xec,0xe9,0xa3,0x8b,0xfd,0xa3,0xa2,0x5f,0xd2,0x33,0x83,0x42,0x65,0xf3,
                     0x6d,0x12,0xb3,0xc0,0x3a,0xc5,0xb5,0x8e,0xfb,0xe8,0x95,0xfd,0xad,0x25,0xd0,0xa4};
    BYTE hash2[SHA256_BLOCK_SIZE] = {0x24,0x8d,0x6a,0x61,0xd2,0x06,0x38,0xb8,0xe5,0xc0,0x26,0x93,0x0c,0x3e,0x60,0x39,
                                     0xa3,0x3c,0xe4,0x59,0x64,0xff,0x21,0x67,0xf6,0xec,0xed,0xd4,0x19,0xdb,0x06,0xc1};
    BYTE hash3[SHA256_BLOCK_SIZE] = {0xcd,0xc7,0x6e,0x5c,0x99,0x14,0xfb,0x92,0x81,0xa1,0xc7,0xe2,0x84,0xd7,0x3e,0x67,
                                     0xf1,0x80,0x9a,0x48,0xa4,0x97,0x20,0x0e,0x04,0x6d,0x39,0xcc,0xc7,0x11,0x2c,0xd0};
    BYTE buf[SHA256_BLOCK_SIZE];
    SHA256_CTX ctx;
    int idx;
    int pass = 1;

    sha256_init(&ctx);
    sha256_update(&ctx, text1, strlen(text1));
    sha256_final(&ctx, buf);
    pass = pass && !memcmp(hash1, buf, SHA256_BLOCK_SIZE);

    sha256_init(&ctx);
    sha256_update(&ctx, text2, strlen(text2));
    sha256_final(&ctx, buf);
    pass = pass && !memcmp(hash2, buf, SHA256_BLOCK_SIZE);

    sha256_init(&ctx);
    for (idx = 0; idx < 100000; ++idx)
       sha256_update(&ctx, text3, strlen(text3));
    sha256_final(&ctx, buf);
    pass = pass && !memcmp(hash3, buf, SHA256_BLOCK_SIZE);

    return pass;
}

void sha256_main(BYTE text[],BYTE buf[SHA256_BLOCK_SIZE]) // 传入字符数组  以及接受数组
{
//    printf("SHA-256 tests: %s\n", sha256_test() ? "SUCCEEDED" : "FAILED");
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, text, strlen(text));
    sha256_final(&ctx, buf);
//    int i;
//    for (i=0;i<SHA256_BLOCK_SIZE;i++) {
//        printf("%02x",buf[i]);
//    }
//    printf("\n");
    //输出结果：21f7ece9a38bfda3a25fd233834265f36d12b3c03ac5b58efbe895fdad25d0a4
    //和网络上的在线sha256结果一样
    //nice
    return;
}
