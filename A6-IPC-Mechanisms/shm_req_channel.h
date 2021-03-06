//
// Created by toni on 11/16/18.
//

#ifndef A6_IPC_MECHANISMS_SHMREQUESTCHANNEL_H
#define A6_IPC_MECHANISMS_SHMREQUESTCHANNEL_H
#import "requestchannel.h"
#include "shm_bounded_buffer.h"
#include <sys/shm.h>
#include <vector>

#define SHM_SIZE 32  /* make it a 1K shared memory segment. Made mine 32 to have the clearing run faster. */


class SHMRequestChannel : public RequestChannel {
    
private:
    Side     my_side;
    //char *data;
    SHMBoundedBuffer* bb1;
    SHMBoundedBuffer* bb2;
    vector<string> filenames;


public:

    SHMRequestChannel ( const string _name , const Side _side);
    SHMRequestChannel();

    string cread();
    /* Blocking read of data from the channel. Returns a string of
    characters read from the channel. Returns NULL if read failed. */

    int cwrite(string msg);

    int createQueue(string mqFileName, Mode mode);
    string mq_name(Mode _mode);

    string getServerOrClient();
    int getServerWriteMemSegId();
    int getServerReadMemSegId();

    ~SHMRequestChannel();
    /* Write the data to the channel. The function returns
    the number of characters written to the channel. */
};


#endif //A6_IPC_MECHANISMS_SHMREQUESTCHANNEL_H
