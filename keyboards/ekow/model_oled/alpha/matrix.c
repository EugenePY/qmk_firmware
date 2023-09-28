/**
 * matrix.c
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <hal.h>
#include "quantum.h"
#include "timer.h"
#include "wait.h"
#include "print.h"
#include "matrix.h"

/**
 *
 * Row pins are input with internal pull-down.
 * Column pins are output and strobe with high.
 * Key is high or 1 when it turns on.
 *
 */
/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_COLS];
static bool         debouncing      = false;
static uint16_t     debouncing_time = 0;

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;

// user-defined overridable functions

__attribute__((weak)) void matrix_init_kb(void) {
    matrix_init_user();
}

__attribute__((weak)) void matrix_scan_kb(void) {
    matrix_scan_user();
}

__attribute__((weak)) void matrix_init_user(void) {}

__attribute__((weak)) void matrix_scan_user(void) {}

// helper functions
void matrix_init(void) {
    // debug_enable = true;
    for (int i = 0; i < MATRIX_COLS; i++) {
        palSetLineMode(col_pins[i], PAL_MODE_OUTPUT_PUSHPULL);
        palClearLine(col_pins[i]);
    }

    for (int j = 0; j < MATRIX_ROWS; j++) {
        palSetLineMode(row_pins[j], PAL_MODE_INPUT_PULLDOWN);
    }
    memset(matrix, 0, MATRIX_ROWS * sizeof(matrix_row_t));
    memset(matrix_debouncing, 0, MATRIX_COLS * sizeof(matrix_row_t));

    matrix_init_kb();
}

uint8_t matrix_scan(void) {
    for (int col = 0; col < MATRIX_COLS; col++) {
        palSetLine(col_pins[col]);
        // need wait to settle pin state
        wait_us(20);

        matrix_row_t data = 0;
        for (int i = 0; i < MATRIX_ROWS; i++) {
            data |= palReadLine(row_pins[i]) << i;
        }
        palClearLine(col_pins[col]);
        if (matrix_debouncing[col] != data) {
            matrix_debouncing[col] = data;
            debouncing             = true;
            debouncing_time        = timer_read();
        }
    }

    if (debouncing && timer_elapsed(debouncing_time) > DEBOUNCE) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix[row] = 0;
            for (int col = 0; col < MATRIX_COLS; col++) {
                matrix[row] |= ((matrix_debouncing[col] & (1 << row) ? 1 : 0) << col);
            }
        }
        debouncing = false;
    }

    matrix_scan_kb();
    return 1;
}

bool matrix_is_on(uint8_t row, uint8_t col) {
    return (matrix[row] & (1 << col));
}

matrix_row_t matrix_get_row(uint8_t row) {
    return matrix[row];
}

void matrix_print(void) {
    printf("\nr/c 01234567\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        printf("%X0: ", row);
        matrix_row_t data = matrix_get_row(row);
        for (int col = 0; col < MATRIX_COLS; col++) {
            if (data & (1 << col))
                printf("1");
            else
                printf("0");
        }
        printf("\n");
    }
}
