#include "access_control.h"
#include "app_config.h"
#include "gpio_driver.h"
#include "lcd.h"
#include "servo.h"
#include "timebase.h"

#include <stdint.h>

typedef enum
{
    UI_PASSWORD = 0,
    UI_MESSAGE,
    UI_STATUS,
    UI_LOCKOUT,
    UI_EMERGENCY,
    UI_GATE
} UiState;

static const char correct_password[PASSWORD_LENGTH] =
{
    '1', '2', '3', '4'
};

static GateState gate_state = GATE_CLOSED;
static UiState ui_state = UI_PASSWORD;

static uint32_t gate_state_started_ms = 0U;
static uint32_t ui_started_ms = 0U;
static uint32_t ui_duration_ms = 0U;
static uint32_t lockout_started_ms = 0U;

static uint32_t car_count = 0U;
static uint32_t failed_attempts = 0U;

static char entered_password[PASSWORD_LENGTH] = {0};
static uint32_t digit_count = 0U;

static void password_reset(void)
{
    uint32_t index;

    digit_count = 0U;

    for (index = 0U; index < PASSWORD_LENGTH; index++)
    {
        entered_password[index] = 0;
    }
}

static uint32_t password_is_correct(void)
{
    uint32_t index;

    if (digit_count != PASSWORD_LENGTH)
    {
        return 0U;
    }

    for (index = 0U; index < PASSWORD_LENGTH; index++)
    {
        if (entered_password[index] != correct_password[index])
        {
            return 0U;
        }
    }

    return 1U;
}

static void show_password_screen(void)
{
    uint32_t index;

    ui_state = UI_PASSWORD;

    lcd_clear();

    lcd_set_cursor(1U, 1U);
    lcd_print("ENTER PASSWORD:");

    lcd_set_cursor(2U, 1U);
    lcd_print("> ");

    for (index = 0U; index < digit_count; index++)
    {
        lcd_send_character('*');
    }
}

static void show_timed_message(
    const char *line_one,
    const char *line_two,
    uint32_t duration)
{
    ui_state = UI_MESSAGE;
    ui_started_ms = millis();
    ui_duration_ms = duration;

    lcd_clear();

    lcd_set_cursor(1U, 1U);
    lcd_print(line_one);

    lcd_set_cursor(2U, 1U);
    lcd_print(line_two);
}

static void show_status_screen(void)
{
    ui_state = UI_STATUS;
    ui_started_ms = millis();
    ui_duration_ms = MESSAGE_TIME_MS;

    lcd_clear();

    lcd_set_cursor(1U, 1U);
    lcd_print("CARS: ");
    lcd_print_number(car_count);

    lcd_set_cursor(2U, 1U);

    if (car_count >= GARAGE_CAPACITY)
    {
        lcd_print("GARAGE FULL");
    }
    else
    {
        lcd_print("AVAILABLE: ");
        lcd_print_number(GARAGE_CAPACITY - car_count);
    }
}

static void show_gate_screen(void)
{
    ui_state = UI_GATE;

    lcd_clear();

    lcd_set_cursor(1U, 1U);
    lcd_print("ACCESS GRANTED");

    lcd_set_cursor(2U, 1U);

    if (gate_state == GATE_OPENING)
    {
        lcd_print("GATE OPENING");
    }
    else if (gate_state == GATE_OPEN)
    {
        lcd_print("GATE OPEN");
    }
    else if (gate_state == GATE_CLOSING)
    {
        lcd_print("GATE CLOSING");
    }
}

static void gate_request_open(void)
{
    if (gate_state == GATE_EMERGENCY)
    {
        return;
    }

    if (car_count >= GARAGE_CAPACITY)
    {
        return;
    }

    servo_open();

    gate_state = GATE_OPENING;
    gate_state_started_ms = millis();

    green_led_set(1U);
    red_led_set(0U);

    show_gate_screen();
}

static void gate_request_close(void)
{
    if (gate_state == GATE_EMERGENCY)
    {
        return;
    }

    servo_close();

    gate_state = GATE_CLOSING;
    gate_state_started_ms = millis();

    show_gate_screen();
}

static void gate_state_machine_task(void)
{
    if ((gate_state == GATE_OPENING) &&
        time_elapsed(
            gate_state_started_ms,
            GATE_TRAVEL_TIME_MS))
    {
        gate_state = GATE_OPEN;
        gate_state_started_ms = millis();

        show_gate_screen();
    }
    else if ((gate_state == GATE_OPEN) &&
             time_elapsed(
                 gate_state_started_ms,
                 GATE_OPEN_TIME_MS))
    {
        gate_request_close();
    }
    else if ((gate_state == GATE_CLOSING) &&
             time_elapsed(
                 gate_state_started_ms,
                 GATE_TRAVEL_TIME_MS))
    {
        gate_state = GATE_CLOSED;

        green_led_set(0U);
        red_led_set(1U);

        password_reset();
        show_password_screen();
    }
}

