set(PROJ_SRC_DIR "../project_sources")
set(IMG_DIR "../images")

set(PROJ_SOURCES 
        main.c
        "${PROJ_SRC_DIR}/gui/screens.c"
        "${IMG_DIR}/img_watch_dog.c"
        "${IMG_DIR}/img_sleepy_dog.c"
)

set(PROJ_INCLUDES 
        .
        "${PROJ_SRC_DIR}/gui"
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

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-function -Wno-deprecated-declarations)



