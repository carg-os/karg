#pragma once

typedef enum {
    LOG_SEVERITY_DEBUG,
    LOG_SEVERITY_INFO,
    LOG_SEVERITY_WARNING,
    LOG_SEVERITY_ERROR,
    LOG_SEVERITY_CRITICAL,
    LOG_SEVERITY_PANIC,
} log_severity_t;

void klogf(log_severity_t severity, const char *fmt, ...);

#define log_debug(fmt, ...)                                                    \
    klogf(LOG_SEVERITY_DEBUG, "%s: " fmt "\n", _MODULE_NAME, ##__VA_ARGS__)
#define log_info(fmt, ...)                                                     \
    klogf(LOG_SEVERITY_INFO, "%s: " fmt "\n", _MODULE_NAME, ##__VA_ARGS__)
#define log_warning(fmt, ...)                                                  \
    klogf(LOG_SEVERITY_WARNING, "%s: " fmt "\n", _MODULE_NAME, ##__VA_ARGS__)
#define log_error(fmt, ...)                                                    \
    klogf(LOG_SEVERITY_ERROR, "%s: " fmt "\n", _MODULE_NAME, ##__VA_ARGS__)
#define log_critical(fmt, ...)                                                 \
    klogf(LOG_SEVERITY_CRITICAL, "%s: " fmt "\n", _MODULE_NAME, ##__VA_ARGS__)
#define log_panic(fmt, ...)                                                    \
    klogf(LOG_SEVERITY_PANIC, "%s: " fmt "\n", _MODULE_NAME, ##__VA_ARGS__);
