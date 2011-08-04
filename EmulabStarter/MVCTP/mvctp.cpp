/*
 * mvctp.cpp
 *
 *  Created on: Jun 30, 2011
 *      Author: jie
 */
#include "mvctp.h"

void SysError(string s) {
	perror(s.c_str());
	exit(-1);
}

void Log(char* format, ...) {
	if (!is_debug)
		return;

	if (log_file == NULL) {
		log_file = fopen("mvctp_run.log", "w");
	}

	va_list args;
	va_start (args, format);
	vfprintf (log_file, format, args);
	va_end (args);
}


void AccessCPUCounter(unsigned *hi, unsigned *lo) {
	asm("rdtsc; movl %%edx, %0; movl %%eax, %1"
			: "=r" (*hi), "=r" (*lo)
			:
			: "%edx", "%eax");
}


double GetElapsedCycles(unsigned cycle_hi, unsigned cycle_lo) {
	unsigned ncycle_hi, ncycle_lo;
	unsigned hi, lo, borrow;
	double result;

	AccessCPUCounter(&ncycle_hi, &ncycle_lo);

	lo = ncycle_lo - cycle_lo;
	borrow = lo > ncycle_lo;
	hi = ncycle_hi - cycle_hi - borrow;
	result = (double) hi * (1 << 30) * 4 + lo;
	if (result < 0) {
		SysError("GetElapsedCycles(): counter returns negative value");
	}
	return result;
}


double GetCPUMhz() {
	double rate;
	unsigned hi, lo;
	AccessCPUCounter(&hi, &lo);
	sleep(1);
	rate = GetElapsedCycles(hi, lo) / 1 / 1000000;
	return rate;
}

double GetElapsedSeconds(CpuCycleCounter lastCount) {
	return GetElapsedCycles(lastCount.hi, lastCount.lo) / 1000000.0 / CPU_MHZ;
}


