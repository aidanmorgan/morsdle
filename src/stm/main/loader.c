#include <string.h>
#include "morsdle_main.h"
#include "stm_flash_write.h"

#define MAX_COMMAND_LENGTH 255




// commands:
// FONT <letter:1> <data_size:uint32_t> then data
// WORD <letter-count:1> <5-letter string>
// chosen so any read from the UART can just read the first 6 characters and then know what to do making
// building the system a little easier
void loader_main(flash_cfg_t* flashcfg, console_t* console) {
    flash_write_session_t session = (flash_write_session_t) {};
    flash_init_session(flashcfg, &session);

    while(1) {
        char command_buffer[MAX_COMMAND_LENGTH + 1];
        scanf("%255s", command_buffer);

        if(strcmp("FONT ", command_buffer) == 0) {

        }
        else if(strcmp("WORD ", command_buffer) == 0) {

        }
        else if(strcmp("EXIT ", command_buffer) == 0)
        {
            flash_flush(&session);
        }
    }
}