/*
    This is source file for creating virtual keyboard
*/

#include "keyboard.h"
#include "status.h"
#include "kernel.h"
#include "task/task.h"
#include "task/process.h"
#include "classic.h"

static struct keyboard* keyboard_list_head = 0;
static struct keyboard* keyboard_list_last = 0;

/*
    initializing the keyboard
*/
void keyboard_init(){
    keyboard_insert(classic_init());
}

/*
    inserting the keyboard
*/
int keyboard_insert(struct keyboard* keyboard){
    int res = 0;
    if(keyboard->init == 0){
        res = -EINVARG;
        goto out;
    }

    if(keyboard_list_last){
        keyboard_list_last->next = keyboard;
        keyboard_list_last = keyboard;
    }
    else{
        keyboard_list_head = keyboard;
        keyboard_list_last = keyboard;
    }

    res = keyboard->init;

out:
    return res;
}

/*
    get the tail index of the keyboard
    as buffer size if 1024, the value of tail always will be 
    from 0 to 1023.

    Initially both head and tail will be pointing to 0th index,
    then a keyboard input is given and will occupy 0th index,
    and now the tail will move to 1st index and head will remain at 0th index.

    Suppose another input buffer provided, so tail at 2nd and head at 0th.

    Now when read from keyboard, tail will be on 2nd index, but head will move to
    1st index, and so on.

    And after 1023, the tail will move to 0th index and this goes on.
*/
static int keyboard_get_tail_index(struct process* process){
    return process->keyboard.tail % sizeof(process->keyboard.buffer);
}

/*
    for having backspace in keyboard buffer
*/
void keyboard_backspace(struct process* process){
    process->keyboard.tail -= 1;
    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = 0x00;
}

/*
    set capsloack for lower and upper letters
*/
void keyboard_set_capslock(struct keyboard* keyboard, KEYBOARD_CAPS_LOCK_STATE state){
    keyboard->capslock_state = state;
}

/*
    gets capslock state
*/
KEYBOARD_CAPS_LOCK_STATE keyboard_get_capslock(struct keyboard* keyboard){
    return keyboard->capslock_state;
}

/*
    push in the cahracter into the for virtual keyboard buffer
*/
void keyboard_push(char c){
    struct process* process = process_current();
    if(!process){
        return;
    }
    
    /* if null, then nu=othing to push */
    if(c == 0){
        return;
    }

    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = c;
    process->keyboard.tail++;
}

/*
    to pop the character from virtual keyboard buffer
*/
char keyboard_pop(){
    if(!task_current()){
        return 0;
    }

    struct process* process = task_current()->process;
    int real_index = process->keyboard.head % sizeof(process->keyboard.buffer);
    char c = process->keyboard.buffer[real_index];
    if(c == 0x00){
        /* nothing to pop, return zero */
        return 0;
    }

    process->keyboard.buffer[real_index] = 0;
    process->keyboard.head++;

    return c;
}