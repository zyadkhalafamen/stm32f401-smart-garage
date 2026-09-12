#ifndef KEYPAD_H_
#define KEYPAD_H_

void keypad_init(void);

/*
 * ترجع الزر المضغوط مرة واحدة فقط.
 * ترجع 0 إذا لم يوجد زر جديد.
 */
char keypad_get_key(void);

#endif /* KEYPAD_H_ */
