//--<include>--
#include "Arduino.h"
#include "ConfigFile.h"
#include "FunctionFile.h"
#include "StringFile.h"

coretimer minute;
pumptimer pump[PUMP_LIMIT];

//--<Function>--
void initPara(void)
{
    for (int i = 1; i < PUMP_LIMIT; i++) 
    {
        pump[i].isOpen = false;
        pump[i].port = i + 1;
        pump[i].ratio = readInt( RATIO_ADDR + i*2 );
        pump[i].weightTatol = readInt( TOTAL_ADDR + i*2 );
        pump[i].pumpLength = readLong( LENFTH_ADDR + i*4 );
        minute.mintueTime[i] = readLong( MINUTE_ADDR + i*4 );        
    }
    minute.recondTime = readLong( RECOND_ADDR + 4 );
    minute.isOpen = true;
}

void timeCheck(int index)
{
    if( pump[index].isOpen )
    {
#ifdef DEBUG
        unsigned long millis_time = millis();
        if( (millis_time - pump[index].startTime) >= pump[index].deltaTime )
#else            
        if( ( millis() - pump[index].startTime ) >= pump[index].deltaTime )
#endif
        {
            digitalWrite(pump[index].port, PUMPCLOSE);            
            pump[index].isOpen = false;
            pump[index].workStatus = 0;
#ifdef DEBUG
           Serial.print(millis_time);
           Serial.println(" END");
#endif
        }
    }
}

int pumpTimeCheck(void)
{
    for (int i = 1; i < PUMP_LIMIT; i++){
        timeCheck(i);
    }
}

void pumpWork(int index, unsigned long ms_time)
{
    pump[index].startTime = millis();
    pump[index].deltaTime = ms_time;
    pump[index].isOpen = true;
    digitalWrite( pump[index].port, PUMPOPEN );    
#ifdef DEBUG
    Serial.print("Index >>");
    Serial.print(index);
    Serial.print("  Port >>");
    Serial.println(pump[index].port);    
    Serial.print(pump[index].startTime);
    Serial.println(" BEGIN >> ");
#endif
}

void pumpSecond(int index, int sec_time)
{
    pumpWork(index, (unsigned long) sec_time*1000);
}

void pumpMinute(int index, int min_time)
{
    pumpWork(index, (unsigned long) min_time*60*1000);
#ifdef DEBUG
    Serial.print("Minute >>");
    Serial.println(min_time);
#endif
}

void pumpRun(int index, bool run_state)
{
    pump[index].isOpen = false;
    if(run_state){    
        digitalWrite( pump[index].port, PUMPOPEN );
    }
    else{
        digitalWrite( pump[index].port, PUMPCLOSE );
    }
#ifdef DEBUG
    Serial.print("Index >>");
    Serial.print(index);
    Serial.print("  Port >>");
    Serial.print(pump[index].port);
    Serial.print("  State >> ");
    Serial.println(run_state);
#endif
}

void pumpClean(void)
{
    for(int i=1; i<PUMP_LIMIT; i++){
        pumpMinute( i, PUMPMINUTE );
        delay(200);
    }
    minute.recondTime = 0;
    saveLong( RECOND_ADDR + 4, minute.recondTime); 
    delay(50); 
}

void minuteCheck(void)
{
    if( minute.isOpen )
    {
        if( ( millis() - minute.startTime ) >= minute.deltaTime ) minute.isOpen = false;
    }
}

int minutesTimeCheck(void)
{
    minuteCheck();
    if( !minute.isOpen )
    {
        minute.startTime = millis();
        minute.deltaTime = (unsigned long)1000*60;
        for(int i=1; i<PUMP_LIMIT; i++){
            minute.mintueTime[i]++;
            saveLong( MINUTE_ADDR+ i*4, minute.mintueTime[i]);
        }
        minute.recondTime++;
        saveLong( RECOND_ADDR+ 4, minute.recondTime);
        minute.isOpen = true;        
    }    
}
