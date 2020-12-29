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
#include "dirent.h"

void copy(block *dst, block *src){
    int i;
    for (i=0;i<sizeof(block_chain_head);i++) {
        dst->head.byte[i] = src->head.byte[i];
    }
    strcpy(dst->body.body,src->body.body);
}

void printBlockHead(block_chain_head* head){
    int i;
    printf("sha_all: ");
    for (i=0;i<32;i++) {
        printf("%02x",head->sha_all[i]);  // 每个字节按照 16 进制输出2位，前面补0
    }
    printf("\nnonce: %lu\n",head->nonce);
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

void read_head_body(BYTE text[10200],block_chain_head* head,block_chain_body body){
    memset(text,0,10200);
    int i=0,j;
    for (j=32;j<sizeof(block_chain_head);j++) {     // 读head，不读 sha_all
        text[i] = head->byte[i];
        i++;
    }
    for (j=0;j<sizeof(block_chain_body);j++) {      // 读body
        text[i] = body.body[j];
        i++;
    }
}

void init(){    // 创建创世纪块
    block_chain_head* genesis = malloc(sizeof(block_chain_head));
    block_chain_body genesis_body;
    strcpy(genesis_body.body,"hello world");
    memset(genesis->byte,0,sizeof(block_chain_head));    // 先将区块头96字节全部初始化为0
    sha256_main(genesis_body.body,sizeof(genesis_body.body),genesis->sha_block);   // 哈希body
    // 随机 nonce ，对(块头+块体)再次哈希
    printf("开始随机nonce\n");
    while (genesis->nonce < 0xffffffffffffffff) {
        // 把块头块体 读入一个 BYTE 数组
        BYTE text[10200];
        read_head_body(text,genesis,genesis_body);
        BYTE buf[SHA256_BLOCK_SIZE];
        size_t size_of_text = sizeof(block_chain_head)-32 + strlen(genesis_body.body);
        sha256_main(text,size_of_text,buf);    // 计算 sha256   有bug??? (已解决)
        // 当text 含有 '\0' 时，出现bug
        // 所以添加变量 size_of_text (注意要减去32 sha_all)
        printf("当前nonce为: %lu     ",genesis->nonce);
        printf("前8bit为:%02x\n",buf[0]);
        if (check_sha(buf)) {   // 检查是否为前导0
            printf("创世纪块nonce为: %lu\n",genesis->nonce);
            const char* filename = "Genesis.block";
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

void read_block(block *blockitem,const char* filename){   // 读一个块
    FILE *fp = fopen(filename,"r");
    if(!fp) {
        perror("File opening failed");
        return;
    }
    int c,i=0;
    while ((c = fgetc(fp)) != EOF) { // standard C I/O file reading loop
        if (i<sizeof(block_chain_head)) {
            blockitem->head.byte[i] = c;
        } else {
            blockitem->body.body[i-sizeof(block_chain_head)] = c;
        }
        i++;
    }
}

int read_dir_block(block block_list[],const char* filePath){ // 读文件夹所有块文件
    DIR *dir = NULL;
    struct dirent *entry;
    if ((dir = opendir(filePath)) == NULL) {
        printf("open dir failed\n");
        return -1;
    }
    int num=0;
    while ((entry = readdir(dir))!=NULL) {
        char* filename = entry->d_name;
        if (strstr(filename,".block")) {      // 匹配区块文件(以.block结尾)
            read_block(&block_list[num],filename);
            num++;
        }
    }
    closedir(dir);
    return num;
}

block find_last_block(){        // 读各个区块
    block block_list[100];
    const char *filePath = ".";
    int size = read_dir_block(block_list,filePath);   // 一共 size 个区块
    // 给所有区块弄一个拓扑排序
    if (size <= 0) {
        printf("no block file\n");
        block tmp;
        memset(tmp.head.byte, 0, sizeof(block_chain_head));
        return tmp;
    }
    // 先找第一个 block
    block topo[size];
    BYTE hash[SHA256_BLOCK_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    int i,j;
    for (i=0;i<size;i++) {
        int cmp = memcmp(hash,block_list[i].head.sha_prev,SHA256_BLOCK_SIZE);
        if(cmp == 0){   // equal， this is the first block
            copy(&topo[0],&block_list[i]);
            break;
        }
    }
    // topo[0] 为创世纪块
    for (i=1;i<size;i++) {
        for (j=0;j<size;j++) {
            int cmp = memcmp(topo[i-1].head.sha_all,block_list[j].head.sha_prev,SHA256_BLOCK_SIZE);
            // 遍历所有区块，找到 prev == topo[i-1] 的，以此来确定 topo[i]
            if (cmp == 0) {
                copy(&topo[i],&block_list[j]);
                break;
            }
        }
    }
    return topo[size-1];
}

block find_first_block(){
    block block_list[100];
    const char *filePath = ".";
    int size = read_dir_block(block_list,filePath);   // 一共 size 个区块
    block tmp;
    memset(tmp.head.byte, 0, sizeof(block_chain_head));
    if (size <= 0) {
        printf("no block file\n");
        return tmp;
    }
    BYTE hash[SHA256_BLOCK_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    int i;
    for (i=0;i<size;i++) {
        int cmp = memcmp(hash,block_list[i].head.sha_prev,SHA256_BLOCK_SIZE);
        if(cmp == 0){   // equal， this is the first block
            return block_list[i];
        }
    }
    return tmp;
}

void new_block(const char* block_name ,char* content){ // 和 init 类似，在末尾添加
    block last_block = find_last_block();
    
    block_chain_head* genesis = malloc(sizeof(block_chain_head));
    block_chain_body genesis_body;
    strcpy(genesis_body.body,content);
    memset(genesis->byte,0,sizeof(block_chain_head));    // 先将区块头全部初始化为0
    int i;
    for (i=0;i<SHA256_BLOCK_SIZE;i++) {
        genesis->sha_prev[i] = last_block.head.sha_all[i];  // 写入 sha_prev
    }
    sha256_main(genesis_body.body,sizeof(genesis_body.body),genesis->sha_block); // sha_block
    // 随机 nonce ，对(块头+块体)再次哈希
    printf("开始随机nonce\n");
    while (genesis->nonce < 0xffffffffffffffff) {
        // 把块头块体 读入一个 BYTE 数组
        BYTE text[10200];
        read_head_body(text,genesis,genesis_body);
        BYTE buf[SHA256_BLOCK_SIZE];
        size_t size_of_text = sizeof(block_chain_head)-32 + strlen(genesis_body.body);
        sha256_main(text,size_of_text,buf);    // 计算 sha256   有bug??? (已解决)
        // 当text 含有 '\0' 时，出现bug
        // 所以添加变量 size_of_text (注意要减去32 sha_all)
        printf("当前nonce为: %lu     ",genesis->nonce);
        printf("前8bit为:%02x\n",buf[0]);
        if (check_sha(buf)) {   // 检查是否为前导0
            printf("新区块nonce为: %lu\n",genesis->nonce);
            const char* filename = strcat(block_name, ".block");
            FILE *fp = fopen(filename,"w");
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

int check_block_chain(){
    block block_list[100];
    const char *filePath = ".";
    int size = read_dir_block(block_list,filePath);   // 一共 size 个区块
    // 给所有区块弄一个拓扑排序
    if (size <= 0) {         // 文件夹下没有区块， 不合法
        printf("no block file\n");
        return 0;
    }
    // 先找第一个 block
    block topo[size];
    BYTE hash[SHA256_BLOCK_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    int i,j;
    for (i=0;i<size;i++) {
        int cmp = memcmp(hash,block_list[i].head.sha_prev,SHA256_BLOCK_SIZE);
        if(cmp == 0){   // equal， this is the first block
            copy(&topo[0],&block_list[i]);
            break;
        }
    }
    if (topo[0].head.nonce == 0) {   // 没找到创世纪块   区块链不合法
        return 0;
    }
    // topo[0] 为创世纪块
    int flag=0;
    for (i=1;i<size;i++) {
        flag=0;
        for (j=0;j<size;j++) {
            int cmp = memcmp(topo[i-1].head.sha_all,block_list[j].head.sha_prev,SHA256_BLOCK_SIZE);
            // 遍历所有区块，找到 prev == topo[i-1] 的，以此来确定 topo[i]
            if (cmp == 0) {
                copy(&topo[i],&block_list[j]);
                flag = 1;
                break;
            }
        }
        if (!flag) {   // 此次循环，找不到一个区块指向上一个区块，区块链不合法
            return 0;
        }
    }
    return 1;
}
