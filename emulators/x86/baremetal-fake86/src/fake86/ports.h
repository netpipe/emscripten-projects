#pragma once

#include "types.h"

typedef void(*write_redirector)(uint16_t portnum, uint8_t value);
typedef uint8_t(*read_redirector)(uint16_t portnum);
typedef void(*write_redirector_16)(uint16_t portnum, uint16_t value);
typedef uint16_t(*read_redirector_16)(uint16_t portnum);

void set_port_write_redirector(uint16_t startport, uint16_t endport, write_redirector callback);
void set_port_read_redirector(uint16_t startport, uint16_t endport, read_redirector callback);
void set_port_write_redirector_16(uint16_t startport, uint16_t endport, write_redirector_16 callback);
void set_port_read_redirector_16(uint16_t startport, uint16_t endport, read_redirector_16 callback);

void	portout(uint16_t portnum, uint8_t value);
void	portout16(uint16_t portnum, uint16_t value);
uint8_t	portin(uint16_t portnum);
uint16_t portin16(uint16_t portnum);

extern write_redirector* port_write_callback;
extern read_redirector* port_read_callback;
extern write_redirector_16* port_write_callback16;
extern read_redirector_16* port_read_callback16;
