/*
    This is header which take care of interrupt 80 when generated 
*/
#ifndef ISR80H_H
#define ISR80H_H

enum SystemCommands{
    SYSTEM_COMMAND0_SUM
};

void isr80h_register_commands();

#endif ISR80H_H