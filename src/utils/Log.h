#ifndef LOG_H
#define LOG_H

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_NONE 99

#define LOG_LEVEL LOG_LEVEL_ERROR

#if (LOG_LEVEL == LOG_LEVEL_DEBUG)
#define DEBUG(__fmt, ...) Serial.printf("[DEBUG] %4d %-50s " __fmt"\n", __LINE__, __FILE__, ##__VA_ARGS__)
#else
#define DEBUG(__fmt, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define LOG(__fmt, ...) Serial.printf("[INFO ] %4d %-50s " __fmt"\n", __LINE__, __FILE__, ##__VA_ARGS__)
#else
#define  LOG(__fmt, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARN)
#define WARN(__fmt, ...) Serial.printf("[WARN ] %4d %-50s " __fmt"\n", __LINE__, __FILE__, ##__VA_ARGS__)
#else
#define  WARN(__fmt, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define ERROR(__fmt, ...) Serial.printf("[ERROR] %4d %-50s " __fmt"\n", __LINE__, __FILE__, ##__VA_ARGS__)
#else
#define  ERROR(__fmt, ...)
#endif

#endif /*LOG_H*/