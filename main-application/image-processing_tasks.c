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
        const char *pcTaskName = "imagePreperationTask is running\r\n"
        uint8_t version = version(); // version is afunction we assumed will return which version we want from a telecommand
        uint8_t frameT = 32; // frames, telecommand ID 64
        uint8_t frameI = 8192; // frames, telecommand ID 64
        uint32_t img = cubesenseMemory(); // would this exist?
        // for the cubesenseMemory function it would have to be faster then then
        // periodic trigger on the capture Task
        uint8_t imageDownload;
        uint8_t compressed
        if version == 0{    // thumbnail version
            imageDownload = initalizeImageDownload(frameT, img)
            if checkDownloadResults(imageDownload){
                if qualityCheck(imageDownload){
                    compressed = compress(imageDownload)
                    prepDownlink(compressed)
                }
            }
        }

        if version == 1{    // Full image version
            imageDownload = initalizeImageDownload(frameI, img)
            if checkDownloadResults(imageDownload){
                compressed = compress(imageDownload)
                prepDownlink(compressed)
            }
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
    while(1){
    const char *pcTaskName = "captureTask is running\r\n";
    uint_8 ready = cameraReadyCheck();
        if ready == 1{ //assuming 1 is True (Ready)
            captureImage(SRAM)
        }

    }
}
