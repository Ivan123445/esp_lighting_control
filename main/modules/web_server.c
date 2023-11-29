#include <esp_log.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"

#include <esp_http_server.h>

#include "../common_struct.h"

//const char on_resp[] = "<!DOCTYPE html><html><head><style type=\"text/css\">html {  font-family: Arial;  display: inline-block;  margin: 0px auto;  text-align: center;}h1{  color: #070812;  padding: 2vh;}.button {  display: inline-block;  background-color: #b30000; //red color  border: none;  border-radius: 4px;  color: white;  padding: 16px 40px;  text-decoration: none;  font-size: 30px;  margin: 2px;  cursor: pointer;}.button2 {  background-color: #364cf4; //blue color}.content {   padding: 50px;}.card-grid {  max-width: 800px;  margin: 0 auto;  display: grid;  grid-gap: 2rem;  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));}.card {  background-color: white;  box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);}.card-title {  font-size: 1.2rem;  font-weight: bold;  color: #034078}</style>  <title>ESP32 WEB SERVER</title>  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">  <link rel=\"icon\" href=\"data:,\">  <link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\"    integrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr\" crossorigin=\"anonymous\">  <link rel=\"stylesheet\" type=\"text/css\" ></head><body>  <h2>ESP32 WEB SERVER</h2>  <div class=\"content\">    <div class=\"card-grid\">      <div class=\"card\">        <p><i class=\"fas fa-lightbulb fa-2x\" style=\"color:#c81919;\"></i>     <strong>GPIO2</strong></p>        <p>GPIO state: <strong> ON</strong></p>        <p>          <form method=\"post\"><a href=\"/?on=1\"><button class=\"button\">ON</button></a></form>          <form method=\"post\"><a href=\"/?off=1\"><button class=\"button button2\">OFF</button></a></form>        </p>      </div>    </div>  </div></body></html>";

static struct app_state_t *app_state;

static const char *TAG_SERV = "web server";


esp_err_t sent_page(httpd_req_t *req)
{
//    extern const unsigned char resp_html_gz_start[] asm("_binary_control_led_form_html_gz_start");
//    extern const unsigned char resp_html_gz_end[] asm("_binary_control_led_form_html_gz_end");
    extern const unsigned char resp_html_gz_start[] asm("_binary_resp_html_gz_start");
    extern const unsigned char resp_html_gz_end[] asm("_binary_resp_html_gz_end");
    ssize_t resp_html_gz_len = resp_html_gz_end - resp_html_gz_start;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

    return httpd_resp_send(req, (const char *)resp_html_gz_start, resp_html_gz_len);
}

esp_err_t handler_control_led(httpd_req_t *req) {
    char _value[32];
    ESP_LOGI(TAG_SERV, "Post_request");

    int buf_len = httpd_req_get_url_query_len(req) + 1;
    char *buf = malloc(buf_len);
    ESP_LOGI(TAG_SERV, "buf_len: %d", buf_len);
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
        ESP_LOGI(TAG_SERV, "Found URL query: %s", buf);
        if (httpd_query_key_value(buf, "auto", _value, sizeof(_value)) == ESP_OK) {
            if (httpd_query_key_value(buf, "on", _value, sizeof(_value)) == ESP_OK) {
                ESP_LOGI(TAG_SERV, "on_value: %s", _value);
                app_state->common_auto_mode = 1;
            }
            if (httpd_query_key_value(buf, "off", _value, sizeof(_value)) == ESP_OK) {
                ESP_LOGI(TAG_SERV, "off_value: %s", _value);
                app_state->common_auto_mode = 0;
            }
        } else if (httpd_query_key_value(buf, "l1", _value, sizeof(_value)) == ESP_OK) {
            if (httpd_query_key_value(buf, "on", _value, sizeof(_value)) == ESP_OK) {
                ESP_LOGI(TAG_SERV, "on_value: %s", _value);
                app_state->line_state |= 0b001;
            }
            if (httpd_query_key_value(buf, "off", _value, sizeof(_value)) == ESP_OK) {
                ESP_LOGI(TAG_SERV, "off_value: %s", _value);
                app_state->line_state &= 0b110;
            }
        } else if (httpd_query_key_value(buf, "l2", _value, sizeof(_value)) == ESP_OK) {
            if (httpd_query_key_value(buf, "on", _value, sizeof(_value)) == ESP_OK) {
                ESP_LOGI(TAG_SERV, "on_value: %s", _value);
                app_state->line_state |= 0b010;
            }
            if (httpd_query_key_value(buf, "off", _value, sizeof(_value)) == ESP_OK) {
                ESP_LOGI(TAG_SERV, "off_value: %s", _value);
                app_state->line_state &= 0b101;
            }
        } else if (httpd_query_key_value(buf, "l3", _value, sizeof(_value)) == ESP_OK) {
            if (httpd_query_key_value(buf, "on", _value, sizeof(_value)) == ESP_OK) {
                ESP_LOGI(TAG_SERV, "on_value: %s", _value);
                app_state->line_state |= 0b100;
            }
            if (httpd_query_key_value(buf, "off", _value, sizeof(_value)) == ESP_OK) {
                ESP_LOGI(TAG_SERV, "off_value: %s", _value);
                app_state->line_state &= 0b011;
            }
        }
    }

    ESP_LOGI(TAG_SERV, "");
    return sent_page(req);
}

httpd_uri_t uri_get = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = sent_page,
        .user_ctx = NULL
};

httpd_uri_t uri_led = {
        .uri      = "/",
        .method   = HTTP_POST,
        .handler  = handler_control_led,
        .user_ctx = NULL
};

httpd_handle_t start_webserver(struct app_state_t *app_state_p)
{
    app_state = app_state_p;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_led);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server) {
        httpd_stop(server);
    }
}
