/*
 * opt3001.h
 *
 *  Created on: Oct 20, 2020
 *      Author: Joe Krachey
 */

#ifndef LM75_H_
#define LM75_H_

#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

/* ADD CODE */
#define LM75_SUBORDINATE_ADDR                 0x4F

/* ADD CODE */
#define LM75_TEMP_REG						  0x00

/**
 *
 *
 *
 */
cy_rslt_t LM75_init(void);

/** Read the temperature from LM75
 *
 * @param
 *
 */
float LM75_get_temp(void);

#endif /* LM75_H_ */
