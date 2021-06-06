#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define _ENC_OPT_ 1
#define _DEC_OPT_ 2

//密钥第一次置换矩阵
// 64位密钥跨过8 16 24 32 40 48 56 64 位取得打乱的56位
int TBL_PC_1[56] = {57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18,
                    10, 2,  59, 51, 43, 35, 27, 19, 11, 3,  60, 52, 44, 36,
                    63, 55, 47, 39, 31, 23, 15, 7,  62, 54, 46, 38, 30, 22,
                    14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4};

//半截密钥偏移表
//将56位密钥分为左右两段28位，分别左移16次并保存每次的值，再将左右两段合并成
// 16组数据
const int TBL_MOVE[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

//密钥第二次置换矩阵
//用上一步偏移后的16组数据，取56位密匙的48位组成16段子密钥
int TBL_PC_2[48] = {14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10,
                    23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
                    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
                    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};

// IP置换矩阵
// 64位明文打乱顺序
int TBL_IP[64] = {58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
                  62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
                  57, 49, 41, 33, 25, 17, 9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
                  61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};

//扩展矩阵
//分成两段32位左右值，分别拓展成48位，其中有重复位，48位再与48位子密钥进行异或
int TBL_EXT[48] = {32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
                   8,  9,  10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
                   16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
                   24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};

// 8个s盒
// 48位数据按6位一段分成8组获得下标取s1-s8表中的值，获取32位数据
// 6位数据首位2位一段，中间4位为一段，取第四行第二列值
int TBL_S1[4][16] = {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
                     {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
                     {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
                     {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}};

int TBL_S2[4][16] = {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
                     {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
                     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
                     {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}};

int TBL_S3[4][16] = {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
                     {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
                     {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
                     {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}};

int TBL_S4[4][16] = {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
                     {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
                     {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
                     {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}};

int TBL_S5[4][16] = {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
                     {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
                     {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
                     {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}};

int TBL_S6[4][16] = {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
                     {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
                     {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
                     {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}};

int TBL_S7[4][16] = {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
                     {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
                     {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
                     {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}};

int TBL_S8[4][16] = {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
                     {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
                     {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
                     {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}};

// P盒
// 8轮s盒查询结果获得的32位，再次进行顺序打乱
int TBL_P[32] = {16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
                 2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25};

//逆IP置换矩阵
// 16轮迭代后，再将左右两段交换，使用逆置换表打乱顺序得到最终加密结果
int TBL_IPR[64] = {40, 8,  48, 16, 56, 24, 64, 32, 39, 7,  47, 15, 55,
                   23, 63, 31, 38, 6,  46, 14, 54, 22, 62, 30, 37, 5,
                   45, 13, 53, 21, 61, 29, 36, 4,  44, 12, 52, 20, 60,
                   28, 35, 3,  43, 11, 51, 19, 59, 27, 34, 2,  42, 10,
                   50, 18, 58, 26, 33, 1,  41, 9,  49, 17, 57, 25};

//该多维数组方便将S盒中的数据转化为2进制字符串
unsigned char agSboxMapList[16][5] = {
    "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
    "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"};

//该函数实现将输入16进制字符串转化为ASCII码，in为输入，out为输出。
//如输入in：313233343536，则out：12345678
int hex_str2bytes(unsigned char *in, unsigned char *out) {
  int i = 0, j = 0;
  int num = 0;
  int tmp = 0;

  for (i = 0; i < strlen((char *)in); i++) {
    if (in[i] < 58 && in[i] > 47) // 0-9
      tmp = in[i] - 48;
    else if (in[i] < 71 && in[i] > 64) // A-F
      tmp = 10 + in[i] - 65;
    else if (in[i] < 103 && in[i] > 96) // a-f
      tmp = 10 + in[i] - 97;
    else {
      printf("Invalid arg!\n");
      return -1;
    }

    if (i % 2 == 0)
      num = tmp * 16;
    else {
      num += tmp;
      out[j++] = num;
    }
  }
  return 0;
}

//该函数实现将输入ASCII码字符串转化为16进制字符串，in为输入，in_len为输入字符串的长度，out为输出。
//如输入in：12345678  in_len：8，则输出out：3132333435363738
int bytes2hex_str(unsigned char *in, int in_len, unsigned char *out) {
  int i;
  for (i = 0; i < in_len; i++) {
    sprintf((char *)out + i * 2, "%02x", in[i]);
  }
  return 0;
}

//该函数实现将ASCII码转化成2进制字符串，in为输入，in_len为输入长度，out为输出。
//如输入in：12345678 in_len：8，则输出：00110001 00110010 00110011 00110100
//00110101 00110110 00110111 00111000。
void bytes2bits_str(unsigned char *in, int in_len, unsigned char *out) {
  int i = 0;
  for (i = 0; i < in_len * 8; i++) {
    if (in[i / 8] & (0x01 << (7 - i % 8)))
      out[i] = '1';
    else
      out[i] = '0';
  }
}

//该函数实现将2进制字符串转化为ASCII码字符串。in为输入，out为输出。
//如输入in：00110001 00110010 00110011 00110100 00110101 00110110 00110111
//00111000，则输出out：12345678
void bits_str2bytes(unsigned char *in, unsigned char *out) {
  int i = 0, j = 0;
  int num = 0;
  for (i = 0; i < strlen((char *)in); i++) {
    if (in[i] == '1') {
      num += pow(2, 7 - i % 8);
    }
    if ((i + 1) % 8 == 0) {
      out[j++] = num;
      num = 0;
    }
  }
}

//该函数实现将2进制字符串转化为10进制整数。
// str为输入，num为输出。如输入str：1101，则输出num：13。
void bits_str2deci(unsigned char *str, int *num) {
  int i = 0, j = 0;
  *num = 0;
  for (i = strlen((char *)str) - 1; i >= 0; i--) {
    if (str[i] == '1') {
      *num += pow(2, j);
    }
    j++;
  }
}

//该函数实现将输入64长度2进制密钥按照置换选择1打乱顺序和选择，得到56长度2进制子密钥。
void getBitsByPc1(unsigned char *in, unsigned char *out) {
  int i = 0;
  int index = 0;
  for (i = 0; i < 56; i++) {
    index = TBL_PC_1[i];
    out[i] = in[index - 1];
  }
}

//该函数实现将56长度2进制子密钥分为左、右两组进行循环左移得到左、右28长度子密钥各16组。
void getBitsListByMove(unsigned char *key56Bits,
                       unsigned char keyLeft28BitsList[][28 + 1],
                       unsigned char keyRight28BitsList[][28 + 1]) {
  int i = 0;
  int shift = 0;

  memcpy(keyLeft28BitsList[0], key56Bits, 28);
  memcpy(keyRight28BitsList[0], key56Bits + 28, 28);

  for (i = 0; i < 16; i++) {
    shift = TBL_MOVE[i];
    memcpy(keyLeft28BitsList[i + 1], keyLeft28BitsList[i] + shift, 28 - shift);
    memcpy(keyLeft28BitsList[i + 1] + 28 - shift, keyLeft28BitsList[i], shift);
    memcpy(keyRight28BitsList[i + 1], keyRight28BitsList[i] + shift,
           28 - shift);
    memcpy(keyRight28BitsList[i + 1] + 28 - shift, keyRight28BitsList[i],
           shift);
  }
}

//该函数实现将输入的16组56位子密钥进行置换选择2，得到16组48位长度子密钥。
void getBitsListByPc2(unsigned char in[][56 + 1], unsigned char out[][48 + 1]) {
  int i = 0;
  int j = 0;
  int index = 0;

  for (i = 0; i < 16; i++) {
    for (j = 0; j < 48; j++) {
      index = TBL_PC_2[j];
      out[i][j] = in[i][index - 1];
    }
  }
}

//该函数实现将64长度明文按照初始置换IP打乱顺序，得到64长度打乱顺序的明文。
void getBitsByIp(unsigned char *in, unsigned char *out) {
  int i = 0;
  int index = 0;
  for (i = 0; i < 64; i++) {
    index = TBL_IP[i];
    out[i] = in[index - 1];
  }
}

//即DES中的f函数，该函数实现将32长度输入数据先通过扩展矩阵转为48长度，再与48长度子密钥进行异或，
//再将异或后得到的数据输入S盒得到32长度数据，最终通过置换矩阵P运算得到32长度result。
unsigned char *getBitsByFunc(unsigned char *data, unsigned char *key,
                             unsigned char *result) {
  int i = 0;
  int index = 0;
  char extData[48 + 1] = {0};
  int x = 0;
  int y = 0;
  unsigned char xBits[2 + 1] = {0};
  unsigned char yBits[4 + 1] = {0};
  unsigned char SboxRet[32 + 1] = {0};

  for (i = 0; i < 48; i++) {
    index = TBL_EXT[i];
    extData[i] = data[index - 1];
  }

  for (i = 0; i < 48; i++) {
    if (extData[i] == key[i])
      extData[i] = '0';
    else
      extData[i] = '1';
  }

  for (i = 0; i < 8; i++) {
    memcpy(xBits, extData + i * 6, 1);
    memcpy(xBits + 1, extData + i * 6 + 5, 1);
    memcpy(yBits, extData + i * 6 + 1, 4);
    bits_str2deci(xBits, &x);
    bits_str2deci(yBits, &y);

    switch (i) {
    case 0:
      memcpy(SboxRet + i * 4, agSboxMapList[TBL_S1[x][y]], 4);
      break;
    case 1:
      memcpy(SboxRet + i * 4, agSboxMapList[TBL_S2[x][y]], 4);
      break;
    case 2:
      memcpy(SboxRet + i * 4, agSboxMapList[TBL_S3[x][y]], 4);
      break;
    case 3:
      memcpy(SboxRet + i * 4, agSboxMapList[TBL_S4[x][y]], 4);
      break;
    case 4:
      memcpy(SboxRet + i * 4, agSboxMapList[TBL_S5[x][y]], 4);
      break;
    case 5:
      memcpy(SboxRet + i * 4, agSboxMapList[TBL_S6[x][y]], 4);
      break;
    case 6:
      memcpy(SboxRet + i * 4, agSboxMapList[TBL_S7[x][y]], 4);
      break;
    case 7:
      memcpy(SboxRet + i * 4, agSboxMapList[TBL_S8[x][y]], 4);
      break;
    default:
      break;
    }
  }

  for (i = 0; i < 32; i++) {
    index = TBL_P[i];
    *(result + i) = SboxRet[index - 1];
  }

  return result;
}

//将32长度的str1、和str2进行异或，结果保存到result中。
void xor_bits_str(unsigned char *str1, unsigned char *str2,
                  unsigned char *result) {
  int i = 0;
  for (i = 0; i < 32; i++) {
    if (*(str1 + i) == *(str2 + i)) {
      memcpy(result + i, "0", 1);
    } else
      memcpy(result + i, "1", 1);
  }
}

//该函数实现将64长度数据in按照逆初始置换Ipr打乱重排，结果保存到out中。
void getBitsByIpr(unsigned char *in, unsigned char *out) {
  int i = 0;
  int index = 0;
  for (i = 0; i < 64; i++) {
    index = TBL_IPR[i];
    out[i] = in[index - 1];
  }
}

//该函数为DES加解密函数，实现将字符串进行DES加解密功能。
// key为密钥，input为明文，input_len为明文长度，output为输出字符串，为ASCII码，opt为要进行的加解密操作，分别有_ENC_OPT_和_DEC_OPT_。
int proc_enc_dec(unsigned char *key, unsigned char *input, int input_len,
                 unsigned char *output, int opt) {
  int i = 0;
  unsigned char keyHexStr[16 + 1] = {0};
  unsigned char keyBits[1024] = {0};
  unsigned char key56Bits[56 + 1] = {0};
  unsigned char keyLeft28BitsList[17][28 + 1] = {{0}};
  unsigned char keyRight28BitsList[17][28 + 1] = {{0}};
  unsigned char key56BitsList[16][56 + 1] = {{0}};
  unsigned char key48BitsList[16][48 + 1] = {{0}};

  unsigned char inputHexStr[16 + 1] = {0};
  unsigned char inputBits[1024] = {0};
  unsigned char inputByIp[64 + 1] = {0};
  unsigned char inputLeftDataList[1 + 16][32 + 1] = {{0}};
  unsigned char inputRightDataList[1 + 16][32 + 1] = {{0}};
  unsigned char inputAfterFunc[32 + 1] = {0};
  unsigned char inputAfterXor[32 + 1] = {0};
  unsigned char inputLast[64 + 1];

  unsigned char binRet[64 + 1] = {0};
  unsigned char strRet[8 + 1] = {0};
  unsigned char hexRet[16 + 1] = {0};

  if (input_len == 8) {
    for (i = 0; i < 8; i++) {
      sprintf((char *)keyHexStr + i * 2, "%02x", key[i]);
      sprintf((char *)inputHexStr + i * 2, "%02x", input[i]);
    }
  } else {
    for (i = 0; i < input_len; i++) {
      sprintf((char *)keyHexStr + i * 2, "%02x", key[i]);
      sprintf((char *)inputHexStr + i * 2, "%02x", input[i]);
    }
    for (; i < 8; i++) {
      sprintf((char *)keyHexStr + i * 2, "%02x", key[i]);
      sprintf((char *)inputHexStr + i * 2, "00");
    }
  }

  bytes2bits_str(key, 8, keyBits);
  bytes2bits_str(input, 8, inputBits);

  getBitsByPc1(keyBits, key56Bits);

  getBitsListByMove(key56Bits, keyLeft28BitsList, keyRight28BitsList);

  for (i = 0; i < 16; i++) {
    memcpy(key56BitsList[i], keyLeft28BitsList[i + 1], 28);
    memcpy(key56BitsList[i] + 28, keyRight28BitsList[i + 1], 28);
  }

  getBitsListByPc2(key56BitsList, key48BitsList);

  //开始对加解密内容操作
  getBitsByIp(inputBits, inputByIp);

  memcpy(inputLeftDataList[0], inputByIp, 32);
  memcpy(inputRightDataList[0], inputByIp + 32, 32);

  for (i = 1; i <= 16; i++) {
    memcpy(inputLeftDataList[i], inputRightDataList[i - 1], 32);

    if (opt == _ENC_OPT_) {
      xor_bits_str(inputLeftDataList[i - 1],
                   getBitsByFunc(inputRightDataList[i - 1],
                                 key48BitsList[i - 1], inputAfterFunc),
                   inputAfterXor);
    } else if (opt == _DEC_OPT_) {
      xor_bits_str(inputLeftDataList[i - 1],
                   getBitsByFunc(inputRightDataList[i - 1],
                                 key48BitsList[16 - i], inputAfterFunc),
                   inputAfterXor);
    } else {
      printf("[ERROR] Invalid option!\n");
    }

    memcpy(inputRightDataList[i], inputAfterXor, 32);
  }

  memcpy(inputLast, inputRightDataList[16], 32);
  memcpy(inputLast + 32, inputLeftDataList[16], 32);

  getBitsByIpr(inputLast, binRet);

  bits_str2bytes(binRet, strRet);
  for (i = 0; i < 8; i++) {
    sprintf((char *)&hexRet[i * 2], "%02x", strRet[i]);
  }
  memcpy(output, strRet, 8);

  return 0;
}

//该函数为3DES加解密函数，实现将字符串进行3DES加解密功能。
// key为密钥，input为明文，input_len为明文长度，output为输出字符串，为ASCII码，opt为要进行的加解密操作，分别有_ENC_OPT_和_DEC_OPT_。函数运行成功返回0。
int _3des_enc_dec(unsigned char *key, unsigned char *input, int input_len,
                  unsigned char *output, int opt) {
  unsigned char key1[8 + 1] = {0};
  unsigned char key2[8 + 1] = {0};
  unsigned char key3[8 + 1] = {0};
  unsigned char ip[8 + 1] = {0};
  memcpy(key1, key, 8);
  memcpy(key2, key + 8, 8);
  memcpy(key3, key, 8);

  if (opt == _ENC_OPT_) {
    strcpy((char *)ip, (char *)input);
    if (proc_enc_dec(key1, ip, input_len, output, _ENC_OPT_))
      return -1;
    memcpy(ip, output, 8);
    if (proc_enc_dec(key2, ip, 8, output, _DEC_OPT_))
      return -1;
    memcpy(ip, output, 8);
    if (proc_enc_dec(key3, ip, 8, output, _ENC_OPT_))
      return -1;
  } else if (opt == _DEC_OPT_) {
    hex_str2bytes(input, ip);
    if (proc_enc_dec(key1, ip, 8, output, _DEC_OPT_))
      return -1;
    memcpy(ip, output, 8);
    if (proc_enc_dec(key2, ip, 8, output, _ENC_OPT_))
      return -1;
    memcpy(ip, output, 8);
    if (proc_enc_dec(key3, ip, 8, output, _DEC_OPT_))
      return -1;
  } else {
    printf("Invalid args!\n");
    return -2;
  }

  return 0;
}

//自定义读取函数
void fgets_t(unsigned char *str, int count) {
  fgets((char *)str, count, stdin);
  str[strlen((char *)str) - 1] = '\0';
}

//简单操作交互界面
int getTip() {
  system("cls");
  puts("\n");
  puts("Please enter the given number:");
  puts("1.decrypt file.");
  puts("==================================================================");
  puts("(c)cls (#)exit");
  return 1;
}

//该函数实现从文件流fp中默认提取8个字符，返回为提取到的字符个数。
int get8Bits(FILE *fp, char *str) {
  int count;
  memset(str, 0, 8);
  count = fread(str, 1, 8, fp);
  return count;
}

//该函数实现文件3DES加密功能。file_in为输入文件，file_out为输出函数，key为密钥
int file_enc(char *file_in, char *file_out, unsigned char *key) {
  FILE *fp_in, *fp_out;
  int count;
  int bytes = 0;
  double speed;
  int start_time, finish_time, time_cost;
  unsigned char input[8 + 1] = {0};
  unsigned char output[8 + 1] = {0};
  unsigned char outputHex[16 + 1] = {0};

  if ((fp_in = fopen(file_in, "rb")) == NULL) {
    perror("file_enc can not open file_in");
    return -1;
  }
  if ((fp_out = fopen(file_out, "wb")) == NULL) {
    perror("file_enc can not open file_out");
    return -1;
  }

  time_t t;
  t = time(NULL);
  start_time = time(&t);
  printf("Start encrypting\n");
  while ((count = get8Bits(fp_in, (char *)input))) {
    bytes += count;
    _3des_enc_dec(key, input, count, output, _ENC_OPT_);
    bytes2hex_str(output, 8, outputHex);
    fwrite(outputHex, 16, 1, fp_out);
  }
  finish_time = time(&t);
  time_cost = finish_time - start_time;
  speed = (double)bytes / time_cost;
  printf("Encryption finished, %d bytes in total, used %d seconds, speed: %.2f "
         "B/s\n",
         bytes, time_cost, speed);
  fclose(fp_in);
  fclose(fp_out);
  return 0;
}

//该函数实现文件3DES解密功能。file_in为输入文件，file_out为输出函数，key为密钥
int file_dec(char *file_in, char *file_out, unsigned char *key) {
  FILE *fp_in, *fp_out;
  int count;
  int bytes = 0;
  double speed;
  int start_time, finish_time, cost_time;
  unsigned char inputHex[16 + 1] = {0};
  unsigned char input[8 + 1] = {0};
  unsigned char output[8 + 1] = {0};

  if ((fp_in = fopen(file_in, "rb")) == NULL) {
    perror("file_dec can not open file_in");
    return -1;
  }

  if ((fp_out = fopen(file_out, "wb")) == NULL) {
    perror("file_dec can not open file_out");
    return -1;
  }

  printf("Start decryting\n");
  time_t t;
  t = time(NULL);
  start_time = time(&t);
  while ((count = fread(inputHex, 1, 16, fp_in))) {
    bytes += count;
    _3des_enc_dec(key, inputHex, 16, output, _DEC_OPT_);
    fwrite(output, strlen((char *)output), 1, fp_out);
  }
  finish_time = time(&t);
  cost_time = finish_time - start_time;
  speed = (double)bytes / cost_time;

  printf("Decryption finished %d bytes in total, used %d seconds,speed: %.2f "
         "B/s\n",
         bytes, cost_time, speed);

  fclose(fp_in);
  fclose(fp_out);
}

int main(int argc, char **argv) {
  FILE *fp_in, *fp_out;
  unsigned char fin_input;
  unsigned char temp[8 + 1] = {0};
  unsigned char enc_key[16 + 1] = {0};
  unsigned char dec_key[16 + 1] = {0};
  unsigned char buff[16 + 1] = {0};
  unsigned char in_buff[8 + 1] = {0};
  unsigned char in_buff_hex[16 + 1] = {0};
  unsigned char out_buff[8 + 1] = {0};
  unsigned char out_buff_hex[16 + 1] = {0};
  char file_in[128 + 1] = {0};
  char file_out[128 + 1] = {9};

  //  getTip();

  while (1) {
    memset(file_in, 0, 32);
    memset(file_out, 0, 32);
    memset(dec_key, 0, 16);

    printf("Input file to decrypt: ");
    fgets_t((unsigned char *)file_in, 32);
    if ((fp_in = fopen(file_in, "rb")) == NULL) {
      perror("file_in");
      break;
    }
    fclose(fp_in);

    snprintf(file_out, sizeof(file_out), "good-%s", file_in);
    if ((fp_out = fopen(file_out, "wb")) == NULL) {
      perror("file_out");
      break;
    }
    fclose(fp_out);

    printf("Input 16bytes key like 1234567887654321 : ");
    memset(buff, 0, 16);
    fgets_t(buff, 18);
    if (strlen((char *)buff) != 16) {
      puts("Key error! you should input 16bytes key!");
      break;
    }
    strcpy((char *)dec_key, (char *)buff);

    file_dec(file_in, file_out, dec_key);
    break;
  }
}
