#ifndef BUTTON_H_ 
#define BUTTON_H_

void BTN_set_invert(void);
void BTN_clear_invert(void);
void BTN_enable_pullup(void);
void BTN_disable_pullup(void);
char* BTN_status(void);

#endif