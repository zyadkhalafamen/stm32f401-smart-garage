#ifndef ACCESS_CONTROL_H_
#define ACCESS_CONTROL_H_

typedef enum
{
    GATE_CLOSED = 0,
    GATE_OPENING,
    GATE_OPEN,
    GATE_CLOSING,
    GATE_EMERGENCY
} GateState;

void access_control_init(void);
void access_control_task(void);
void access_control_handle_key(char key);

GateState access_control_get_gate_state(void);

#endif /* ACCESS_CONTROL_H_ */