static void emergency_toggle(void)
{
    if (gate_state != GATE_EMERGENCY)
    {
        /*
         * Fail-safe behaviour:
         * Keep the passage open during emergency.
         */
        gate_state = GATE_EMERGENCY;
        servo_open();

        green_led_set(0U);
        red_led_set(1U);

        ui_state = UI_EMERGENCY;

        lcd_clear();

        lcd_set_cursor(1U, 1U);
        lcd_print("EMERGENCY MODE");

        lcd_set_cursor(2U, 1U);
        lcd_print("D: RESET SYSTEM");
    }
    else
    {
        servo_close();

        gate_state = GATE_CLOSING;
        gate_state_started_ms = millis();

        show_gate_screen();
    }
}

static void submit_password(void)
{
    if (car_count >= GARAGE_CAPACITY)
    {
        password_reset();

        show_timed_message(
            "GARAGE FULL",
            "ENTRY DENIED",
            2500U);

        return;
    }

    if (password_is_correct() != 0U)
    {
        failed_attempts = 0U;
        password_reset();

        gate_request_open();
    }
    else
    {
        failed_attempts++;
        password_reset();

        if (failed_attempts >= MAX_FAILED_ATTEMPTS)
        {
            ui_state = UI_LOCKOUT;
            lockout_started_ms = millis();

            red_led_set(1U);
            green_led_set(0U);

            lcd_clear();

            lcd_set_cursor(1U, 1U);
            lcd_print("SYSTEM LOCKED");

            lcd_set_cursor(2U, 1U);
            lcd_print("WAIT 30 SECONDS");
        }
        else
        {
            show_timed_message(
                "ACCESS DENIED",
                "WRONG PASSWORD",
                MESSAGE_TIME_MS);
        }
    }
}

static void register_vehicle_entry(void)
{
    if (car_count < GARAGE_CAPACITY)
    {
        car_count++;

        show_timed_message(
            "CAR ENTERED",
            "COUNT UPDATED",
            MESSAGE_TIME_MS);

        /*
         * A represents the vehicle passing the
         * entry sensor while the gate is open.
         */
        if (gate_state == GATE_OPEN)
        {
            gate_request_close();
        }
    }
    else
    {
        show_timed_message(
            "GARAGE FULL",
            "ENTRY DENIED",
            MESSAGE_TIME_MS);
    }

    password_reset();
}

static void register_vehicle_exit(void)
{
    if (car_count > 0U)
    {
        car_count--;

        show_timed_message(
            "CAR EXITED",
            "COUNT UPDATED",
            MESSAGE_TIME_MS);
    }
    else
    {
        show_timed_message(
            "GARAGE EMPTY",
            "NO CARS INSIDE",
            MESSAGE_TIME_MS);
    }

    password_reset();
}

void access_control_handle_key(char key)
{
    /*
     * Emergency key remains active during every state,
     * including system lockout.
     */
    if (key == 'D')
    {
        emergency_toggle();
        return;
    }

    if ((ui_state == UI_LOCKOUT) ||
        (gate_state == GATE_EMERGENCY))
    {
        return;
    }

    if (key == 'A')
    {
        register_vehicle_entry();
    }
    else if (key == 'B')
    {
        register_vehicle_exit();
    }
    else if (key == 'C')
    {
        password_reset();
        show_status_screen();
    }
    else if ((key >= '0') &&
             (key <= '9') &&
             (gate_state == GATE_CLOSED))
    {
        if (ui_state != UI_PASSWORD)
        {
            show_password_screen();
        }

        if (digit_count < PASSWORD_LENGTH)
        {
            entered_password[digit_count] = key;
            digit_count++;

            lcd_send_character('*');
        }
    }
    else if ((key == '*') &&
             (digit_count > 0U) &&
             (ui_state == UI_PASSWORD))
    {
        digit_count--;
        entered_password[digit_count] = 0;

        lcd_set_cursor(
            2U,
            (uint8_t)(digit_count + 3U));

        lcd_send_character(' ');

        lcd_set_cursor(
            2U,
            (uint8_t)(digit_count + 3U));
    }
    else if ((key == '#') &&
             (gate_state == GATE_CLOSED))
    {
        submit_password();
    }
}

static void user_interface_task(void)
{
    if ((ui_state == UI_LOCKOUT) &&
        time_elapsed(
            lockout_started_ms,
            LOCKOUT_TIME_MS))
    {
        failed_attempts = 0U;
        password_reset();

        show_password_screen();
    }
    else
    {
        if (((ui_state == UI_MESSAGE) ||
             (ui_state == UI_STATUS)) &&
            time_elapsed(
                ui_started_ms,
                ui_duration_ms))
        {
            show_password_screen();
        }
    }
}

void access_control_init(void)
{
    gate_state = GATE_CLOSED;
    ui_state = UI_PASSWORD;

    car_count = 0U;
    failed_attempts = 0U;

    password_reset();

    servo_close();

    green_led_set(0U);
    red_led_set(1U);

    lcd_clear();

    lcd_set_cursor(1U, 3U);
    lcd_print("SMART GARAGE");

    lcd_set_cursor(2U, 2U);
    lcd_print("FIRMWARE V2");

    short_delay_ms(1800U);

    show_password_screen();
}

void access_control_task(void)
{
    gate_state_machine_task();
    user_interface_task();

    /*
     * Optional IR, PIR and limit-switch tasks
     * can be added here later.
     */
}

GateState access_control_get_gate_state(void)
{
    return gate_state;
}
