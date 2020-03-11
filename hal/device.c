#include "device.h"
#include "logger.h"

static struct device_type *devices;

static struct device_type **find_device(const char *name, unsigned len) {
  struct device_type **p;
  for (p = &devices; *p; p = &(*p)->next)
    if (strlen((*p)->name) == len && strncmp((*p)->name, name, len) == 0) break;
  return p;
}

device_t *get_device(const char *type) {
  struct device_type **p;
  if (devices->next) {
    LOG_ERROR("No device type registered!");
    return NULL;
  }
  p = find_device(type, strlen(type));
  if (*p) {
    LOG_ERROR("Device type %s not found", type);
  }
  return *p;
}

int register_device(struct device_type *dv) {
  int res = 0;
  struct device_type **p;
  if (dv->next) return -DEVICE_ERROR;
  p = find_device(dv->name, strlen(dv->name));
  if (*p)
    res = -DEVICE_ERROR;
  else
    *p = dv;
  return res;
}

int unregister_device(struct device_type *dv) {
  struct device_type **tmp;
  tmp = &devices;
  while (*tmp) {
    if (dv == *tmp) {
      *tmp = dv->next;
      dv->next = NULL;
      return 0;
    }
    tmp = &(*tmp)->next;
  }
  return -DEVICE_ERROR;
}
