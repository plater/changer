# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS main.c gpios.c i2c_bits.c display.c pay.c test.c     # list the source files of this component
    INCLUDE_DIRS 		       # optional, add here public include directories
    REQUIRES            esp_driver_gpio esp_hal_gpio esp_timer nvs_flash
)
