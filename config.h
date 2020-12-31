// => Hardware select
// #define LILYGO_WATCH_2019_WITH_TOUCH     // To use T-Watch2019 with touchscreen, please uncomment this line
// #define LILYGO_WATCH_2019_NO_TOUCH          // To use T-Watch2019 Not touchscreen , please uncomment this line
#define LILYGO_WATCH_2020_V1             //To use T-Watch2020, please uncomment this line


// NOT SUPPORT ...
//// #define LILYGO_WATCH_BLOCK
// NOT SUPPORT ...

// => Function select
#define LILYGO_WATCH_LVGL                   //To use LVGL, you need to enable the macro LVGL

#define TWATCH_USE_PSRAM_ALLOC_LVGL
#define TWATCH_LVGL_DOUBLE_BUFFER
#define LILYGO_WATCH_2019_WITH_TOUCH
#define LILYGO_WATCH_LVGL_FS
#define LILYGO_WATCH_LVGL_FS_SPIFFS         // File System
#define LILYGO_WATCH_HAS_BMA423             // Axis Sensor


#include  <LilyGoWatch.h>
