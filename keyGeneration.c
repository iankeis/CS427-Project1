#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void printInBinary(uint64_t n, int bitLength){
    int bits[bitLength];
    for(int i=bitLength-1; i>=0; i--){
        if(n & 1){
            bits[i] = 1;
        } else {
            bits[i] = 0;
        }
        n>>=1;
    }
    for(int j=0; j<bitLength; j++){
        printf("%d", bits[j]);
        if( (j+1)%4 == 0 ){ printf(" "); }
    }
    printf("\n");
    return;
}

int convertToHex(char c){
    int retVal = 0;
    if(c >= '0' && c <= '9'){
        retVal = c - '0';
    } else if( c >= 'A' && c <= 'F'){
        retVal = c - 'A' + 10;
    } else {
        retVal = c - 'a' + 10;
    }
    return retVal;
}

uint8_t rotateKey(uint64_t *key, char flag, int round){
    uint8_t retVal;
    int returnByte = (round % 8) * 8;
    if(flag == 'e'){ // rotate left for encryption
        *key = (*key << 1) | (*key >> 63);
    } else { // rotate right for decryption
        *key = (*key >> 1) | (*key << 63);
    }
    retVal = (*key >> returnByte) & 0xFF;
    printf("0x%02X ", retVal);
    return retVal;
}

int main(int argc, char const *argv[]) {
    int fd = open("key.txt", O_RDONLY);
    if(!fd){ perror(""); exit(1); }

    int bytesRead, hexVal;
    unsigned char buf;
    uint64_t key = 0;
    uint8_t subKey;

    // read in key from file and convert to hex
    while( (bytesRead = read(fd, &buf, 1)) != 0 ){
        hexVal = convertToHex(buf);
        key = (key << 4) + hexVal;
    }

    int round = 0;
    while(round < 16){
        int i = 0;
        while(i < 12){
            subKey = rotateKey(&key, 'e', 4*round+(i%4));
            i++;
        }
        printf("\n");
        round++;
        // subKey = (key >> ((i%8)*8)) & 0xFF;
    }
    // printf("0x%016llX\n", key);
    return 0;
}
