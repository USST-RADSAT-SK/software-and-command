void imagePreperationTask(imageID, SRAM):
    /**
    * @brief prepare the image for processing
    * @header software-and-command/main-application
    * @param imageID - is the image that will be processes
    *        SRAM - is the where in the storage the image will be stored
    * @pre None
    * @post Stores the image into the memory storage
    * @return None
    */
    while(1){
        uint_8 img = retrieveImage(imageID) // assuming retrieveImage dereferences for us
        uint_8 compressed = compression(img) // idk what the compression means entirely -Addi
        uint_8 prepared = prepDownlink(compressed) // turns it to packets?
        // somehow send it to downlink manager
            // is this suppose to even be in a while(1)?
        // we dont know what the downlink manager is expecting? so how do we send it?
        downLinkManager(prepared)
        // Downlink manager isn't where its supposed to go i think
    }
    return 0

    // ask lars if 'thumbnail = False' is allowed in C
