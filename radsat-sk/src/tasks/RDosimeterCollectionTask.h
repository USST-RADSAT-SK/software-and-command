/**
 * @file RDosimeterCollectionTask.h
 * @date December 28, 2021
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */


#ifndef RPAYLOADCOLLECTIONTASK_H_
#define RPAYLOADCOLLECTIONTASK_H_


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void DosimeterCollectionTask(void* parameters);

/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/


int checkFlags();


#endif /* RPAYLOADCOLLECTIONTASK_H_ */
