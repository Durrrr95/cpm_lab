#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ssid_config.h"
#include "lwip/api.h"
#include <string.h>
#include <stdarg.h>

#include "remote_debug.h"
#include "remote_config.h"
#include "servo_pwm.h"
#include "battery_monitor.h"




void task_pwm_test(void *pvParameters) {

    while(1) {
        for (uint32_t i = 1000; i < 2000; i+=10) {
            servo_pwm_set_steering(i);
            servo_pwm_set_motor(CONFIG_VAR_motor_signal);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        for (int i = 2000; i > 1000; i-=10) {
            servo_pwm_set_steering(i);
            servo_pwm_set_motor(CONFIG_VAR_motor_signal);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}




void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    /** WiFi config **/
    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    /** Init modules **/
    init_remote_debug();
    init_servo_pwm();


    /** Disable 4th LED **/
    gpio_enable(0, GPIO_OUTPUT);
    gpio_write(0, 0);


    /** Start tasks **/
    xTaskCreate(task_remote_debug_sender, "task_remote_debug_sender", 512, NULL, 2, NULL);
    xTaskCreate(task_remote_config, "task_remote_config", 512, NULL, 2, NULL);
    //xTaskCreate(task_pwm_test, "task_pwm_test", 512, NULL, 2, NULL);
    xTaskCreate(task_battery_monitor, "task_battery_monitor", 512, NULL, 2, NULL);
}
