/*
 * application_tasks.cpp
 *
 * Created: 27-11-2023 14:57:44
 *  Authors: 	Roel Smeets & Gerard Harkema
 *  Revision: V2.0
 *  Modified: 07-09-2026
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <Arduino.h>
#include "WS_Bluetooth.h"
#include "WS_GPIO.h"
#include "WS_RS485.h"
#include "WS_CAN.h"
#include "WS_RTC.h"
#include "WS_DIN.h"
#include "WS_SD.h"
#include "WS_ETH.h"

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "application_tasks.h"

///////////////////////////////////////////////////////////////////////////////
// application tasks handler declarations

static TaskHandle_t myTaskHandle = NULL;


///////////////////////////////////////////////////////////////////////////////
// hardware handle shared by all application tasks



///////////////////////////////////////////////////////////////////////////////
// void StartApplicationTasks(void)

void StartApplicationTasks(void)
{
	// TODO: Start application tasks here
	startMyTask();
}

// TODO: Implement task starting functions 
bool startMyTask(void)
{
	BaseType_t result = pdFAIL;

	result = xTaskCreate(MyTask, "tsk_MyTask", 4 * (configMINIMAL_STACK_SIZE), NULL, 0, &myTaskHandle);
	if (result == pdPASS)
	{
		return true;
	}
	return false;
}


// TODO: Implement tasks
void MyTask(void *pvParameters)
{


	// Task implementation goes here

	for (;;)
	{
		// Task loop
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}	