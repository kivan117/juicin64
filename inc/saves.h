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

#endif