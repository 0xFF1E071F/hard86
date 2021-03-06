/*  Hard86 - An 8086 Emulator with support for virtual hardware
	
    Copyright (C) 2014 Stephen Zhang

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.	
*/

#ifndef EMULATOR_ENGINE_INTERFACE_H
#define EMULATOR_ENGINE_INTERFACE_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <wchar.h>

#include "emulator_engine.h"
#include "../../../global/defines.h"
#include "../../../global/typedefs.h"

/* system multithreading */
#include "../../../system/multithreading/event/c/event.h"
#include "../../../system/multithreading/mutex/c/cmutex.h"
#include "../../../system/multithreading/thread/c/thread.h"

	typedef struct{
		/* system memory */
		uint8* mem;
		uint32 mem_size;

		/* I/O bus */
		/*	The I/O address space consists of 2 16 (64K)
		individually addressable 8-bit I/O ports,
		numbered 0 through FFFFH
		*/
		uint8* io_bus;
		/* registers */
		/*	general purpose
		AX, BX, CX, DX

		we store this as an array of uint8
		so we can easily access h.o byte and l.o byte
		*/
		uint8 ax[2];
		uint8 bx[2];
		uint8 cx[2];
		uint8 dx[2];

		uint16 sp;
		uint16 bp;
		uint16 si;
		uint16 di;

		/* special */
		/* segment registers */
		uint16 cs;
		uint16 ss;
		uint16 ds;
		uint16 es;

		/* pointers & indicators */
		uint16 ip;
		uint32 r_ip;	/*	this is the 'real' ip value
						that is, not segmented	*/

		/*	flags register
		format :
		Bit# : 15	14	13/12	11	10	9	8	7	6	5	4	3	2	1	0
		Flag : 0	NT	IOPL	OF	DF	IF	TF	SF	ZF	0	AF	0	PF	1	CF
		*/
		union{
			struct{
				uint8 CF : 1;
				uint8 reserved_1 : 1;	/* always 1 */
				uint8 PF : 1;
				uint8 reserved_2 : 1; /* always 0 */
				uint8 AF : 1;
				uint8 reserved_3 : 1;	/* always 0 */
				uint8 ZF : 1;
				uint8 SF : 1;
				uint8 TF : 1;
				uint8 IF : 1;
				uint8 DF : 1;
				uint8 OF : 1;
				uint8 IOPL : 2;
				uint8 NT : 1;
				uint8 reserved_4 : 1;	/* 1 on 8086, 0 above */
			}f_bits;
			uint16 flags;
		};

		// Emulator specific data (non-8086 related ... kinda)
		MUTEX sys_mutex;

		/*
			* True if we want the debugger callbacks to be executed
			* for each opcode while we are in an external interrupt.
			* False if we want external interrupts to be 'invisible'
			* to the rest of the program
			*/
		int step_through_extern_int;
		int is_in_extern_int;	// in external interrupt?

		int step_through_int;
		int is_in_int;			// in interrupt?

		op_data_ptr op_data;

	}sys_state_type, *sys_state_ptr;

typedef void(*DBGCALLBACK)(MUTEX, sys_state_ptr);

int system_init(MUTEX sys_mutex_,
				DBGCALLBACK bp_hit_func,
				DBGCALLBACK pre_ex_func,
				DBGCALLBACK pos_ex_func,
				DBGCALLBACK user_input_func);
int system_destroy();
int system_execute();

void system_halt();

sys_state_ptr get_system_state();

uint16 read_mem_16(uint32 addr);
void write_mem_16(uint16 val, uint32 addr);
uint8 read_mem_8(uint32 addr);
void write_mem_8(uint8 val, uint32 addr);

void write_io_port(uint8 val, uint16 port);
uint8 read_io_port(uint16 port);

void system_load_mem(uint8* data, uint32 size);
void system_load_bios(uint8* data, uint16 size);

void write_reg(int reg, uint16 val);
uint16 read_reg(int reg);

void stack_push(uint16 val);
uint16 stack_pop();
int stack_empty();

void extern_int(uint8 inum);
void set_step_through_extern_int(int v);
int* get_is_in_extern_int();

void set_step_through_int(int v);
int* get_is_in_int();

void set_sys_mutex(MUTEX mutex);

#ifdef __cplusplus
}
#endif

#endif