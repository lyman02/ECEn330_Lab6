#ifndef BTN_H_
#define BTN_H_

// GPIO pins associated with buttons
#define BTN_A      32
#define BTN_B      33
#define BTN_MENU   13
#define BTN_OPTION  0
#define BTN_SELECT 27
#define BTN_START  39
#define BTN_MASK ( \
	1LLU << BTN_A | \
	1LLU << BTN_B | \
	1LLU << BTN_MENU | \
	1LLU << BTN_OPTION | \
	1LLU << BTN_SELECT | \
	1LLU << BTN_START \
	)

#endif // BTN_H_
