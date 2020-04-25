void imagePreperationTask(imageID, SRAM, verison):
    /**
    * @brief prepare the image for processing
    * @header software-and-command/main-application
    * @param imageID - is the image that will be processes
    *        SRAM - is the where in the storage the image will be stored
    *        version - thumbnail == 0, full photo == 1
    * @pre None
    * @post Stores the image into the memory storage
    * @return None
    */
    while(1){
        if version == 0:
            {
            // find a way to reference the image from the cubesense memory
            // maybe have capture task return the location?
            imageDownload = initalizeImageDownload(32frames)
             if checkDownloadResults(imageDownload):
                {
                if qualityCheck(imageDownload):
                    {
                    compressed = compressThumbnail(imageDownload)
                    prepDownlink(imageDownload)
                    }
                }
            }

        if version == 1:
            {
            // find a way to reference the image from the cubesense memory
            // maybe have capture task return the location?
            imageDownload = initalizeImageDownload(8192frames)
            if checkDownloadResults(imageDownload):
                compressed = compressThumbnail(imageDownload)
                prepDownlink(imageDownload)
            }
    }
    return 0



void captureTask():
    /**
    * @brief Image capture task
    * @header software-and-command/main-application
    * @param None
    * @pre None
    * @post Store one image into the Cubesense memory
    * @return None
    */
    while(1){
    uint_8 cameraReadyCheck()
    if cameraReadyCheck == 1 //assuming 1 is True (Ready)
        img = captureImage(SRAM) // where do we put the captured image in the cubesense memory

    }
    return 0

    // This will have to return the location of the photo in the cubesense memory
    // because we call in the imagePreperationTask



    // discarded notes and code: ----------------------------------------------------------
    // uint_8 img = retrieveImage(imageID) // assuming retrieveImage dereferences for us
    // uint_8 compressed = compression(img) // idk what the compression means entirely -Addi
    // uint_8 prepared = prepDownlink(compressed) // turns it to packets?
    // // somehow send it to downlink manager
    //     // is this suppose to even be in a while(1)?
    // // we dont know what the downlink manager is expecting? so how do we send it?
    // downLinkManager(prepared)
    // // Downlink manager isn't where its supposed to go i think
