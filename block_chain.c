//
//  block_chain.c
//  BlockChain
//
//  Created by panliang on 2020/12/25.
//  Copyright © 2020 panliang. All rights reserved.
//

#include "block_chain.h"
#include "sha256.h"
#include <stdlib.h>
#include <string.h>

void printBlockHead(block_chain_head* head){
    int i;
    printf("nonce: %lu\n",head->nonce);
    printf("sha_prev: ");
    for (i=0;i<32;i++) {
        printf("%02x",head->sha_prev[i]);  // 每个字节按照 16 进制输出2位，前面补0
    }
    printf("\nsha_block: ");
    for (i=0;i<32;i++) {
        printf("%02x",head->sha_block[i]);
    }
    printf("\n");
}

void test(){
    block_chain_head* tmp = malloc(sizeof(block_chain_head));
    // 第一个无符号整数字节分别为：  00000000 00000001 00000002 00000003
    // 第一个无符号整数输出结果为：     03       02       01       00
    // 说明 int 是按照内存地址减少的顺序读的每个字节
    // 但是这不影响，只要大家用相同的方式去解析这些字符即可
    memset(tmp->byte,0xff,sizeof(block_chain_head));   // 按字节对内存赋值
    printBlockHead(tmp);
    free(tmp);
}

int check_sha(BYTE text[SHA256_BLOCK_SIZE]){  // 检验是否为前导0
    if(text[0]==0x00 && text[1] == 0x00)    // 前2 byte 为0满足要求
        return 1;
    return 0;
}

void find_last_block(){
    
}

void read_head_body(BYTE text[10200],block_chain_head* head,block_chain_body body){
    memset(text,0,10200);
    int i,j;
    for (i=0;i<sizeof(block_chain_head);i++) {
        text[i] = head->byte[i];
    }
    for (j=0;j<sizeof(block_chain_body);j++) {
        text[i] = body.body[j];
        i++;
    }
}

void init(){    // 创建创世纪块
    block_chain_head* genesis = malloc(sizeof(block_chain_head));
    block_chain_body genesis_body;
    strcpy(genesis_body.body,"hello world");
    memset(genesis->byte,0,sizeof(block_chain_head));    // 先将区块头96字节全部初始化为0
    sha256_main(genesis_body.body,sizeof(genesis_body.body),genesis->sha_block);   // 哈希 body
    // 随机 nonce ，对(块头+块体)再次哈希
    printf("开始随机nonce\n");
    while (genesis->nonce < 0xffffffffffffffff) {
        // 把块头块体 读入一个 BYTE 数组
        BYTE text[10200];
        read_head_body(text,genesis,genesis_body);
        BYTE buf[SHA256_BLOCK_SIZE];
        int size_of_text = sizeof(block_chain_head) + strlen(genesis_body.body);
        sha256_main(text,size_of_text,buf);    // 计算 sha256       有bug??????????????????????????????
        // 当text 含有 '\0' 时，出现bug
        // 所以添加变量 size_of_text
        printf("当前nonce为: %lu     ",genesis->nonce);
        printf("前8bit为:%02x\n",buf[0]);
        if (check_sha(buf)) {   // 检查是否为前导0
            printf("创世纪块nonce为: %lu\n",genesis->nonce);
            const char* filename = "Genesis";
            FILE *fp = fopen(filename,"w");
            int i;
            for (i=0;i<SHA256_BLOCK_SIZE;i++) {
                genesis->sha_all[i] = buf[i];
            }
            for (i=0;i<sizeof(block_chain_head);i++) {   // 写入区块头
                fprintf(fp,"%c",genesis->byte[i]);
            }
            for (i=0;i<strlen(genesis_body.body);i++) {  // 写入区块体
                fprintf(fp,"%c",genesis_body.body[i]);
            }
            break;
        }
        genesis->nonce++;
    }
}
