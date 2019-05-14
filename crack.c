// this brute force approach was adapted from syb0rg on codereview.stackexchange
// https://codereview.stackexchange.com/questions/38474/brute-force-algorithm-in-c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "proj-2_sha256.c"

char alphabet[126 - 32 + 1];

static const int alphabetSize = sizeof(alphabet) - 1;

typedef unsigned char BYTE;
SHA256_CTX ctx;
BYTE buf[SHA256_BLOCK_SIZE];
FILE *fp;
FILE *fp1;
FILE *createdPasswords;
FILE *passwordfile;
FILE *SHApasswordfile;
BYTE buffer[32];
char word[6 + 1];
long fileSize;

void bruteSequential(int maxLen);
void checkPwd4sha256(char* guess);
void ByteArray2SHA(char* str);
void string2ByteArray(char* input, BYTE* output);
void getAlphabet();
void outputPasswords(long total);
void passwordCompare();




void checkPwd4sha256(char* guess){
  fseek(fp, 0, SEEK_SET);
  int n;
  for (int i = 1; i <= 10; ++i)
  {
    fread(buffer, 32, 1, fp);
    n = memcmp(buf, buffer, sizeof(buffer));
    if(n==0){
      printf("%s %d\n", guess, i);
    }
    // printf("i = %d\n", i);
    //
    // for (int k = 0; k < SHA256_BLOCK_SIZE; ++k) {
    //   printf("%02x", buffer[k]);
    // }
    // printf("\n");
  }

}

void checkSHApasswordfile(char* guess){
  fseek(SHApasswordfile, 0, SEEK_SET);
  int n;
  ByteArray2SHA(guess);
  for (int i = 1; i <= fileSize; ++i){
    fread(buffer, 32, 1, SHApasswordfile);
    n = memcmp(buf, buffer, sizeof(buffer));
    if(n==0){
      printf("%s %d\n", guess, i);
    }
  }
}

void output6Passwords(long total){
  long count = 0;
  while((fscanf(createdPasswords, "%s", word)!=EOF) && (count < total)){
    printf("%s\n", word);
    count++;
  }
  if(count < total){
    bruteSequential(6);
    count++;
  }
}

void outputPasswords(long total){
  long count = 0;
  while((fscanf(createdPasswords, "%s", word)!=EOF) && (count < total)){
    printf("%s\n", word);
    count++;
  }
  if(count < total){
    bruteSequential(6);
    count++;
  }
}

void passwordCompare(){
  while(fscanf(passwordfile, "%s", word)!=EOF){
    checkSHApasswordfile(word);
  }
}


void ByteArray2SHA(char* str)
{
    int len = strlen(str);
    BYTE hash[len];
    string2ByteArray(str, hash);
    sha256_init(&ctx);
    sha256_update(&ctx, hash, len);
    sha256_final(&ctx, buf);

}

//function to convert string to byte array
void string2ByteArray(char* input, BYTE* output)
{
    int loop;
    int i;

    loop = 0;
    i = 0;

    while(input[loop] != '\0')
    {
        output[i++] = input[loop++];
    }
}

void bruteImpl(char* str, int index, int maxDepth)
{
    for (int i = 0; i < alphabetSize; ++i)
    {
        str[index] = alphabet[i];

        if (index == maxDepth - 1){
          //printf("string %s converts to: ", str);
          // for (int k = 0; k < SHA256_BLOCK_SIZE; ++k) {
          //   printf("%02x", buf[k]);
          // }
          //printf("\n");
          if (maxDepth == 4){
            ByteArray2SHA(str);
            checkPwd4sha256(str);
          } else {
            printf("%s\n", str);
          }


        }else bruteImpl(str, index + 1, maxDepth);
    }
}

void bruteSequential(int maxLen)
{
  // makes a string of max length (4)
    char* buf = malloc(maxLen + 1);
    memset(buf, 0, maxLen + 1);
    bruteImpl(buf, 0, maxLen);
    free(buf);
}

void getAlphabet()
{
  for( int i = 32; i <= 126; i++ )
  {
      alphabet[i-32] = (char) i;
  }
  alphabet[126 - 32 + 1] = '\0';
}


int main(int argc, char* argv[])
{
  getAlphabet();
  if (argc == 1){
    fp = fopen("pwd4sha256", "r");
    bruteSequential(4);
    fp1 = fopen("pwd6sha256", "r");
    bruteSequential(4);
    fclose(fp);


    fclose(fp);
    return 0;
  }
  if (argc == 2){
    long total;
    total = atoi(argv[1]);
    printf("total: %ld\n", total);
    createdPasswords = fopen("edited_passwords.txt", "r");
    outputPasswords(total);
    return 0;
  }
  if (argc == 3){
    passwordfile = fopen(argv[1], "r");
    SHApasswordfile = fopen(argv[2], "r");
    fseek(SHApasswordfile, 0 , SEEK_END);
    fileSize = ftell(SHApasswordfile) / 32;
    fseek(SHApasswordfile, 0 , SEEK_SET);

    passwordCompare();

    fclose(passwordfile);
    fclose(SHApasswordfile);
  }
}
