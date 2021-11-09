cmake_minimum_required(VERSION 3.5)

set(PROJ_SRC_DIR "../project_sources")
set(IMG_DIR "../images")
set(CERT_DIR "../certificates")

set(PROJ_SOURCES 
        main.c
        "${PROJ_SRC_DIR}/wifi/wifi_connect.c"
        "${PROJ_SRC_DIR}/imu/imu_sample.c"
        "${PROJ_SRC_DIR}/aws/aws_connect.c"
        "${PROJ_SRC_DIR}/aws/aws_mqtt_msg.c"
        "${PROJ_SRC_DIR}/misc/app_nvs.c"
        "${PROJ_SRC_DIR}/gui/gui.c"
        "${PROJ_SRC_DIR}/gui/gui_theme.c"
        "${PROJ_SRC_DIR}/gui/gui_animation.c"
        "${PROJ_SRC_DIR}/gui/screens.c"
        "${PROJ_SRC_DIR}/gui/pairing_screen.c"
        "${PROJ_SRC_DIR}/gui/play_screen.c"
        "${PROJ_SRC_DIR}/gui/start_screen.c"
        "${PROJ_SRC_DIR}/gui/wifi_screen.c"
        "${PROJ_SRC_DIR}/gui/watch_screen.c"
        "${PROJ_SRC_DIR}/state/app_state.c"
        "${PROJ_SRC_DIR}/processing/app_processing.c"
        "${IMG_DIR}/img_watch_dog.c"
        "${IMG_DIR}/img_sleepy_dog.c"
        "${IMG_DIR}/img_play_dog.c"
        "${IMG_DIR}/img_normal_dog.c"
        "${IMG_DIR}/icon_wifi.c"
        "${IMG_DIR}/icon_left_arrow.c"
        )

set(PROJ_INCLUDES 
        .
        "${PROJ_SRC_DIR}/gui"
        "${PROJ_SRC_DIR}/misc"
        "${PROJ_SRC_DIR}/imu"
        "${PROJ_SRC_DIR}/aws"
        "${PROJ_SRC_DIR}/wifi"
        "${PROJ_SRC_DIR}/state"
        "${PROJ_SRC_DIR}/processing"
        "${CERT_DIR}"
)

set(PROJ_REQUIRES
        lvgl 
        lvgl_esp32_drivers
        lvgl_touch 
        lvgl_tft 
        lv_examples
        esp_mpu60x0_C
        esp-aws-iot
        fatfs
        nvs_flash
)

idf_component_register( SRCS            ${PROJ_SOURCES}
                        INCLUDE_DIRS    ${PROJ_INCLUDES}
                        REQUIRES        ${PROJ_REQUIRES}
)   

target_add_binary_data(${COMPONENT_TARGET} "../certificates/aws-root-ca.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../certificates/certificate.pem.crt" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../certificates/private.pem.key" TEXT)

target_compile_definitions(${COMPONENT_LIB}  PRIVATE LV_CONF_INCLUDE_SIMPLE=1)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-function -Wno-unused-variable -Wno-deprecated-declarations)



