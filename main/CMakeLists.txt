idf_component_register(SRCS "application_main.c" "common_struct.h"
#                            "modules/headers/auto_control.h" "modules/headers/led_control.h" "modules/headers/web_server.h" "modules/headers/wifi_control.h"
                            "modules/wifi_control.c" "modules/web_server.c" "modules/led_control.c" "modules/auto_control.c" "modules/control_line.c" "modules/dynamic_illumination.c"
                        REQUIRES esp_https_server nvs_flash led_strip esp_adc_cal
                        EMBED_FILES "misc/resp.html.gz")