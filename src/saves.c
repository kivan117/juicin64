#include <libdragon.h>
#include "string.h"
#include "saves.h"

int check_eeprom(void)
{
    //get the saved header with checksum
    uint8_t stored_sum[8];
    eeprom_read(0, stored_sum);

    uint8_t temp_buffer[8];
    uint8_t sum = 0;
    //sum all other bytes in eeprom, invert the result
    for(int i = 1; i<64; i++) //eeprom is 512 bytes long, read in 8-byte blocks
    {
        eeprom_read(i, temp_buffer);
        for(int k = 0; k<8; k++)
        {
            sum += temp_buffer[k];
        }
    }
    sum = ~sum;
    //compare the stored value to the computed sum
    if(stored_sum[0] != sum)
    {
        return 0;
    }
    return 1;
}

void reset_eeprom(void)
{
    uint8_t stored_sum[8];
    //write a whole new eeprom
    stored_sum[0] = 0xFF;
    for(int i = 1; i<8; i++)
    {
        stored_sum[i] = 0x00;
    }
    eeprom_write(0, stored_sum); //write the expected checksum to the header
    stored_sum[0] = 0x00; //make sure all values in our 8 byte buffer are 0
    //write all 0's to the rest of eeprom
    for(int i = 1; i < 64; i++)
    {
        eeprom_write(i, stored_sum);
    }
}

void read_scores(uint32_t* scores)
{
    //eeprom is read in 8 byte blocks
    //block 0 is the eeprom header
    //blocks 1-5 correspond to the 10 top scores, 4 bytes each
    //our high scores are 32bit unsigned ints, so we'll read in
    //one block at a time, which is 2 scores at a time,
    //and copy them into the "scores" argument array

    uint8_t score_buffer[8];
    eeprom_read(1, score_buffer);
    memcpy(&scores[0], &score_buffer[0], 4);
    memcpy(&scores[1], &score_buffer[4], 4);
    eeprom_read(2, score_buffer);
    memcpy(&scores[2], &score_buffer[0], 4);
    memcpy(&scores[3], &score_buffer[4], 4);
    eeprom_read(3, score_buffer);
    memcpy(&scores[4], &score_buffer[0], 4);
    memcpy(&scores[5], &score_buffer[4], 4);
    eeprom_read(4, score_buffer);
    memcpy(&scores[6], &score_buffer[0], 4);
    memcpy(&scores[7], &score_buffer[4], 4);
    eeprom_read(5, score_buffer);
    memcpy(&scores[8], &score_buffer[0], 4);
    memcpy(&scores[9], &score_buffer[4], 4);

}

void write_scores(uint32_t* scores)
{
    //this takes in at least 10 32bit scores
    //and writes them to eeprom, then
    //calculates the new checksum
    //and writes that to the header

    //no actual checking of the score order occurs,
    //they are written in whatever order they are
    //given to the function.

    uint8_t score_buffer[8];
    memcpy(&score_buffer[0], &scores[0], 4);
    memcpy(&score_buffer[4], &scores[1], 4);
    eeprom_write(1, score_buffer);
    memcpy(&score_buffer[0], &scores[2], 4);
    memcpy(&score_buffer[4], &scores[3], 4);
    eeprom_write(2, score_buffer);
    memcpy(&score_buffer[0], &scores[4], 4);
    memcpy(&score_buffer[4], &scores[5], 4);
    eeprom_write(3, score_buffer);
    memcpy(&score_buffer[0], &scores[6], 4);
    memcpy(&score_buffer[4], &scores[7], 4);
    eeprom_write(4, score_buffer);
    memcpy(&score_buffer[0], &scores[8], 4);
    memcpy(&score_buffer[4], &scores[9], 4);
    eeprom_write(5, score_buffer);

    uint8_t temp_buffer[8];
    uint8_t sum = 0;
    //sum all other bytes in eeprom, invert the result
    for(int i = 1; i<64; i++) //eeprom is 512 bytes long, read in 8-byte blocks
    {
        eeprom_read(i, temp_buffer);
        for(int k = 0; k<8; k++)
        {
            sum += temp_buffer[k];
        }
    }
    sum = ~sum;

    eeprom_read(0, temp_buffer);
    temp_buffer[0] = sum;
    eeprom_write(0, temp_buffer);
}

void sort_scores(uint32_t* scores)
{
    //basic bubble sort because this should honestly never get called
    int n = 10;
    int i, j, temp;
    bool swapped;
    for (i = 0; i < n-1; i++)
    {
        swapped = false;
        for (j = 0; j < n-i-1; j++)
        {
            if (scores[j] < scores[j+1])
            {
                temp = scores[j];
                scores[j] = scores[j+1];
                scores[j+1] = temp;
                swapped = true;
            }
        }
 
        if (swapped == false) 
        break; 
    }
}

uint8_t insert_score(uint32_t* scores, uint32_t newscore)
{
    //insert new score, return position
    int n = 10;
    int i = 0;
    uint8_t insert_pos = 10;
    for(i = n-1; i >= 0; i--)
    {
        if(newscore < scores[i])
        {
            break;
        }
        else
        {
            insert_pos = i;
        }
    }

    if(insert_pos < n)
    {
        for(i = n-1; i > insert_pos; i--)
        {
            scores[i] = scores[i-1];
        }
        scores[insert_pos] = newscore;
    }


    return insert_pos;
}