//--<Head>--
#ifndef STRING_FILE
#define STRING_FILE

//--<Include>--
#include "string.h"

//--<union>--
union dataLong{
    unsigned long data;
    byte b[4];
};
union dataInt{
    int data;
    byte b[2];
};

//--<TypeDefine>--
typedef struct cmdcache{
    char cache[48];
    char * psub[3];
    char * suffix;
}cmdcache;

//--<Declaration>--
int SerialStringRece(void);
int SerialStringProcess(void);
int saveInt( int address, int data );
int readInt( int address);
int saveLong( int address, unsigned long data );
unsigned long readLong( int address);

#endif
