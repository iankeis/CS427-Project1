#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

uint8_t fTable[16][16] = {
    0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3,0x21,0x15,0x78,0x99,0xb1,0xaf,0xf9,
    0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,0x4e,0x38,0x44,0x28,
    0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,0x7a,0xc3,0xe9,0xfa,0x3d,0x53,
    0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2,
    0x39,0xb6,0x7b,0x0f,0xc1,0x93,0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8,
    0x55,0xb9,0xda,0x85,0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90,
    0x35,0xd5,0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76,
    0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,0x4a,0x1d,
    0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,0xde,0xc7,0x67,0x18,
    0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,0xdc,0xc9,0x5d,0x5c,0x31,0xa4,
    0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40,
    0x34,0x4b,0x1c,0x73,0xd1,0xc4,0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5,
    0xad,0x04,0x23,0x9c,0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2,
    0x0c,0xef,0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8,
    0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,0xbb,0xac,
    0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,0x05,0x59,0x2a,0x46
};

// helper routine for testing that brings numbers in binary format
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

//converts a single char into a hex value
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

//rotates value specified by <key> 1 bit. method will shift left if 'e' is passed in <flag> and right if anything else is
uint8_t getSubKey(uint64_t *key, char direction, int round){
    uint8_t retVal;
    int returnByte = (round % 8) * 8;
    if(direction == 'e'){ // rotate left for encryption
        *key = (*key << 1) | (*key >> 63);
    } else { // rotate right for decryption
        retVal = (*key >> returnByte) & 0xFF;
        *key = (*key >> 1) | (*key << 63);
        return retVal;
    }
    retVal = (*key >> returnByte) & 0xFF;
    return retVal;
}

int rotateBits(uint16_t *bitString, int bitLength, char direction){
    if(direction == 'l'){ // rotate left
        *bitString = (*bitString << 1) | (*bitString >> (bitLength - 1));
    } else { // rotate right
        *bitString = (*bitString >> 1) | (*bitString << (bitLength - 1));
    }
    return 0;
}

uint8_t getFTableValue(uint8_t g){
    return fTable[ (g >> 4) & 0xF ][ g & 0xF ];
}

uint16_t concatinate(uint8_t k1, uint8_t k2){
    uint16_t retVal = 0;
    return ((retVal + k1) << 8) + k2;
}

int GFunction(uint16_t r, uint16_t *t, uint8_t k0, uint8_t k1, uint8_t k2, uint8_t k3, int round){
    uint8_t g1, g2, g3, g4, g5, g6;
    g2 = r & 0xFF;
    g1 = (r >> 8) & 0xFF;
    g3 = getFTableValue(g2^k0) ^ g1;
    g4 = getFTableValue(g3^k1) ^ g2;
    g5 = getFTableValue(g4^k2) ^ g3;
    g6 = getFTableValue(g5^k3) ^ g4;
    // printf("g1 = %02x, g2 = %02x, g3 = %02x, g4 = %02x, g5 = %02x, g6 = %02x\n", g1,g2,g3,g4,g5,g6);
    *t = *t + g5;
    *t = (*t << 8) + g6;
    return 0;
}

int Ffunction(uint16_t r0, uint16_t r1, uint16_t *f0, uint16_t *f1, uint64_t *key, int encrypt, int round){
    uint8_t subKey[12];
    uint16_t t0 = 0;
    uint16_t t1 = 0;
    // if encrypt flag set, generate subkeys in forwards order
    if(encrypt){
        for(int i=0; i<12; i++){
            subKey[i] = getSubKey(key, 'e', 4*round+(i%4));
            // printf("0x%02X ", subKey[i]);
        }
    // if encrypt flag set to zero, generate subkeys in reverse order for decryption
    } else {
        for(int i=0; i<12; i++){
            subKey[11-i] = getSubKey(key, 'd', 4*(15-round)+(3 - i%4));
            // printf("0x%02X ", subKey[11-i]);
        }
        for(int j=0; j<12; j++){
            // printf("0x%02X ", subKey[j]);
        }
        printf("\n");
    }
    GFunction(r0, &t0, subKey[0], subKey[1], subKey[2], subKey[3], round);
    GFunction(r1, &t1, subKey[4], subKey[5], subKey[6], subKey[7], round);
    *f0 = (t0 + (t1<<1) + concatinate(subKey[8],  subKey[9]))  % (0xFFFF+1);
    *f1 = ((t0<<1) + t1 + concatinate(subKey[10], subKey[11])) % (0xFFFF+1);
    // printf("t0 = %02x, t1 = %02x f0 = %02x, f1 = %02x\n", t0, t1, *f0, *f1);

    return 0;
}

