//
//  main.c
//  BlockChain
//
//  Created by panliang on 2020/12/25.
//  Copyright © 2020 panliang. All rights reserved.
//

#include <stdio.h>
#include "sha256.h"
#include "block_chain.h"
#include <string.h>

int main(int argc, const char * argv[]) {
    BYTE buf[SHA256_BLOCK_SIZE];   // 用于接受sha256结果
    const char* tmp = "学习区块链^_^";
    sha256_main(tmp,strlen(tmp),buf);
    int i;
    for (i=0;i<SHA256_BLOCK_SIZE;i++) {
        printf("%02x",buf[i]);
    }
    printf("\n");
    init();
    return 0;
}
