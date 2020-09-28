/*
 * standardMemMang.c
 *
 *  Created on: 18-Oct-2012
 *      Author: Akhil Piplani
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <errno.h>

#include <stdlib.h>
#include <unistd.h>

//Used for heap size estimate
static int heapBytesRemaining=0;

void *pvPortMalloc( size_t xSize ) {
	return malloc(xSize);
}

void vPortFree( void *pv ) {
	free(pv);
}

void vPortInitialiseBlocks( void ) {

}

size_t xPortGetFreeHeapSize( void ) {
	//This gives a rough estimate of how much heap space is used
	return heapBytesRemaining;
}

void *_sbrk(ptrdiff_t increment) {

	extern char _sheap_, _eheap_;
	static void *cur_heap_pos = 0;

	//Making it reentrant in case someone has crazy thoughts and actually calls
	// this function despite malloc being readily available
	vTaskSuspendAll();

	if(cur_heap_pos == 0) {
		cur_heap_pos = &_sheap_;
		heapBytesRemaining = (int)&_eheap_ - (int)&_sheap_;
	}

	if((cur_heap_pos + increment) > (void *) &_eheap_) {
#if( configUSE_MALLOC_FAILED_HOOK == 1 )
		extern void vApplicationMallocFailedHook( void );
		vApplicationMallocFailedHook();
#else
		//NOTE: Last parameter have to match length of string!
		write(1, "\n\r _sbrk: Out of heap-space! \n\r", 32);
		errno = ENOMEM;
#endif
		xTaskResumeAll();
		return (void *) -1;
	}

	//We could do stack collision detection here,
	// except that FreeRTOS use heap for that...

	void * old_heap_pos = cur_heap_pos;
	cur_heap_pos += increment;
	heapBytesRemaining -= increment;
	xTaskResumeAll();
	return old_heap_pos;
}

//Providing hooks to attempt to make newlib reentrant
//The following lines are from http://www.nadler.com/embedded/newlibAndFreeRTOS.html
#if !defined(configUSE_NEWLIB_REENTRANT) ||  (configUSE_NEWLIB_REENTRANT!=1)
  #warning "#define configUSE_NEWLIB_REENTRANT 1 // Required for thread-safety of newlib sprintf, strtok, etc..."
  // If you're *really* sure you don't need FreeRTOS's newlib reentrancy support, remove this warning...
#endif

void __malloc_lock()     {       vTaskSuspendAll(); };
void __malloc_unlock()   { (void)xTaskResumeAll();  };

// newlib also requires implementing locks for the application's environment memory space,
// accessed by newlib's setenv() and getenv() functions.
// As these are trivial functions, momentarily suspend task switching (rather than semaphore).
// TODO: This should be moved as it has nothing to do with memory management
void __env_lock()    {       vTaskSuspendAll(); };
void __env_unlock()  { (void)xTaskResumeAll();  };
