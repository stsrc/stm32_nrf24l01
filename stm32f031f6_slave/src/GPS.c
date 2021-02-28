/*
 * where-is-catiusha - cat localisation project
 * Copyright (C) 2021 Konrad Gotfryd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "GPS.h"
#include "stupid_delay.h"
#include "simple_buffer.h"
#include <errno.h>
#include "UART.h"

int gps_get_data(char *data, size_t data_size) {
	int ret;
	uint16_t cnt = 0;
	uint16_t multiplier = 100;
	const int delay = 1;

	if (data_size < 64)
		return -1;

	do {
		ret = buffer_SearchGetLabel(&UART1_receive_buffer, "$GPGLL,",
					    "\r\n", data, 0); //TODO data size problems
		if (ret)
			delay_ms(delay);
	} while (((ret == -EBUSY) || (ret == -EINVAL)) && (++cnt < multiplier));
	if (cnt == multiplier)
		return -EBUSY;
	return 0;
}

int gps_init() {
	UART_1_init();
	return 0;
}
