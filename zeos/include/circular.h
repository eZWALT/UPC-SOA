#ifndef CIRCULAR_HH
#define CIRCULAR_HH 

#define MAX_BUFF_SIZE 32

/**
 * @struct circular_buffer
 * @brief Structure representing a circular buffer.
 */
typedef struct {
    char keys[MAX_BUFF_SIZE];
    int write_idx;
    int read_idx;
    int size;
} circular_buffer;

/**
 * @brief Initializes the circular buffer.
 * 
 * @param crc_bff Pointer to the circular buffer.
 */
void init(circular_buffer* crc_bff);

/**
 * @brief Checks if the circular buffer is full.
 * 
 * @param crc_bff Pointer to the circular buffer.
 * @return 1 if the buffer is full, otherwise 0.
 */
int is_full(circular_buffer* crc_bff);

/**
 * @brief Checks if the circular buffer is empty.
 * 
 * @param crc_bff Pointer to the circular buffer.
 * @return 1 if the buffer is empty, otherwise 0.
 */
int is_empty(circular_buffer* crc_bff);

/**
 * @brief Returns the current size of the circular buffer.
 * 
 * @param crc_bff Pointer to the circular buffer.
 * @return Current size of the buffer.
 */
int size(circular_buffer* crc_bff);

/**
 * @brief Adds an item to the circular buffer.
 * 
 * @param crc_bff Pointer to the circular buffer.
 * @param item Item to be added to the buffer.
 * @return 1 if the item was successfully added, otherwise 0.
 */
int put(circular_buffer* crc_bff, char item);

/**
 * @brief Retrieves an item from the circular buffer.
 * 
 * @param crc_bff Pointer to the circular buffer.
 * @param ret_value Pointer to store the retrieved item.
 * @return 1 if an item was successfully retrieved, otherwise 0.
 */
int get(circular_buffer* crc_bff, char *ret_value);

#endif