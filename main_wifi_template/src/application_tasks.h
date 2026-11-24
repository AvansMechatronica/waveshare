/*
 * application_tasks.h
 *
 * Created: 27-11-2023 15:01:19
 *  Authors: 	Roel Smeets & Gerard Harkema
 *  Revision: V2.0
 *  Modified: 07-09-2026
 */ 


#ifndef APPLICATIONTASKS_H_
#define APPLICATIONTASKS_H_



///////////////////////////////////////////////////////////////////////////////
// objects made available for external use


///////////////////////////////////////////////////////////////////////////////
// function prototypes
void StartApplicationTasks(void);
bool startMyTask(void);
void MyTask(void *pvParameters);

#endif /* APPLICATIONTASKS_H_ */