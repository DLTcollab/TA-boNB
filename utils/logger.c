#include "logger.h"
static int TA_LOG_LEVEL = LV_INFO;

int get_log_level(void) { return TA_LOG_LEVEL; }

void set_log_level(int level) { TA_LOG_LEVEL = level; }
