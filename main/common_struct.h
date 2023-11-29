#ifndef TEST_PROJECT_COMMON_STRUCT_H
#define TEST_PROJECT_COMMON_STRUCT_H

struct app_state_t {
    bool common_auto_mode;
    uint8_t individual_auto_mode;  // 0.0.0.0.0.line3.line2.line1
    uint8_t line_state;  // 0.0.0.0.0.line3.line2.line1
};
const struct app_state_t DEFAULT_APP_STATE = {1, 7, 0 };

#endif //TEST_PROJECT_COMMON_STRUCT_H
