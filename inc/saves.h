#ifndef __SAVES_H__
#define __SAVES_H__

//calculates eeprom checksum and compares it against the
//checksum saved in the eeprom header.
//return non-zero for a passing check, 0 for a failing check
int check_eeprom(void);

//resets eeprom with all 0's and a matching header
void reset_eeprom(void);

//takes a pointer to a uint32_t array, capacity at least 10.
//reads top 10 scores from eeprom into the first 10 entries
//in the 'scores' array
void read_scores(uint32_t* scores);

//takes a pointer to a uint32_t array, capacity at least 10.
//writes the first 10 entries in the 'scores' array to the
//high score list in eeprom in the order given.
//no sorting is done by this function, only blind copy
void write_scores(uint32_t* scores);

//takes an unsorted list of 10 scores
//sorts the 10 scores, greatest to least
//stores the result in the input argument
//no value is returned
void sort_scores(uint32_t* scores);

//takes 10 unsigned 32bit scores, sorted greatest to least and one new score
//inserts the new score into the sorted list
//if the new score is less than all others, it is discarded
//the newly sorted top 10 scores are stored in the first argument
//returns the position, 0 to 10, of the new score in the sorted list
uint8_t insert_score(uint32_t* scores, uint32_t newscore);

#endif