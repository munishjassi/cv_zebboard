#include "hardware.c"

#define SOBEL_FILTER "sobel_filter_top"
#define ERODE_FILTER "erode_filter_top"
#define GRAY_FILTER "gray_filter_top"
//#define ARCH_DIR "/sys/devices/soc0"
#define ARCH_DIR "/sys/devices/fpga-axi.1"

bool SOBEL_HW = false, ERODE_HW= false, GRAY_HW = false;
char SOBEL_DEV[50] = "/dev/", ERODE_DEV[50] = "/dev/", GRAY_DEV[50] = "/dev/";
