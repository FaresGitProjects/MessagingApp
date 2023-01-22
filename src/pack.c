#include <stdlib.h>
#include <string.h>

#include "serialize.h"

/* Pack the user input provided in input into the appropriate message
 * type in the space provided by packed.  You can assume that input is a
 * NUL-terminated string, and that packed is a buffer of size
 * PACKET_SIZE.
 *
 * Returns the packet type for valid input, or -1 for invalid input.
 */
void *initialize(void *packed, int type);
void *message(void *packed, char *message);

int ensureMessage(char *msg);

int pack(void *packed, char *input) {
//STATISTICS
    int isStat = 0;
    char *ptrStat = strstr(input, "/stats");
    if(ptrStat != NULL && *ptrStat == input[0]) {
        isStat = 1;
    }
    if(isStat) {
        if(input[6] != '\0') {
            return -1;
        }
        packed = initialize(packed, STATISTICS);
        return STATISTICS;
    }






//LABELED
    int isLbl = 0;
    int justSpace = 0;
    char *ptrAt = strstr(input, "@");
    char *msg;
    char *trgt;
    if(ptrAt != NULL && *ptrAt == input[0]) {
        isLbl = 1;
    }
    if(isLbl) {
        trgt = ptrAt+1;
        size_t tlen = strcspn(trgt, " ");
        if(tlen > NAME_SIZE) {
            return -1;
        }
        char c;
        int i = 1+tlen;
        int charFound = 0;
        int msglen = 0;
        while((c = input[i])!='\0') {
            if(c != ' ' && charFound == 0) {
                msg = (input + i);
                charFound = 1;
            }
            if(charFound) {
                msglen += 1;
            }
            i++;
        }
        if(!charFound) {
            return -1;
        }

        packed = initialize(packed, LABELED);

        size_t datalengths[] = {msglen, tlen, 0};
        *((size_t *)packed) = datalengths[0];
        packed += sizeof(size_t);
        *((size_t *)packed) = datalengths[1];
        packed += sizeof(size_t);
        *((size_t *)packed) = datalengths[2];
        packed += sizeof(size_t);

        if(!ensureMessage(msg)) {
            return -1;
        }
        packed = message(packed, msg);

        for(i = 0; (c = trgt[i]) != ' '; i++) {
            *((char *)packed) = c;
            packed += sizeof(char);
        }
        return LABELED;
    }
















//STATUS
    int isSts = 0;
    justSpace = 1;
    char *ptr = strstr(input,"/me ");
    if(ptr != NULL && *ptr == input[0]) {
        isSts = 1;
    }
    if(isSts) {
        char c = 0;
        for(int i = 4;(c = input[i])!='\0';i++) {
            if(c != ' ') {
                msg = (input+i);
                justSpace = 0;
                break;
            }
        }
        if(justSpace) {
            return -1;
        }
        packed = initialize(packed, STATUS);

        size_t datalengths[] = {strlen(msg),0};
        *((size_t *)packed) = datalengths[0];
        packed += sizeof(size_t);
        *((size_t *)packed) = datalengths[1];
        packed += sizeof(size_t);

        if(!ensureMessage(msg)) {
            return -1;
        }
        packed = message(packed, msg);
        return STATUS;
    }
















//MESSAGE
    justSpace = 1;
    char c;
    for(int i = 0; (c = input[i])!='\0'; i++) {
        if(c != ' ') {
            justSpace = 0;
            break;
        }
    }
    if(justSpace) {
        return -1;
    }
    size_t datalengths[] = {strlen(input),0};

    packed = initialize(packed, MESSAGE);

    *((size_t *)packed) = datalengths[0];
    packed += (sizeof(size_t));
    *((size_t *)packed) = datalengths[1];
    packed += (sizeof(size_t));

    if(!ensureMessage(input)) {
        return -1;
    }
    packed = message(packed, input);

    return MESSAGE;
}

/* Create a refresh packet for the given message ID.  You can assume
 * that packed is a buffer of size PACKET_SIZE.
 *
 * You should start by implementing this method!
 *
 * Returns the packet type.
 */





int pack_refresh(void *packed, int message_id) {
    packed = initialize(packed, REFRESH);
    *((int *)packed) = message_id;;
    return REFRESH;
}

void *initialize(void *packed, int type) {
    *((int *)packed) = type;
    packed += sizeof(int);
    int i = 0;
    char c;
    char ubit[NAME_SIZE] = "faresmoh";
    while((c = ubit[i]) != '\0') {
        *((char *)packed) = c;
        packed += sizeof(char);
        i++;
    }
    int rem = NAME_SIZE - i;
    i = 0;
    while(i < rem) {
        *((char *)packed) = '\0'; //ASCII NULL
        packed += sizeof(char);
        i++;
    }
    return packed;
}

void *message(void *packed, char *message) {
    size_t len = strlen(message);
    for(size_t i = 0; i < len; i++){
        *((char *)packed) = message[i];
        packed += sizeof(char);
    }
    return packed;
}

int ensureMessage(char *msg) {
    strcat(msg,"\0");
    if(strlen(msg)>MAX_MESSAGE_SIZE) {
        return 0;
    }
    return 1;
}
