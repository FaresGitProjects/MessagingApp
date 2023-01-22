#include <stdlib.h>
#include <string.h>

#include "serialize.h"

/* Unpack the given packet into the buffer unpacked.  You can assume
 * that packed points to a packet buffer large enough to hold the
 * packet described therein, but you cannot assume that the packet is
 * otherwise valid.  You can assume that unpacked points to a character
 * buffer large enough to store the unpacked packet, if it is a valid
 * packet.
 *
 * Returns the packet type that was unpacked, or -1 if it is invalid.
 */

int scanForMessage(void *ptr);
int scanForStatus(void *ptr);
int scanForLABELED(void *ptr);


int unpack(char *unpacked, void *packed) {
    //MESSAGE
    int type = *((int *)packed);
    packed += sizeof(int);
    if(type == MESSAGE) {
        char *ubit = ((char *)packed);
        packed += sizeof(char)*NAME_SIZE;
        if(!scanForMessage(packed)) {
            return -1;
        }
        size_t *datalength = ((size_t *)packed);
        packed += sizeof(size_t)*2;
        char *msg = ((char *)packed);

        char c;
        size_t i = 0;
        while((c = ubit[i])!='\0'&& i < NAME_SIZE) {
            unpacked[i] = c;
            i++;
        }
        unpacked[i] = ':';
        unpacked[i+1] = ' ';
        unpacked += i+2;
        i = 0;
        while(i < datalength[0]) {
            c = msg[i];
            unpacked[i] = c;
            i++;
        }
        unpacked[i] = '\0';
        return MESSAGE;
    }
    else if(type == STATUS) {
        char *ubit = ((char *)packed);
        packed += sizeof(char)*NAME_SIZE;
        if(!scanForStatus(packed)) {
            return -1;
        }
        size_t *datalength = ((size_t *)packed);
        packed += sizeof(size_t)*2;
        char *status = ((char *)packed);

        char c;
        size_t i = 0;
        while((c = ubit[i])!='\0'&& i < NAME_SIZE) {
            unpacked[i] = c;
            i++;
        }
        unpacked[i] = ' ';
        unpacked += i+1;
        i = 0;
        while(i < datalength[0]) {
            c = status[i];
            unpacked[i] = c;
            i++;
        }
        unpacked[i] = '\0';
        return STATUS;
    }
    else if(type == LABELED) {
        char *ubit = ((char *)packed);
        packed += sizeof(char)*NAME_SIZE;
        size_t *datalengths = ((size_t *)packed);
        packed += sizeof(size_t)*3;
        char *msg = ((char *)packed);
        packed += sizeof(char)*datalengths[0];
        char *trgt = ((char *)packed);

        char c;
        size_t i = 0;
        while((c = ubit[i])!='\0'&& i < NAME_SIZE) {
            unpacked[i] = c;
            i++;
        }
        unpacked[i] = ':';
        unpacked[i+1] = ' ';
        unpacked[i+2] = '@';
        unpacked += i+3;
        i=0;
        while(i < datalengths[1]) {
            c = trgt[i];
            unpacked[i] = c;
            i++;
        }
        unpacked[i] = ' ';
        unpacked += i+1;
        i = 0;
        while(i < datalengths[0]) {
            c = msg[i];
            unpacked[i] = c;
            i++;
        }
        unpacked[i] = '\0';
        return LABELED;
    }
    else {
        return -1;
    }
    return -1;
}

/* Unpack the given packed packet into the given statistics structure.
 * You can assume that packed points to a packet buffer large enough to
 * hold the statistics packet, but you cannot assume that it is
 * otherwise valid.  You can assume that statistics points to a
 * statistics structure.
 *
 * Returns the packet type that was unpacked, or -1 if it is invalid.
 */
int unpack_statistics(struct statistics *statistics, void *packed) {
    int type = *((int *)packed);
    packed += sizeof(int);
    if(type == STATISTICS) {
        char *ubit = ((char *)packed);
        packed += sizeof(char)*NAME_SIZE;
        char *ldr = ((char *)packed);
        packed += sizeof(char)*NAME_SIZE;
        int *ldrCount = ((int *)packed);
        packed += sizeof(int);
        long *nvldCount = ((long *)packed);
        packed += sizeof(long);
        long *rfrshCount = ((long *)packed);
        packed += sizeof(long);
        int *msgCount = ((int *)packed);

        int i = 0;
        while(i < NAME_SIZE && ubit[i]!='\0') {
            statistics->sender[i] = ubit[i];
            i++;
        }
        statistics->sender[i] = '\0';

        i = 0;
        while(i < NAME_SIZE && ldr[i]!='\0') {
            statistics->most_active[i] = ldr[i];
            i++;
        }

        statistics->most_active_count = *ldrCount;
        statistics->invalid_count = *nvldCount;
        statistics->refresh_count = *rfrshCount;
        statistics->messages_count = *msgCount;

        return STATISTICS;
    }
    else {
        return -1;
    }
    return 0;
}

int scanForMessage(void *ptr) {
    //UBIT garanteed
    //pointer starts afters type and UBIT
    size_t length = *(size_t *)ptr;
    if(length > MAX_MESSAGE_SIZE || length == 0) {
        return 0;
    }
    ptr += sizeof(size_t);
    size_t zero = *(size_t *)ptr;
    size_t z = 0;
    if(zero != z) {
        return 0;
    }
    ptr += sizeof(size_t);
    char *sptr = (char*)ptr;
    for(size_t i = 0; i < length;i++) {
        char c = sptr[i];
        if(c == '\0' || c < 0) {
            return 0;
        }
    }
    return 1;
}

int scanForStatus(void *ptr) {
     //UBIT garanteed
    //pointer starts afters type and UBIT
    size_t length = *(size_t *)ptr;
    if(length > MAX_MESSAGE_SIZE || length == 0) {
        return 0;
    }
    ptr += sizeof(size_t);
    size_t zero = *(size_t *)ptr;
    size_t z = 0;
    if(zero != z) {
        return 0;
    }
    ptr += sizeof(size_t);
    char *sptr = (char*)ptr;
    for(size_t i = 0; i < length;i++) {
        char c = sptr[i];
        if(c == '\0' || c < 0) {
            return 0;
        }
    }
    return 1;
}
