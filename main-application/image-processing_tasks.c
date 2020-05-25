void imagePreperationTask(void* pvParameters){
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
    while(1){
        const char *pcTaskName = "imagePreperationTask is running\n"
        uint_8 imageDownload;
        uint_8 version = initalizeImageDownloadType()
        unit_8 img = initalizeImageDownload()
        uint_8 compressed


        if version == 1{ // full photo
            if checkDownloadResults(img) == 1{
                compressed = compressImage(img)
                prepDownlink(compressed) // is this proper c code?
                break // we want to exit

            }
        }
        else if (version == 0){
            if checkDownloadResults(img) == 1{
                uint_8 check = qualityCheck(img)
                if check == 1{
                    compressed = compressImage(img)
                    prepDownlink(compressed) // is this proper c code?
                    break // we want to exit
                }
                else if (check == 0){
                    break // is this how to exit the while?
                }
            }
        }


void captureTask(){
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
    const char *pcTaskName = "captureTask is running\n"
    uint_8 waitFlag;
    uint_8 CapturePaused;
    uint_8 SuperResolution;
    uint_8 ready = cameraReadyCheck();

    while(1){
        if (CapturePaused == 1){ // 1 = True
            break // we want to exit
        }
        else if (CapturePaused == 0){
            if (SuperResolution == 1){ // activated
                if ready == 1{
                    captureImage() // this function puts it in the memory
                    if (waitFlag == 1){ // full
                        break // we want to exit
                    }
                }
            }
            else if (SuperResolution == 0){
                if ready == 1{
                    captureImage() // this function puts it in the memory
                    break // we want to exit
                }
            }
        }
    }
}
