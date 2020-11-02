# set(COMPONENT_REQUIRES )

# set(COMPONENT_SRCS "main.c")
# set(COMPONENT_ADD_INCLUDEDIRS "")

set(SOURCES main.c)

idf_component_register( SRCS            ${SOURCES}
                        INCLUDE_DIRS    .
                        REQUIRES        lvgl 
                                        lvgl_esp32_drivers
                                        lvgl_touch 
                                        lvgl_tft 
                                        lv_examples
                                        esp_mpu60x0_C
                                        esp-aws-iot
                                        fatfs
                                        nvs_flash
)   

target_add_binary_data(${COMPONENT_TARGET} "../certificates/aws-root-ca.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../certificates/certificate.pem.crt" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../certificates/private.pem.key" TEXT)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
