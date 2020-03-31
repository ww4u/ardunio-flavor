#include "Arduino.h"
//#include "ConfigFile.h"
#include <EEPROM.h>
#include "StringFile.h"
#include "FunctionFile.h"

String rxCache;
cmdcache cmdBuff;

extern coretimer minute;
extern pumptimer pump[PUMP_LIMIT];

int SerialStringRece(void)
{       
    rxCache = Serial.readStringUntil('\n');        
    rxCache.toCharArray( cmdBuff.cache, rxCache.length()+1 );
#ifdef DEBUG
    Serial.println(rxCache);
    Serial.println(cmdBuff.cache);
    Serial.print(" >> ");
    Serial.println(rxCache.length()+1 );  
#endif
 
    cmdBuff.psub[0] = strtok(cmdBuff.cache, " ");        
    cmdBuff.suffix = strtok(NULL, "\n");

    cmdBuff.psub[1] = strtok(cmdBuff.psub[0], ":");
    cmdBuff.psub[2] = strtok(NULL, ":");
    //int i = 2;
    //while( cmdBuff.psub[i] = strtok(NULL, ":") ){ i++; }  

#ifdef DEBUG
    Serial.println(cmdBuff.cache);
    Serial.print(cmdBuff.psub[1]);
    Serial.print(" >> ");
    Serial.print(cmdBuff.psub[2]);
    Serial.print(" >> ");
    Serial.println(cmdBuff.suffix);    
#endif

    if( strcmp( cmdBuff.psub[1], "SYRUP" ) != 0 ){
        return -1;    //设备不符      
    }
    return 0;
}

int workStatusBack( int index )
{
    switch( pump[index].workStatus ){
        case 0:
            Serial.println( "IDLE" );break;
        case 1:
            Serial.println( "BUSY" );break;
        case 2:
            Serial.println( "CLEAN" );break;
        case 3:
            Serial.println( "CALIBRATE" );break;
        case -1:
            Serial.println( "ERROR" );break;
        default:
            Serial.println( "BUG" );break;  
    }
    return pump[index].workStatus;
}

