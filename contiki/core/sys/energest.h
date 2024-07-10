
/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Header file for the energy estimation mechanism
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#ifndef ENERGEST_H_
#define ENERGEST_H_

#include "sys/rtimer.h"

typedef struct {
  /*  unsigned long cumulative[2];*/
  unsigned long current;
} energest_t;

enum energest_type {
  ENERGEST_TYPE_CPU,
  ENERGEST_TYPE_LPM,
  ENERGEST_TYPE_IRQ,
  ENERGEST_TYPE_LED_GREEN,
  ENERGEST_TYPE_LED_YELLOW,
  ENERGEST_TYPE_LED_RED,
  ENERGEST_TYPE_TRANSMIT,
  ENERGEST_TYPE_LISTEN,

  ENERGEST_TYPE_FLASH_READ,
  ENERGEST_TYPE_FLASH_WRITE,

  ENERGEST_TYPE_SENSORS,

  ENERGEST_TYPE_SERIAL,

  ENERGEST_TYPE_MAX
};
// định nghĩa các biến/ các chế độ được định nghĩa trong contiki

void energest_init(void);
unsigned long energest_type_time(int type);// trả về thời gian đã trôi qua của một loại chế độ cụ thể được định nghĩa bởi
#ifdef ENERGEST_CONF_LEVELDEVICE_LEVELS// kiểm tra nếu biến được định nghĩa thì thuwcjhieenj bt dưới ko thì kết thúc
unsigned long energest_leveldevice_leveltime(int powerlevel);// lấy đầu và và trả về mức tiêu thụ ở chế độ đó.
#endif
void energest_type_set(int type, unsigned long value);
void energest_flush(void);

#if ENERGEST_CONF_ON
/*extern int energest_total_count;*/// khởi tạo các mảng với số ptu max đc quy định  dưới đây như sau.
extern energest_t energest_total_time[ENERGEST_TYPE_MAX];
extern rtimer_clock_t energest_current_time[ENERGEST_TYPE_MAX];
extern unsigned char energest_current_mode[ENERGEST_TYPE_MAX];

#ifdef ENERGEST_CONF_LEVELDEVICE_LEVELS
extern energest_t energest_leveldevice_current_leveltime[ENERGEST_CONF_LEVELDEVICE_LEVELS];
#endif

#define ENERGEST_ON(type)  do { \
                           /*++energest_total_count;*/ \
                           energest_current_time[type] = RTIMER_NOW(); \
			   energest_current_mode[type] = 1; \
                           } while(0)
#ifdef __AVR__// nếu avr đưuọc định nghĩa thì thực hiện các define dưới đây
/* Handle 16 bit rtimer wraparound */
// check chế độ hiện tại của type#0 ko nếu không tính toán thời gian từ khi nó dùng ENERGEST_ON đến hiện tại và gắn lại type về 0.
// hàm RTIMER_NOW() có tác dụng tính thời gian ở thời điểm hiện tại.
/* định nghĩa các biến: energest_t energest_total_time[type] trả về tông thời gian mà nó ở chế độ type.
 rtimer_clock_t energest_current_time[ENERGEST_TYPE_MAX]: trả về thời gian khi node  vào chế độ đó. 
 unsigned char energest_current_mode[ENERGEST_TYPE_MAX]; trả về chế độ hiện tại của nó.
 RTIMER_ARCH_SECOND: biến đươc định nghĩa là thơi gian một vòng lặp của đồng hồ h/thống */
#define ENERGEST_OFF(type) if(energest_current_mode[type] != 0) do {	\
							if (RTIMER_NOW() < energest_current_time[type]) energest_total_time[type].current += RTIMER_ARCH_SECOND; \
							energest_total_time[type].current += (rtimer_clock_t)(RTIMER_NOW() - \
							energest_current_time[type]); \
							energest_current_mode[type] = 0; \
                           } while(0)
                          

#define ENERGEST_OFF_LEVEL(type,level) do { \
										if (RTIMER_NOW() < energest_current_time[type]) energest_total_time[type].current += RTIMER_ARCH_SECOND; \
										energest_leveldevice_current_leveltime[level].current += (rtimer_clock_t)(RTIMER_NOW() - \
										energest_current_time[type]); \
										energest_current_mode[type] = 0; \
                                       } while(0)

#define ENERGEST_SWITCH(type_off, type_on) do { \
                                             rtimer_clock_t energest_local_variable_now = RTIMER_NOW(); \
                                             if(energest_current_mode[type_off] != 0) { \
                                               if (energest_local_variable_now < energest_current_time[type_off]) { \
                                                 energest_total_time[type_off].current += RTIMER_ARCH_SECOND; \
                                               } \
                                               energest_total_time[type_off].current += (rtimer_clock_t)(energest_local_variable_now - \
                                                 energest_current_time[type_off]); \
                                               energest_current_mode[type_off] = 0; \
                                             } \
                                             //rtimer_clock_t energest_trans = RTIMER_NOW(); // đặt để tính thời gia chuyển trạng thái của nó
                                             //energest_current_time[type_on] = energest_trans;
                                             energest_current_time[type_on] = energest_local_variable_now; \
                                             energest_current_mode[type_on] = 1; \
                                           } while(0)

#else // nếu avr chưa đươc define thì sẽ thực hiện lại các define sau.
#define ENERGEST_OFF(type) if(energest_current_mode[type] != 0) do {	\
                           energest_total_time[type].current += (rtimer_clock_t)(RTIMER_NOW() - \
                           energest_current_time[type]); \
			   energest_current_mode[type] = 0; \
                           } while(0)

#define ENERGEST_OFF_LEVEL(type,level) do { \
                                        energest_leveldevice_current_leveltime[level].current += (rtimer_clock_t)(RTIMER_NOW() - \
			                energest_current_time[type]); \
			   energest_current_mode[type] = 0; \
                                        } while(0)
// hàm chuyển đổi trạng thái của node
#define ENERGEST_SWITCH(type_off, type_on) do { \
                                             rtimer_clock_t energest_local_variable_now = RTIMER_NOW(); \
                                             if(energest_current_mode[type_off] != 0) { \
                                               energest_total_time[type_off].current += (rtimer_clock_t)(energest_local_variable_now - \
                                                 energest_current_time[type_off]); \
                                               energest_current_mode[type_off] = 0; \
                                             } \
                                             energest_current_time[type_on] = energest_local_variable_now; \
                                             energest_current_mode[type_on] = 1; \
                                           } while(0)
#endif

#else /* ENERGEST_CONF_ON */
#define ENERGEST_ON(type) do { } while(0)
#define ENERGEST_OFF(type) do { } while(0)
#define ENERGEST_OFF_LEVEL(type,level) do { } while(0)
#define ENERGEST_SWITCH(type_off, type_on) do { } while(0)
#endif /* ENERGEST_CONF_ON */

#endif /* ENERGEST_H_ */
