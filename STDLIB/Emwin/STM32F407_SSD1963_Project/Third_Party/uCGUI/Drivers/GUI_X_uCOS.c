#include <includes.h>
#include "GUI_Private.H"
#include "stdio.H"

static  OS_EVENT  *DispSem;
static  OS_EVENT  *EventMbox;
static  OS_EVENT  *KeySem;
static  int        KeyPressed;
static  char       KeyIsInited;


/**********************************************************************************************************
*                                        TIMING FUNCTIONS
* Notes: Some timing dependent routines of uC/GUI require a GetTime and delay funtion. 
*        Default time unit (tick), normally is 1 ms.
**********************************************************************************************************/
int GUI_X_GetTime(void)
{
   	return ((int)OSTimeGet());
}

void GUI_X_Delay(int period)
{
  	INT32U  ticks;
  	ticks=(period*1000)/OS_TICKS_PER_SEC;
  	OSTimeDly((INT16U)ticks);
}


/**********************************************************************************************************
*                                          GUI_X_ExecIdle()
**********************************************************************************************************/
/*WM空闲时调用*/
void GUI_X_ExecIdle(void) 
{
  	OSTimeDly(50);
}


/**********************************************************************************************************
*                                    MULTITASKING INTERFACE FUNCTIONS
* Note(1): 1) The following routines are required only if uC/GUI is used in a true multi task environment, 
*             which means you have more than one thread using the uC/GUI API.  In this case the #define 
*             GUI_OS 1   needs to be in GUIConf.h
**********************************************************************************************************/
void GUI_X_InitOS (void)
{ 
    DispSem = OSSemCreate(1);           //建立一个互斥型信号量
  EventMbox = OSMboxCreate((void *)0);	//建立一个邮箱
}


void GUI_X_Lock(void)
{ 
  INT8U err;
  OSSemPend(DispSem,0,&err);
}


void GUI_X_Unlock(void)
{ 
  OSSemPost(DispSem);
}


U32 GUI_X_GetTaskId(void)
{ 
  return ((U32)(OSTCBCur->OSTCBPrio));
}

/**********************************************************************************************************
*                                        GUI_X_WaitEvent()
*                                        GUI_X_SignalEvent()
**********************************************************************************************************/


void GUI_X_WaitEvent(void)
{
   INT8U err;
   (void)OSMboxPend(EventMbox,0,&err);
}


void GUI_X_SignalEvent(void)
{
  (void)OSMboxPost(EventMbox,(void *)1);
}

/**********************************************************************************************************
*                                      KEYBOARD INTERFACE FUNCTIONS
*
* Purpose: The keyboard routines are required only by some widgets.
*          If widgets are not used, they may be eliminated.
*
* Note(s): If uC/OS-II is used, characters typed into the log window will be placed	in the keyboard buffer. 
*          This is a neat feature which allows you to operate your target system without having to use or 
*          even to have a keyboard connected to it. (useful for demos !)
**********************************************************************************************************/

static void CheckInit(void)
{
  if(KeyIsInited==DEF_FALSE)
    {
      KeyIsInited = DEF_TRUE;
      GUI_X_Init();
    }
}


/*被GUI_Init()调用,用来初始化一些GUI运行之前需要用的硬件,如键盘或者鼠标之类的.如果不需要的话,可以为空*/
void GUI_X_Init(void)
{
  KeySem = OSSemCreate(0);
}


int GUI_X_GetKey(void)
{
    int r;
    r = KeyPressed;
    CheckInit();
    KeyPressed = 0;
    return (r);
}


int GUI_X_WaitKey(void)
{
    int  r;
    INT8U  err;
    CheckInit();
    if(KeyPressed==0)
      {
        OSSemPend(KeySem,0,&err); //等待信号量
      }
    r= KeyPressed;
    KeyPressed = 0;
    return (r);
}


void GUI_X_StoreKey(int k)
{
    KeyPressed = k;
    OSSemPost(KeySem); //释放信号量
}

void GUI_X_Log(const char *s) 
{ 
 GUI_USE_PARA(s); 
}

void GUI_X_Warn(const char *s) 
{
 GUI_USE_PARA(s); 
}

void GUI_X_ErrorOut(const char *s)
{ 
GUI_USE_PARA(s); 
}
