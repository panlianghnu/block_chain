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
    //init(); //生成创世纪块
    char block_name[200] = "block_two";
    new_block(block_name,"这是创世纪块后的第二个块！");
    return 0;
}
