/*
    more OS functions
*/

#include "peachos.h"
#include "string.h"

/*
    breaking the terminal line into tokens
*/
struct command_argument* peachos_parse_command(const char* command, int max){
    /* creating root token */
    struct command_argument* root_command = 0;
    /* second word */
    char scommand[1025];
    if(max >= (int)sizeof(scommand)){
        return 0;
    }

    /* copying the second word and creating token */
    strncpy(scommand, command, sizeof(scommand));
    char* token = strtok(scommand, " ");
    if(!token){
        goto out;
    }

    /* allocating memory for root token */
    root_command = peachos_malloc(sizeof(struct command_argument));
    if(!root_command){
        goto out;
    }
    
    /*  assigning memory with values */
    strncpy(root_command->argument, token, sizeof(root_command->argument));
    root_command->next = 0;

    /* forming chain like linked list of tokens */
    struct command_argument* current = root_command;
    token = strtok(NULL, " ");
    while(token != 0){
        struct command_argument* new_command = peachos_malloc(sizeof(struct command_argument));
        if(!new_command){
            break;
        }

        strncpy(new_command->argument, token, sizeof(new_command->argument));
        new_command->next = 0x00;
        current->next = new_command;
        current = new_command;
        token = strtok(NULL, " ");
    }

out:
    return root_command;
};

/*
    get the key and then block
*/
int peachos_getkeyblock(){
    int val = 0;
    do{
        val = peachos_getkey();
    }
    while(val == 0);
    return val;
}

/*
    this will print only when you press enter like filling password
*/
void peachos_terminal_readline(char* out, int max, bool output_while_typing){
    int i = 0;
    for(i = 0; i < max-1; i++){
        char key = peachos_getkeyblock();

        /* carriage return means we have read the line */
        if(key == 13){
            break;
        }

        if(output_while_typing){
            peachos_putchar(key);
        }

        /* backspace */
        if(key == 0x08 && i >= 1){
            out[i-1] = 0x00;
            /* -2 because we will +1 when we go to the continue */
            i -= 2;
            continue;
        }
        out[i] = key;
    }

    /* add the null terminator at the end */
    out[i] = 0x00;
}

/*
    runs the command or process from terminal
*/
int peachos_system_run(const char* command){
    char buf[1024];
    strncpy(buf, command, sizeof(buf));
    struct command_argument* root_command_argument = peachos_parse_command(buf, sizeof(buf));
    if(!root_command_argument){
        return -1;
    }

    return peachos_system(root_command_argument);
}