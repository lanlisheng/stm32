/**
 ******************************************************************************
 * @addtogroup SMART SMART Core hardware abstraction layer
 * @{
 * @addtogroup SMART_IRQ IRQ Setup Functions
 * @{
 *
 * @file       smart_irq.h  
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * 	       Parts by Thorsten Klose (tk@midibox.org)
 * @brief      IRQ functions header.
 * @see        The GNU Public License (GPL) Version 3
 * @Add by Richile
 *
 *****************************************************************************/
/* 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef SMART_IRQ_H
#define SMART_IRQ_H

#include <stdint.h>
#include <stdbool.h>

/* Public Functions */
extern int32_t SMART_IRQ_Disable(void);
extern int32_t SMART_IRQ_Enable(void);
extern bool SMART_IRQ_InISR(void);

#endif /* SMART_IRQ_H */
