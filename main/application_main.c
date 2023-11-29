#include "esp_log.h"
#include "nvs_flash.h"

#include "modules/auto_control.c"
#include "modules/control_line.c"
#include "modules/wifi_control.c"
#include "modules/web_server.c"

#include "common_struct.h"


void app_main(void) {
    struct app_state_t *app_state = malloc(sizeof(struct app_state_t));
    *app_state = DEFAULT_APP_STATE;

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "WIFI_MODE_STA");
    wifi_init_sta();

    ESP_LOGI(TAG, "WEB_SERVER_STA");
    start_webserver(app_state);

    ESP_LOGI(TAG, "AUTO_CONTROL_STA");
    xTaskCreatePinnedToCore(auto_control_exec, "auto_control_task", 4096, app_state, 5, NULL, 0);

    ESP_LOGI(TAG, "UPDATE_LINE");
    xTaskCreatePinnedToCore(control_line_exec, "control_line_task", 4096, app_state, 5, NULL, 0);

}