int wsu_encrypt(uint64_t plainTextWord, uint64_t *cipherTextWord, uint64_t *key){
    uint16_t w0, w1, w2, w3;
    uint16_t k0, k1, k2, k3;
    uint16_t r0, r1, r2, r3, r2_prime, r3_prime;
    uint16_t f0 = 0;
    uint16_t f1 = 0;
    // printf("key: 0x%016llX\n", *key);
    // printf("0x%016llX\n", plainTextWord);

    w3 = plainTextWord & 0xFFFF;
    w2 = (plainTextWord >>= 16) & 0xFFFF;
    w1 = (plainTextWord >>= 16) & 0xFFFF;
    w0 = (plainTextWord >>= 16) & 0xFFFF;

    k3 = *key & 0xFFFF;
    k2 = (*key >> 16) & 0xFFFF;
    k1 = (*key >> 32) & 0xFFFF;
    k0 = (*key >> 48) & 0xFFFF;

    r3 = w3 ^ k3;
    r2 = w2 ^ k2;
    r1 = w1 ^ k1;
    r0 = w0 ^ k0;
    // printf("After whitening %02x %02x %02x %02x\n", r0,r1,r2,r3);

    int round = 0;
    while(round < 16){
        Ffunction(r0, r1, &f0, &f1, key, 1, round);
        // printf("f0 = %02x, f1 = %02x\n", f0, f1);
        r2_prime = r2 ^ f0;
        rotateBits(&r2_prime, 16, 'r');
        rotateBits(&r3, 16, 'l');
        r3_prime = r3 ^ f1;
        r2 = r0;
        r3 = r1;
        r0 = r2_prime;
        r1 = r3_prime;
        // printf("After round(%d) %02x %02x %02x %02x\n", round,r0,r1,r2,r3);
        round++;
    }

    *cipherTextWord = *cipherTextWord + r2;
    *cipherTextWord = (*cipherTextWord << 16) + r3;
    *cipherTextWord = (*cipherTextWord << 16) + r0;
    *cipherTextWord = (*cipherTextWord << 16) + r1;
    *cipherTextWord ^= *key;

    // printf("After whitening %016llx\n", *cipherTextWord);
    return 0;
}

int wsu_decrypt(uint64_t cipherTextWord, uint64_t *plainTextWord, uint64_t *key){
    return 0;
}

int main(int argc, char const *argv[]) {
    if(argc < 3){
        printf("Usage: ./wsu_crypt <plaintextFile> <keyFile>\n");
        exit(1);
    }
    // checks that both plaintext file and key file exist and are readable
    if(access(argv[1], R_OK) != 0){ perror("plaintext file error"); exit(1); }
    if(access(argv[2], R_OK) != 0){ perror("key file error"); exit(1); }

    int inputFile_d = open(argv[1], O_RDONLY);
    if(!inputFile_d){ perror(""); exit(1); } // if error in opening file, print error and exit
    int keyFile_d = open(argv[2], O_RDONLY);
    if(!keyFile_d){ perror(""); exit(1); }
    int outputFile_d = open("ciphertext.txt", (O_WRONLY | O_CREAT));
    if(!outputFile_d){ perror(""); exit(1); }

    int bytesRead, hexVal;
    unsigned char keyBuf;
    uint64_t key = 0;
    uint8_t subKey;

    // read in key from file and convert to hex
    while( (bytesRead = read(keyFile_d, &keyBuf, 1)) != 0 ){
        hexVal = convertToHex(keyBuf);
        key = (key << 4) + hexVal;
    }
    close(keyFile_d);

    unsigned char ptBuf;
    int counter = 0;
    uint64_t plainTextWord = 0;
    uint64_t cipherTextWord = 0;

    while( (bytesRead = read(inputFile_d, &ptBuf, 1)) != 0 ){
        plainTextWord = (plainTextWord << 4) + convertToHex(ptBuf);
        if(((counter + 1) % 16) == 0){
            wsu_encrypt(plainTextWord, &cipherTextWord, &key);
            plainTextWord = 0;
            dprintf(outputFile_d, "%016llx", cipherTextWord); // output ciphertext to output file
        }
        counter++;
        // printInBinary(plainTextWord, 64);
        // printf("%016llX\n", plainTextWord);
    }

    close(inputFile_d);
    close(outputFile_d);
    return 0;
}


// 0001 0011 1001 1110 0010 1011 0011 0100 0011 0101 1110 0010 1011 0011 0100 0101 0101 0111 0010 0110 0011 1100 0101 0110
