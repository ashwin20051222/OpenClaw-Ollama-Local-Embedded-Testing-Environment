/**
 * state_machine_test.c
 *
 * INTENTIONALLY BUGGY CODE — for testing deeper AI logic analysis.
 *
 * This file implements a simple robotic claw state machine with
 * the following deliberate bugs that GCC CANNOT catch:
 *
 *   Bug 1: Missing default case in switch (undefined behavior if
 *           state variable gets corrupted)
 *   Bug 2: No timeout handling — SEARCH state can loop forever
 *           if no object is detected (watchdog will reset the MCU)
 *   Bug 3: STATE_VERIFY transitions to STATE_DROP without checking
 *           if the grab actually succeeded (object may fall)
 *   Bug 4: Missing volatile qualifier on shared ISR variable
 *   Bug 5: Blocking delay inside the main loop with no break
 *           condition (sensor_read could block indefinitely)
 *   Bug 6: No state entry/exit actions — motor may stay energized
 *           during state transitions
 *
 * These are LOGIC bugs, not syntax errors. The code compiles cleanly.
 * A good AI tester should catch these through reasoning alone.
 *
 * Usage:
 *   1. Paste this code into the OpenClaw chat
 *   2. Type: "Review this state machine for logic bugs.
 *            What will happen on real hardware?"
 *   3. The AI should identify at least bugs 1-4 above
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* ═══════════════════════════════════════════════════════ */
/*  State Definitions                                      */
/* ═══════════════════════════════════════════════════════ */

typedef enum {
  STATE_IDLE,
  STATE_SEARCH,
  STATE_ALIGN,
  STATE_GRAB,
  STATE_VERIFY,
  STATE_DROP
} RobotState;

/* ═══════════════════════════════════════════════════════ */
/*  Global Variables                                        */
/* ═══════════════════════════════════════════════════════ */

RobotState current_state = STATE_IDLE;

// ⚠️ BUG 4: Missing 'volatile' — this is modified by an ISR
//           The compiler may optimize away reads from this variable
bool object_detected = false;

uint32_t search_start_time = 0;

/* ═══════════════════════════════════════════════════════ */
/*  Mock Hardware Functions                                 */
/* ═══════════════════════════════════════════════════════ */

// Simulates reading from an ultrasonic distance sensor
uint32_t sensor_read_distance(void) {
  // In real firmware, this would read from an ADC or trigger
  // an ultrasonic pulse and measure echo time
  return 150; // mm
}

// Simulates checking if the claw gripper has pressure
bool gripper_has_pressure(void) {
  return true; // Always returns true — another potential issue
}

// Simulates motor control
void motor_set_speed(int8_t speed) {
  printf("Motor speed set to: %d\n", speed);
}

void gripper_close(void) { printf("Gripper closing...\n"); }

void gripper_open(void) { printf("Gripper opening...\n"); }

uint32_t get_system_time_ms(void) {
  static uint32_t t = 0;
  return t++;
}

/* ═══════════════════════════════════════════════════════ */
/*  ISR — External interrupt for object detection sensor   */
/* ═══════════════════════════════════════════════════════ */

void EXTI0_IRQHandler(void) {
  // This runs in interrupt context
  object_detected = true;
  // ⚠️ No clearing of the interrupt flag!
  // On real hardware, this ISR will fire continuously
}

/* ═══════════════════════════════════════════════════════ */
/*  State Machine — Main Logic                              */
/* ═══════════════════════════════════════════════════════ */

void run_state_machine(void) {
  // ⚠️ BUG 1: No default case in switch
  //    If current_state gets corrupted (e.g., stack overflow,
  //    cosmic ray bit flip), behavior is completely undefined
  switch (current_state) {
  case STATE_IDLE:
    motor_set_speed(0);
    if (object_detected) {
      current_state = STATE_SEARCH;
      search_start_time = get_system_time_ms();
      object_detected = false;
    }
    break;

  case STATE_SEARCH:
    // ⚠️ BUG 2: No timeout! If the object is never found,
    //    this state runs forever. On real hardware, the
    //    watchdog timer will reset the MCU.
    motor_set_speed(50);

    uint32_t distance = sensor_read_distance();
    if (distance < 100) { // Object within 100mm
      current_state = STATE_ALIGN;
    }
    // No else — no timeout check — stuck forever
    break;

  case STATE_ALIGN:
    motor_set_speed(20); // Slow approach

    distance = sensor_read_distance();
    if (distance < 30) { // Close enough to grab
      current_state = STATE_GRAB;
    }
    break;

  case STATE_GRAB:
    motor_set_speed(0); // Stop moving
    gripper_close();

    // Immediately transition — no delay to let gripper close!
    current_state = STATE_VERIFY;
    break;

  case STATE_VERIFY:
    // ⚠️ BUG 3: Always transitions to DROP without checking
    //    if the grab actually succeeded. gripper_has_pressure()
    //    always returns true (mock), but even if it returned false,
    //    this code doesn't handle the failure case.
    if (gripper_has_pressure()) {
      current_state = STATE_DROP;
    }
    // What if pressure check fails? No retry, no error state!
    break;

  case STATE_DROP:
    gripper_open();
    // ⚠️ BUG 6: No motor stop command here!
    //    Motor might still be running from SEARCH or ALIGN state
    current_state = STATE_IDLE;
    break;

    // ⚠️ BUG 1: Missing default case
    // default:
    //     current_state = STATE_IDLE;  // Safe recovery
    //     motor_set_speed(0);
    //     gripper_open();
    //     break;
  }
}

/* ═══════════════════════════════════════════════════════ */
/*  Main Loop                                               */
/* ═══════════════════════════════════════════════════════ */

int main(void) {
  printf("Robot Claw Controller Starting...\n");
  printf("State Machine Initialized: IDLE\n");

  // ⚠️ BUG 5: Infinite loop with no exit condition
  //    On a desktop this is fine, but on an MCU you need
  //    to feed the watchdog timer inside this loop
  while (1) {
    run_state_machine();
  }

  // This line is unreachable
  return 0;
}
