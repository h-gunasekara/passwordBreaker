// this brute force approach was adapted from syb0rg on codereview.stackexchange
// https://codereview.stackexchange.com/questions/38474/brute-force-algorithm-in-c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char alphabet[126 - 32 + 1];

static const int alphabetSize = sizeof(alphabet) - 1;

alphabet();

void bruteImpl(char* str, int index, int maxDepth)
{
    for (int i = 0; i < alphabetSize; ++i)
    {
        str[index] = alphabet[i];

        if (index == maxDepth - 1) printf("%s\n", str);
        else bruteImpl(str, index + 1, maxDepth);
    }
}

void bruteSequential(int maxLen)
{
    char* buf = malloc(maxLen + 1);

    for (int i = 1; i <= maxLen; ++i)
    {
        memset(buf, 0, maxLen + 1);
        bruteImpl(buf, 0, i);
    }

    free(buf);
}

void alphabet()
{
  for( int i = 32; i <= 126; i++ )
  {
      alphabet[i-32] = (char) i;
  }
  alphabet[126 - 32 + 1] = '\0';
}

int main(int argc, char * argv[])
{
  alphabet();
  if (argc == 1){
    bruteSequential(4);
    return 0;
  }

}
