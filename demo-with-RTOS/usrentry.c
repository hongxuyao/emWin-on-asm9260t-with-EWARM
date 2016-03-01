#include "usrinc.h"
#include <string.h>

/** @addtogroup Glory_EmbeddedWorks
 * @{
 */

/**
 * function description
 *
 * @param  none
 * @return none
 *
 * @brief  none
 */
void UserEntryInit(void)
{
  // 在这里创建线程。
  
  
  // 创建GUI线程。
  if(OS_ERR_NONE != OSTaskCreate(
      task_gui, NULL,
      &task_gui_stk[TASK_GUI_STK-1],
      TASK_GUI_PRIO))
  {
    DBG_PUTS("\ntask-gui create failed");
    sys_suspend();
  }
}

void UserEntryLoop(void)
{
  
  while(1)
  {
    LED_Toggle();
    sys_delay(500);
  }
}




/**
 * @}
 */

