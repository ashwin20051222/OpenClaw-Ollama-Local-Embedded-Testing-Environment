/**
 * motor_test.c
 * 
 * INTENTIONALLY BUGGY CODE — for testing the c-analyzer skill.
 * 
 * This file simulates updating a PWM duty cycle for a motor on a
 * microcontroller. It contains the following deliberate bugs:
 * 
 *   Bug 1: Missing semicolon on the assignment to *pwm_register
 *   Bug 2: Uninitialized pointer (pwm_register doesn't point to
 *           a real hardware address — will cause a Hard Fault)
 *   Bug 3: Unused parameter warning (motor_id is printed but the
 *           format specifier doesn't match the type on all platforms)
 * 
 * Usage:
 *   1. Copy this file into your OpenClaw workspace
 *   2. In the OpenClaw chat, type:
 *      "Run the c-analyzer skill on motor_test.c and tell me
 *       what the GCC compiler finds."
 *   3. The AI should catch all 3 issues above
 */

#include <stdio.h>
#include <stdint.h>

void update_pwm_duty(uint8_t motor_id, uint32_t duty_cycle) {
    uint32_t max_duty = 4096;
    uint32_t *pwm_register;  // ⚠️ BUG: Uninitialized pointer!

    if (duty_cycle > max_duty) {
        duty_cycle = max_duty;
    }

    // ⚠️ BUG: Missing semicolon on the next line!
    // ⚠️ BUG: Writing to a random/null memory address!
    *pwm_register = duty_cycle

    printf("Motor %d PWM updated to %d\n", motor_id, duty_cycle);
}

int main() {
    uint8_t current_motor = 1;

    printf("Starting motor controller...\n");
    update_pwm_duty(current_motor, 2048);

    return 0;
}
