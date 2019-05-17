// this brute force approach was adapted from syb0rg on codereview.stackexchange
// https://codereview.stackexchange.com/questions/38474/brute-force-algorithm-in-c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "proj-2_sha256.c"

// this is to store all ASCII characters 32 to 126 (+ 1 for the null byte)
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
long count = 0;
long total;


void checkPwd6sha256(char* guess);
void bruteSequential(int maxLen);
void checkPwd4sha256(char* guess);
void ByteArray2SHA(char* str);
void string2ByteArray(char* input, BYTE* output);
void getAlphabet();
void outputPasswords(long total);
void passwordCompare();
void checkSHApasswordfile(char* guess);




void checkPwd4sha256(char* guess){
  // checks guesses against all the 4 character hashes
  fseek(fp, 0, SEEK_SET);
  int n;
  // iterates through all hashes
  for (int i = 1; i <= 10; ++i)
  {
    fread(buffer, 32, 1, fp);
    n = memcmp(buf, buffer, sizeof(buffer));
    // If there is a match then print that to stdout
    if(n==0){
      printf("%s %d\n", guess, i);
    }

  }

}

void checkPwd6sha256(char* guess){
  // checks guesses against all the 6 character hashes
  fseek(fp1, 0, SEEK_SET);
  int n;
  // iteates through all hashes
  for (int i = 1; i <= 20; ++i)
  {
    fread(buffer, 32, 1, fp1);
    n = memcmp(buf, buffer, sizeof(buffer));
    // If there is a match then print that to stdout
    if(n==0){
      printf("%s %d\n", guess, i + 10);
    }

  }

}



void passwordCompare(){
  // iterates through passwords checks them against the SHA hashes
  while(fscanf(passwordfile, "%s", word)!=EOF){
    checkSHApasswordfile(word);
  }
}

void checkSHApasswordfile(char* guess){
  // Checks all the hashes in the SHA file
  fseek(SHApasswordfile, 0, SEEK_SET);
  int n;
  // converts the guess into a SHA
  ByteArray2SHA(guess);
  for (int i = 1; i <= fileSize; ++i){
    fread(buffer, 32, 1, SHApasswordfile);
    n = memcmp(buf, buffer, sizeof(buffer));
    if(n==0){
      printf("%s %d\n", guess, i);
    }
  }
}

void ByteArray2SHA(char* str) {
  int len = strlen(str);
  BYTE hash[len];
  // converts string to hash
  string2ByteArray(str, hash);
  // initialises hash
  sha256_init(&ctx);
  sha256_update(&ctx, hash, len);
  // finalises
  sha256_final(&ctx, buf);
}


void find6Passwords(){
  // loops through all preprocessed passwords
  // finds the sha hash then compares it to the other hash list
  while(fscanf(createdPasswords, "%s", word)!=EOF){
    //printf("%s\n", word);
    ByteArray2SHA(word);
    checkPwd6sha256(word);
  }
}

void outputPasswords(long total){
  // loops through all preprocessed passwords
  // once that list is exhausted it brute forces 6 digit characters
  while((fscanf(createdPasswords, "%s", word)!=EOF) && (count < total)){
    printf("%s\n", word);
    count++;
  }
  bruteSequential(6);
}


void string2ByteArray(char* input, BYTE* output) {
  //function to convert string to byte array
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
  // recursively goes thought everything in the alphabet in brute force style
    for (int i = 0; i < alphabetSize; ++i)
    {
        str[index] = alphabet[i];

        if (index == maxDepth - 1){
          //printf("string %s converts to: ", str);
          // for (int k = 0; k < SHA256_BLOCK_SIZE; ++k) {
          //   printf("%02x", buf[k]);
          // }
          //printf("\n");
          if (maxDepth == 6){
            if (count >= total){
              exit(0);
            }
            count++;
          }
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
  // makes a string of max length ()
    char* buf = malloc(maxLen + 1);
    memset(buf, 0, maxLen + 1);
  // recursively finds all options
    bruteImpl(buf, 0, maxLen);
    free(buf);
}

void getAlphabet()
{
  // interates through alll characters of the ASSCII table
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
    // Opens files
    //fp = fopen("pwd4sha256", "r");
    //bruteSequential(4);
    fp1 = fopen("pwd6sha256", "r");
    createdPasswords = fopen("edited_passwords.txt", "r");
    find6Passwords();

    fclose(fp1);
    fclose(createdPasswords);
    //fclose(fp);
    return 0;
  }
  if (argc == 2){
    // finds total
    total = atoi(argv[1]);
    // Opens files
    createdPasswords = fopen("edited_passwords.txt", "r");
    outputPasswords(total);
    fclose(createdPasswords);
    return 0;
  }
  if (argc == 3){
    // Opens files
    passwordfile = fopen(argv[1], "r");
    SHApasswordfile = fopen(argv[2], "r");
    // finds the length of the SHA file
    fseek(SHApasswordfile, 0 , SEEK_END);
    fileSize = ftell(SHApasswordfile) / 32;
    fseek(SHApasswordfile, 0 , SEEK_SET);

    passwordCompare();

    fclose(passwordfile);
    fclose(SHApasswordfile);
  }
}
