#include "usrinc.h"
#include "DIALOG.h"
#include "GUI.h"

// http://www.segger.com/emwin-samples.html
extern void MainTask(void);

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: task_ui
//| 功能描述 |: GUI应用线程循环。
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |: GUI线程堆栈安排在SDRAM中，因为它需要较大堆栈空间！
//|          |:
////////////////////////////////////////////////////////////////////////////////
OS_STK task_gui_stk[TASK_GUI_STK] @ ".noinit";
void task_gui(void *p_arg)
{
  // 初始化LCD控制器硬件。
  if(!LCDHW_Init()){
    sys_suspend();
  }
  
  // emWin延时代码。
  MainTask();
}

