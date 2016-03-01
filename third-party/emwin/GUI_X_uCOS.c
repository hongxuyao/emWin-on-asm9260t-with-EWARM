#include "usrinc.h"
#include "GUI.h"

#define GUI_EVENT_ENABLE    (0)

#if defined(GUI_EVENT_ENABLE) && (GUI_EVENT_ENABLE > 0)
static OS_EVENT *GUI_X_Event_Sem = NULL;
#endif

//
// Configuration
//
void GUI_X_Config(void) {
  static u32_t GUIHeapMem[1024*1024];
  GUI_ALLOC_AssignMemory(GUIHeapMem, sizeof(GUIHeapMem));
  GUI_ALLOC_SetAvBlockSize(128);
  GUI_SetDefaultFont(GUI_DEFAULT_FONT);
}

//
// Initialization
//
void GUI_X_Init(void)
{
#if defined(GUI_EVENT_ENABLE) && (GUI_EVENT_ENABLE > 0)
  GUI_X_Event_Sem = OSSemCreate(1);
  if(!GUI_X_Event_Sem){
    DBG_PUTS("\nGUI_X_Event_Sem create faild");
    sys_suspend();
  }
  GUI_SetSignalEventFunc(GUI_X_SignalEvent);
  GUI_SetWaitEventFunc(GUI_X_WaitEvent);
  GUI_SetWaitEventTimedFunc(GUI_X_WaitEventTimed);
#endif
}

//
// Timing routines
//
int  GUI_X_GetTime(void)
{
  return sys_tick;
}

void GUI_X_Delay(int Period)
{
  sys_delay(Period);
}

void GUI_X_ExecIdle(void)
{
  OSTimeDly(1);
}

#if defined(GUI_EVENT_ENABLE) && (GUI_EVENT_ENABLE > 0)
//
// Event driving (optional with multitasking)
//
void GUI_X_SignalEvent(void)
{
  OSSemPost(GUI_X_Event_Sem);
}

void GUI_X_WaitEvent(void)
{
  INT8U err;
  OSSemPend(GUI_X_Event_Sem, 0, &err);
}

void GUI_X_WaitEventTimed(int Period)
{
  INT8U err;
  OSSemPend(GUI_X_Event_Sem, ms2tick(Period), &err);
}

#endif

//
// Multitask routines - required only if multitasking is used (#define GUI_OS 1)
//
void GUI_X_InitOS(void)
{
}

void GUI_X_Lock(void)
{
}

void GUI_X_Unlock(void)
{
}

U32  GUI_X_GetTaskId(void)
{
  return OSPrioCur;
}

//
// Recording (logs/warnings and errors) - required only for higher levels
//
void GUI_X_Log(const char *s)
{
  DBG_PRINT("\nLog:%s", s);
}

void GUI_X_Warn(const char *s)
{
  DBG_PRINT("\nWarn:%s", s);
}

void GUI_X_ErrorOut(const char *s)
{
  DBG_PRINT("\nErr:%s", s);
}

