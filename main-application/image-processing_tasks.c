
/**
* @brief prepare the image for processing
*
* This task will grab a full image from the cubesense memory and initalize
* image download and check the results. If we want the image to be a thumbnail
* it will go through a quality check. The end result will give a compressed file
* that will be prepared and sent to the downlink manager.
* @header software-and-command/main-application
* @param parameters unused
* @pre None
* @post Stores the image into the memory storage
* @return None
*/

void imagePreperationTask(void* pvParameters){
    uint_8 imageDownload;
    uint_8 version
    unit_8 img = initalizeImageDownload()
    uint_8 compressed
    uint_8 check = qualityCheck(img)

    QueueHandle_t xQueueImageVersion;
    xQueueImageVersion = xQueueCreate(MAX_LENGTH, 1)

    while(1){

        if (xQueueImageVersion != NULL){
            if (xQueueRecieve(xQueueImageVersion, &version, portMAX_DELAY) == pdTRUE){

                if (version == 1){ // full photo
                    if checkDownloadResults(img) == 1{
                        compressed = compressImage(img);
                        xQueueSend(xQueueDownLink, &(createPackets(compressed), portMAX_DELAY);
                    }
                }
                else if (version == 0){ // thumbnail
                    if checkDownloadResults(img) == 1{
                        if check == 1{
                            compressed = compressImage(img)
                            xQueueSend(xQueueDownLink, &(createPackets(compressed), portMAX_DELAY);
                        }
                    }
                }
            }
        }
    }
}



/**
* @brief Image capture task
*
* This task will get called from a periodic trigger. After checking if the
* camera is ready to be used, it will take the photo storing it in one of the
* local SRAM memory locations.
* @header software-and-command/main-application
* @param pararmeters unused
* @pre None
* @post Store one image into the Cubesense memory
* @return None
*/

void captureTask(){
    const char *pcTaskName = "captureTask is running\n"
    uint_8 waitFlag;
    uint_8 CapturePaused;
    uint_8 SuperResolution;
    uint_8 i = 0;
    uint_8 ready = cameraReadyCheck();
    uint_8 condition

    QueueHandle_t xQueueSuperResolution;
    xQueueSuperResolution = xQueueCreate(MAX_LENGTH, 1)
    // uxitemsize parameter is 1 byte
    // using the queue to determine if were using a SuperResolution or not
    // We dont know how big the Queue will need to be


    while(1){

        ulTaskNotifyTake(pdTrue, portMax_Delay); //????

        if (xQueueSuperResolution != NULL){
            if (xQueueRecieve(xQueueSuperResolution, &condition, portMAX_DELAY) == pdTRUE){ // activated
                if (condition == 1){
                    while (i < 4){
                        if (ready == 1){
                            captureImage(); // this function puts it in the memory
                            i++;
                            taskYIELD();
                        }
                    }
                }
                else if (condition == 0){ // deactivated
                    if ready == 1{
                        captureImage(); // this function puts it in the memory
                        taskYIELD();
                    }
                }
            }
        }
    }
}
