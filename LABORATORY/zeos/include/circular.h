#ifndef CIRCULAR_HH
#define CIRCULAR_HH 

#define MAX_BUFF_SIZE 32

typedef struct{
    char keys[MAX_BUFF_SIZE];
    int write_idx;
    int read_idx;
    int size;
} circular_buffer;

//Constructor
void init(circular_buffer* crc_bff){
    crc_bff->write_idx = 0;
    crc_bff->read_idx = 0;
    crc_bff->size = 0;
}

int is_full(circular_buffer* crc_bff){
    return ((crc_bff->write_idx+1) % MAX_BUFF_SIZE) == crc_bff->read_idx;
}

int is_empty(circular_buffer* crc_bff){
    return crc_bff->read_idx == crc_bff->write_idx;
}

int size(circular_buffer* crc_bff){
    return crc_bff->size;
}

//Bolean function, returns 0 if it wasn't possible
int put (circular_buffer* crc_bff, int item)
{
    //Buffer is full and blocked
    if(is_full(crc_bff)){
        return 0;
    }
    else{
        crc_bff->keys[crc_bff->write_idx] = item;
        crc_bff->write_idx = (crc_bff->write_idx+1) % MAX_BUFF_SIZE;
        return 1;
    }
}

int get(circular_buffer* crc_bff, char * ret_value){
    // Buffer is empty
    if(is_empty(crc_bff)){
        return 0;
    }
    else{
        *ret_value = crc_bff->keys[crc_bff->read_idx];
        crc_bff->read_idx = (crc_bff->read_idx+1) % MAX_BUFF_SIZE;
        return 1;
    }
}

#endif