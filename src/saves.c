#include <libdragon.h>
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

}

void write_scores(uint32_t* scores)
{

}