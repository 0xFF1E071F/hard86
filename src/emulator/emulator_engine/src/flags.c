/**
* @file flag modifying routines
* x86 emulator engine for hard86
* Stephen Zhang, 2014
*/

#include "flags.h"
#include "emulator_engine.h"

extern sys_state_type sys_state;

/*	CPU flags are :
0	NT	IOPL	OF	DF	IF	TF	SF	ZF	0	AF	0	PF	1	CF

NT and IOPL flags are 80386+ only, so we don't need to worry about
them

Control Flags:
DF		Direction
IF		Interrupt flag
TF		Trap - Single step for debugging

Status Flags:
CF		Carry flag
PF		Parity flag
Set if the least-significant byte of the result contains an even number of 1 bits;
cleared otherwise.
AF		Adjust flag
Set if an arithmetic operation generates a carry or a borrow out of bit 3 of the
result; cleared otherwise. This flag is used in binary-coded decimal (BCD) arithmetic.
ZF		Zero flag
SF		Sign flag
Equal to most significant bit of value (1 if negative, 0 if positive)
OF		Overflow flag
*/

static uint8 parity[256]={
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00
};

void set_df(int v){
	FLAG_DF=v ? 1 : 0;
}

void set_if(int v){
	FLAG_IF=v ? 1 : 0;
}

void set_tf(int v){
	FLAG_TF=v ? 1 : 0;
}

void set_cf(int v){
	FLAG_CF=v ? 1 : 0;
}

void set_pf(uint32 v){
	FLAG_PF=parity[v & 255];
}

void set_af(){
	/** TODO **/
}

void set_zf(uint32 v){
	FLAG_ZF=v ? 0 : 1;
}

void set_sf(uint32 v){
	FLAG_SF=v;
}

void set_of(int v){
	FLAG_OF=v ? 1 : 0;
}

void setf_add16(uint16 a, uint16 b){
	set_of(((uint32)a+b) >> 16 ? 1 : 0);
	set_sf(((a+b) & 0x8000)>>15);
	set_zf(a+b);
	set_pf(a+b);
	set_cf((uint32)a+b >> 16 ? 1 : 0);
}

void setf_add8(uint8 a, uint8 b){
	set_of(((uint16)a+b) >> 8 ? 1 : 0);
	set_sf(((a+b) & 0x80)>>7);
	set_zf(a+b);
	set_pf(a+b);
	set_cf((uint16)a+b >> 8 ? 1 : 0);
}

void setf_adc16(uint16 a, uint16 b, uint16 c){
	setf_add16(a, b+c);
}

void setf_adc8(uint8 a, uint8 b, uint8 c){
	setf_add8(a, b+c);
}

/* a - b */
void setf_sub16(uint16 a, uint16 b){
	set_of(((uint32)a-b) >> 16 ? 1 : 0);
	set_sf(((a-b) & 0x8000)>>15);
	set_zf(a-b);
	set_pf(a-b);
	set_cf(a < b);
}

void setf_sub8(uint8 a, uint8 b){
	set_of(((uint16)a-b) >> 8 ? 1 : 0);
	set_sf(((a-b) & 0x80)>>7);
	set_zf(a-b);
	set_pf(a-b);
	set_cf(a < b);
}