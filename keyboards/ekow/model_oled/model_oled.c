/* Default function definations */
/* Copyright 2021 QMK
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "keyboard.h"
#include "model_oled.h"
#include "qp.h"
#include "color.h"
#include "qp_comms.h"
#include "matrix.h"

#include "oled_config.h"
#include "ssd1331.h"
#include "wait.h"

#include "graphic.h"
#include "img/gb.qff.h"
#include "img/OLED-test.qgf.h"
#include "img/firmware_graphic.qgf.h"

// for flashing the image
#include "oled_main.h"

enum via_oled_config_value { id_oled_timeout = 1 };

// eeprom config
static oled_config_t oled_config;

void oled_read_config(void) {
    oled_config.raw = eeconfig_read_user();
}

void eeconfig_init_user(void) { // EEPROM is getting reset!
    oled_config.raw            = 0x0000;
    oled_config.timeout_enable = true;     // We want this enabled by default
    oled_config.n_image        = 0;        // order of default image(auther info)
    eeconfig_update_user(oled_config.raw); // Write default value to EEPROM now
}
#define TIMEOUT_TRESHOLD 10
#define OLED_SLEEP_THREHOLD 20

static void via_oled_config_set_value(const uint8_t *data) {
    const uint8_t *value_id   = &(data[0]);
    const uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_oled_timeout: {
            oled_config.timeout_enable = (bool)value_data[0];
            break;
        }
    }
}

static void via_oled_config_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_oled_timeout: {
            value_data[0] = oled_config.timeout_enable;
            break;
        }
    }
}

static void via_oled_config_save_value(void) {
    eeconfig_update_user(oled_config.raw);
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id == id_custom_channel) {
        switch (*command_id) {
            case id_custom_set_value: {
                via_oled_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                via_oled_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                via_oled_config_save_value();
                break;
            }
            default: {
                // Unhandled message.
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }

    // Return the unhandled state
    *command_id = id_unhandled;
}

/* model oled main */

static painter_device_t       oled_switch;
static painter_font_handle_t  font         = NULL;
static graphic_node_t         root         = {.value = NULL, .next = NULL, .last = NULL};
static painter_image_handle_t graphic      = NULL;
static graphic_node_t        *current_node = NULL;

static void oled_setup(void) {
    oled_init(&oled_switch);
    oled_switch = qp_ssd1331_make_spi_device(OLED_WIDTH, OLED_HEIGHT, OLED_SS_PIN, OLED_SSD_1331_DC_PIN, OLED_REST_PIN, OLED_SPI_CLK_DIVISOR, 0);
    qp_init(oled_switch, QP_ROTATION_0);
    //  turn on the OLED
    oled_on();
    // uint8_t args[] = {};
    qp_rect(oled_switch, 0, 0, OLED_WIDTH - 1, OLED_HEIGHT - 1, HSV_BLACK, true);

    oled_read_config();
}

void keyboard_post_init_kb(void) {
    oled_setup();
    // loading the graphics
    font       = qp_load_font_mem(font_gb);
    graphic    = qp_load_image_mem(gfx_firmware_graphic);
    root.value = &graphic;
    root.next  = NULL;
    root.last  = NULL;
    // add(&root, &img);
    create_nodes(&root, (void *)gfx_OLED);
    current_node = &root;
    for (uint8_t i = 0; i < oled_config.n_image; i++) {
        if (!forward(&current_node)) {
            oled_config.n_image = 0; // fall back the default image
            via_oled_config_save_value();
        };
    }
    render_graphic(*current_node->value);
    keyboard_post_init_user();
}

__attribute__((weak)) bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    return true;
}
static deferred_token token;

void render_graphic(painter_image_handle_t graphic) {
    uint8_t x = (MIN(OLED_WIDTH - graphic->width, 0)) / 2;
    uint8_t y = (MIN(OLED_HEIGHT - graphic->height, 0)) / 2;
    token     = qp_animate(oled_switch, x, y, graphic);
}

void graphic_forward_kb(void) {
    if (forward(&current_node)) {
        qp_stop_animation(token);
        render_graphic(*current_node->value);
        oled_config.n_image += 1;
        via_oled_config_save_value();
    }
}

void graphic_backward_kb(void) {
    if (backward(&current_node)) {
        qp_stop_animation(token);
        render_graphic(*current_node->value);
        oled_config.n_image -= 1;
        via_oled_config_save_value();
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_IMG:
            model_oled_flash_img_jump();
            break;

        case KC_ESC:
            if (get_mods() == (MOD_BIT(KC_LALT) | MOD_BIT(KC_LCTL))) {
                if (record->event.pressed) {
                    model_oled_flash_img_jump();
                    return false;
                }
            }

        case KC_RIGHT:
            if (get_mods() == (MOD_BIT(KC_LALT) | MOD_BIT(KC_LCTL))) {
                if (record->event.pressed) {
                    graphic_forward_kb();
                    return false;
                }
            }
            break;
        case KC_LEFT:
            if (get_mods() == (MOD_BIT(KC_LALT) | MOD_BIT(KC_LCTL))) {
                if (record->event.pressed) {
                    graphic_backward_kb();
                    return false;
                }
            }
            break;

        case KC_OLED_TOGGLE:
            if (record->event.pressed) {
                oled_toggle();
            }
            break;

        default:
            if (record->event.pressed) {
                timeout_reset_timer();
            }
            break;
    }
    return process_record_keymap(keycode, record);
}

static uint16_t timeout_timer   = 0;
static uint16_t timeout_counter = 0; // in minute intervals

uint16_t get_timeout_threshold(void) {
    if (oled_config.timeout_enable) {
        return TIMEOUT_TRESHOLD;
    } else {
        return 0;
    }
}

void timeout_reset_timer(void) {
    timeout_timer   = timer_read();
    timeout_counter = 0;
    oled_on();
};

void timeout_tick_timer(void) {
    // timeout_threshold = 0 will disable timeout
    uint16_t timeout_threshold = get_timeout_threshold();
    if (timeout_threshold > 0) {
        if (timer_elapsed(timeout_timer) >= 60000) { // 1 minute tick
            timeout_counter++;
            timeout_timer = timer_read();
        }
        if (timeout_threshold > 0 && timeout_counter >= timeout_threshold) {
            oled_dim();
            if (timeout_counter >= OLED_SLEEP_THREHOLD) {
                oled_off();
            }
        }
    }
}

void housekeeping_task_kb(void) {
    timeout_tick_timer();
};

// static THD_WORKING_AREA(waPainterThread, 256);
// static THD_FUNCTION(PainterThread, arg) {
//    void arg;
//   while (true) {
//      chThdYield();
// }
//}

// #define MATRIX_IO_DELAY 20
// void matrix_io_delay(void) {
//    wait_us(MATRIX_IO_DELAY);
//   chThdYield();
//}
// thread_t *thread = NULL;

int main(void) {
    platform_setup();

#ifdef USE_MSD_PROTOCOL
    if_requested_model_oled_flash();
#endif

    protocol_setup();
    keyboard_setup();
    protocol_init();
    /* Main loop */
    // thread = chThdCreateStatic(waPainterThread, sizeof(waPainterThread), NORMALPRIO, PainterThread, NULL);
    while (true) {
        protocol_task();
        // Run Quantum Painter task
        void qp_internal_task(void);
        qp_internal_task();
#ifdef DEFERRED_EXEC_ENABLE
        // Run deferred executions
        void deferred_exec_task(void);
        deferred_exec_task();
#endif // DEFERRED_EXEC_ENABLE

        housekeeping_task();
        chThdYield();
    }
}
