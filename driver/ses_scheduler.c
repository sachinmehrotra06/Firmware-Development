/*INCLUDES ************************************************************/
#include "ses_timer.h"
#include "ses_scheduler.h"
#include "util/atomic.h"
#include "ses_button.h"
#include "ses_led.h"
#include "ses_lcd.h"
#include "ses_uart.h"

/* PRIVATE VARIABLES **************************************************/
/** list of scheduled tasks */
static taskDescriptor* taskList = NULL;

static systemTime_t curr_time;

/*FUNCTION DEFINITION *************************************************/
static void scheduler_update(void) {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

		curr_time++;

		taskDescriptor * temp = taskList;
		while(temp != NULL)
		{
			if(!(temp->execute))
			{
				if(temp->period != 0 || temp->expire != 0)
				{
					//update the expire duration for periodic tasks
					temp->expire -= 1;
				}
				//marking the expired tasks and setting period again in expire
				if(temp->expire == 0)
				{
					temp->execute = 1;
					temp->expire = temp->period;
				}
			}
			temp = temp->next;
		}
	}

}

void scheduler_init()
{
	//initializing the timer2
	timer2_start();
	//setting the function pointer to run schedule
	timer2_setCallback(&scheduler_update);
}

void scheduler_run() {

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		taskDescriptor * temp = taskList;

		for(;temp != NULL; temp = temp->next)
		{
			//executing the next task marked
			if(temp->execute == 1)
			{
				temp->task(temp->param);
				//if task is not periodic removing from memory
				if(temp->period == 0 && temp->expire == 0)
					scheduler_remove(temp);
				else
					temp->execute = 0;
			}
		}
	}
}

bool scheduler_add(taskDescriptor * toAdd) {
	taskDescriptor* temp1;
	taskDescriptor *temp2;

	bool val = true;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

		temp1 = taskList->next;
		temp2 = taskList;


		//checking the whether invalid task or not
		if(toAdd != NULL)
		{
			toAdd->next = NULL;
			//checking if there is no task at all in the list if not add first node as toAdd
			if(taskList != NULL)
			{
				for(; temp1 != NULL; temp2 = temp1, temp1 = temp1->next)
				{
					//checking whether the task is scheduled
					if(temp1 == toAdd)
					{
						val = false;
						break;
					}
				}
				//if task is not scheduled and there are already few tasks in the list then toAdd is added in the end
				if(temp1 == NULL && val)
				{
					temp2->next = toAdd;
					toAdd->next = NULL;
				}
			}
			else
			{
				taskList = toAdd;
				toAdd->next = NULL;
			}
		}
		else
			val = false;

	}
	return val;
}

void scheduler_remove(taskDescriptor * toRemove) {

	taskDescriptor * temp1;
	taskDescriptor * temp2;

	//checking whether first node(task) is supposed to be removed
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

		temp1 = taskList->next;
		temp2 = taskList;

		if(toRemove != taskList)
		{
			while(temp1 != NULL)
			{
				//removing node in the list
				if(temp1 == toRemove)
				{
					temp2->next = temp1->next;
					temp1->next = NULL;
					temp1 = NULL;
				}
				temp2 = temp1;
				temp1 = temp1->next;
			}
		}
		else
		{
			//for removing first node(task) from the list
			taskList = temp1;
			temp2->next = NULL;
			temp2 = NULL;
		}
	}
}

systemTime_t scheduler_getTime()
{
	return curr_time;
}

void scheduler_setTime(systemTime_t time)
{
	curr_time = time;
}
