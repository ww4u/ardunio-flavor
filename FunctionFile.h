//--<Head>--
#ifndef FUNCTION_FILE
#define FUNCTION_FILE

#define PUMPOPEN     LOW
#define PUMPCLOSE    HIGH
#define PUMPMINUTE   3
#define PUMP_LIMIT   6
#define PUMP_ALL     0
#define RATIO_ADDR   0
#define TOTAL_ADDR   20
#define MINUTE_ADDR  40  
#define LENFTH_ADDR  80 
#define RECOND_ADDR  120

typedef struct coretimer
{
    unsigned long startTime;
    unsigned long deltaTime;
    unsigned long mintueTime[PUMP_LIMIT];
    unsigned long recondTime;
    bool isOpen;    
}coretimer;

typedef struct pumptimer
{  
    unsigned long startTime;
    unsigned long deltaTime;
    
    unsigned long pumpLength;
    int weightTatol;
    int ratio;
    
    int workStatus;
    int port;
    bool isOpen;    
}pumptimer;

void initPara(void);
int pumpTimeCheck(void);
int minutesTimeCheck(void);
void pumpWork(int index, unsigned long ml_time);
void pumpSecond(int index, int sec_time);
void pumpMinute(int index, int min_time);
void pumpRun(int index, bool run_state);
void pumpClean(void);

#endif
