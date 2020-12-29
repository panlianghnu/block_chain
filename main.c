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
    // init(); //生成创世纪块
    block last = find_last_block();
    printBlockHead(&last.head);
    printf("body:%s\n",last.body.body);
    return 0;
}
