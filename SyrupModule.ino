
//--<#define>--

#include <stdio.h>
#include "ConfigFile.h"
#include "StringFile.h"
#include "FunctionFile.h"

//--<Global&Cache>--

//--<initFun>--
void setup()
{
    Serial.begin(9600);
    for (int i = 2; i < PUMP_LIMIT+1; i++)
    {
        pinMode(i, OUTPUT);
        digitalWrite(i, PUMPCLOSE);
    }
    initPara();
//#ifdef DEBUG
    Serial.println( "BootEnd" ); 
//#endif
}

//--<mainLoop>--
unsigned char indexLoop = 0;
void loop()
{
    if ( Serial.available()>0 )
    {
//        delay(50);        //48Bytes
        if( !SerialStringRece() ){
            SerialStringProcess();
        }
        else{
            Serial.println( "CmdError" );
        }
    }
    pumpTimeCheck();
    if(indexLoop % 64 == 0){
        minutesTimeCheck();
    }
    indexLoop++;
    delay(20);    
}