int SerialStringProcess(void)
{
    int index = 0;
    unsigned long num = 0;
    
    if( strcmp( cmdBuff.psub[2], "WEIGHT" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d,%d", &index, &num );
        if( index == 0 ){ return -1; }
        pump[index].weightTatol += num;
        saveInt( TOTAL_ADDR + index*2, pump[index].weightTatol );
        pumpWork( index, num*pump[index].ratio );
        pump[index].workStatus = 1;
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "TIME" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d,%d", &index, &num );     
        if( index == 0 ){ return -1; }
        pumpSecond( index, num );
        pump[index].workStatus = 1;
        return 0;      
    }
    if( strcmp( cmdBuff.psub[2], "RATIO?" ) == 0 ){
        //sscanf( cmdBuff.suffix, "%d", &index );
        //if( index == 0 ){ return -1; }
        for(int i=1; i<PUMP_LIMIT; i++){
            Serial.println( pump[i].ratio );
        }
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "RATIO" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d,%d", &index, &num );
        //for(int i=1; i<PUMP_LIMIT; i++){
        //    pump[i].ratio = num;
        //    saveInt( RATIO_ADDR + i*2, pump[i].ratio);
        //}
        pump[index].ratio = num;
        saveInt( RATIO_ADDR + index*2, pump[index].ratio);
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "CALIBRATE" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d", &index);
        if( index == 0 ){ return -1; }
        pumpSecond( index, 10);
        pump[index].workStatus = 3;
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "MEASUREMENTS" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d,%d", &index, &num );
#ifdef DEBUG
        Serial.print(index);
        Serial.print(" >> ");
        Serial.println(num);
#endif
        if( index == 0 ){ return -1; }
        pump[index].ratio = (int)( 10000000/num );
        saveInt( RATIO_ADDR + index*2, pump[index].ratio);
        return 0;
    }    
    if( strcmp( cmdBuff.psub[2], "TOTAL?" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d", &index );
        if( index == PUMP_ALL ){
            for(int i=1; i<PUMP_LIMIT; i++){
                Serial.println( pump[i].weightTatol );
            }
        }
        else{        
            Serial.println( pump[index].weightTatol );
        }
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "CLEARACC" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d", &index );
        if( index == 0 ){ return -1; }
        //if( index == PUMP_ALL)
        //{
        //    for(int i=1; i<PUMP_LIMIT; i++){
            pump[index].weightTatol = 0;
            minute.mintueTime[index] = 0;
            saveInt( TOTAL_ADDR + index*2, pump[index].weightTatol );
            saveLong( MINUTE_ADDR+ index*4, minute.mintueTime[index]);
        //    }
        //}
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "STORAGE?" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d", &index );
        if( index == PUMP_ALL ){
            for(int i=1; i<PUMP_LIMIT; i++){
                 Serial.println( minute.mintueTime[i] );
            }
        }
        else{
            Serial.println( minute.mintueTime[index] );
        }
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "MINUTE" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d,%d", &index, &num );
        if( index == 0 ){ return -1; }
        pumpMinute( index, num );
        pump[index].workStatus = 1;
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "CLEAN" ) == 0 ){
        pumpClean();
        for( int i=1; i<PUMP_LIMIT; i++){
            pump[i].workStatus = 2;
        }
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "RECORD?" ) == 0 ){
        Serial.println( readLong( RECOND_ADDR+4 ) );
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "FILL" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d", &index );
        if( index == 0 ){ return -1; }
        pumpWork( index, pump[index].pumpLength );
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "LENGTH?" ) == 0 ){
        //sscanf( cmdBuff.suffix, "%d", &index);
        //if( index == 0 ){ return -1; }
        for( int i=1; i<PUMP_LIMIT; i++){
            Serial.println( pump[i].pumpLength );
        }
        return 0;
    }    
    if( strcmp( cmdBuff.psub[2], "LENGTH" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d", &num );
        //if( index == 0 ){ return -1; }
        for( int i=1; i<PUMP_LIMIT; i++){
            pump[i].pumpLength = num;
            saveLong( LENFTH_ADDR+ i*4, pump[i].pumpLength);
        }
        return 0;
    }
    if( strcmp( cmdBuff.psub[2], "START" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d", &index);
        pumpRun(index, true);
        pump[index].workStatus = 1;
        return 0;
    }
    if( strcmp(cmdBuff.psub[2], "STOP" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d", &index);
        //if( index == 0 ){ return -1; }
        if( index == PUMP_ALL ){ 
            for( int i=1; i<PUMP_LIMIT; i++){
                pumpRun(i, false);
                pump[i].workStatus = 0;
            }
        }
        else{
            pumpRun(index, false);
            pump[index].workStatus = 0;
        }       
        return 0;
    }
    if( strcmp(cmdBuff.psub[2], "STATUS?" ) == 0 ){
        sscanf( cmdBuff.suffix, "%d", &index);
        if( index == PUMP_ALL ){
            for(int i=1; i<PUMP_LIMIT; i++){
                 workStatusBack(i);
            }
        }
        else{
            workStatusBack(index);
        }
        return 0;
    }
    if( strcmp(cmdBuff.psub[2], "VERSION?" ) == 0 ){
        Serial.println( "00.01" );
        return 0;
    }
    return -2;
}

int saveInt( int address, int data )
{
    dataInt unidata;
    unidata.data = data;
    for(int i=0;i<2;i++)
    EEPROM.write(address+i, unidata.b[i]);
    return 0;
}
int readInt( int address )
{
    dataInt unidata;
    for(int i=0;i<2;i++)
    unidata.b[i]=EEPROM.read(address+i);
    return unidata.data;
}
int saveLong( int address, unsigned long data )
{
    dataLong unidata;
    unidata.data = data;
    for(int i=0;i<4;i++)
    EEPROM.write(address+i, unidata.b[i]);
    return 0;
}
unsigned long readLong( int address )
{
    dataLong unidata;
    for(int i=0;i<4;i++)
    unidata.b[i]=EEPROM.read(address+i);
    return unidata.data;
}
