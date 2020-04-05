void imagePreperationTask(imageID, SRAM, thumbnail = False):
    /**
    * @brief prepare the image for processing
    * @header software-and-command/main-application
    * @param image - is the image that will be processes
    *        SRAM - is the where in the storage the image will be stored
    * @pre None
    * @post Stores the image into the memory storage
    * @return None
    */
    while(1){
        uint_8 img = retrieveImage(imageID, thumbnail)
        uint_8 compressed = compression(img) // idk what the compression means entirely -Addi
        uint_8 prepared = prepDownlink(compressed) // turns it to frames?
        // somehow send it to downlink manager
            // is this suppose to even be in a while(1)?
        //

    }
    return 0

    // ask lars if 'thumbnail = False' is allowed in C
