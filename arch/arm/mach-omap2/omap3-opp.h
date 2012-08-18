#ifndef __OMAP3_OPP_H_
#define __OMAP3_OPP_H_

#include <mach/omap-pm.h>

static struct omap_opp omap3630_mpu_rate_table[] = {
	{0, 0, 0},
	{S300M,  VDD1_OPP1, 0x25, 0x0, 0x0},
	{S600M,  VDD1_OPP2, 0x30, 0x0, 0x0},
	{S900M, VDD1_OPP3, 0x3c, 0x0, 0x0},
	{S1000M, VDD1_OPP4, 0x40, 0x0, 0x0},
	{S1100M, VDD1_OPP5, 0x4a, 0x0, 0x0},
//	{S1150M, VDD1_OPP5, 0x4b, 0x0, 0x0},
//	{S1250M, VDD1_OPP5, 0x4d, 0x0, 0x0},
//	{S1300M, VDD1_OPP5, 0x4e, 0x0, 0x0},
//	{S1400M, VDD1_OPP5, 0x50, 0x0, 0x0},
//	{S1500M, VDD1_OPP5, 0x50, 0x0, 0x0},
	{S1180M, VDD1_OPP6, 0x3f, 0x0, 0x0},
};

/*static struct omap_opp omap3630_dsp_rate_table[] = {
	{0, 0, 0},
	{S260M, VDD1_OPP1, 0x1f, 0x0, 0x0},
	{S520M, VDD1_OPP2, 0x2d, 0x0, 0x0},
	{S520M, VDD1_OPP3, 0x3d, 0x0, 0x0},
	{S520M,  VDD1_OPP4, 0x3e, 0x0, 0x0},
	{S520M,  VDD1_OPP5, 0x40, 0x0, 0x0},
	{S65M,  VDD1_OPP6, 0x40, 0x0, 0x0},
};*/

static struct omap_opp omap3630_dsp_rate_table[] = {
	{0, 0, 0},
	/*OPP1 (OPP50) - 1.0V*/
	{S260M, VDD1_OPP1, 0x25, 0x0, 0x0},
	/*OPP2 (OPP100) - 1.1625V*/
	{S520M, VDD1_OPP2, 0x2f, 0x0, 0x0},
	/*OPP3 (OPP130) - 1.3V*/
	{S520M, VDD1_OPP3, 0x3b, 0x0, 0x0},
	/*OPP4 (OPP-1G) - 1.35V*/
	{S800M, VDD1_OPP4, 0x3c, 0x0, 0x0},
	/*OPP5 (OPP-1.3G) - 1.35V*/
//	{S600M, VDD1_OPP5, 0x3d, 0x0, 0x0},//  ok works!!
	{S520M, VDD1_OPP5, 0x3e, 0x0, 0x0},// test -> 
//	{S800M, VDD1_OPP5, 0x3d, 0x0, 0x0}, //test -> marche pas - plante
//	{S800M, VDD1_OPP5, 0x3b, 0x0, 0x0}, //test_UV -> rame un peu en 3D...
//	{S700M, VDD1_OPP5, 0x3c, 0x0, 0x0}, //test_UV -> 
	/*OPP5 (OPP-1.3G) - 1.35V*/
	{S700M, VDD1_OPP6, 0x3f, 0x0, 0x0},
};



static struct omap_opp omap3630_l3_rate_table[] = {
	{0, 0, 0},
	/*OPP1 (OPP50) - 0.97V*/
	{S166M, VDD2_OPP1, 0x1f, 0x0, 0x0},
	/*OPP2 (OPP100) - 1.1625V*/
	{S200M, VDD2_OPP2, 0x2f, 0x0, 0x0},
};

//static struct omap_opp omap3611_l3_rate_table[] = {
//	{0, 0, 0},
	/*OPP1 (OPP50) - 0.97V*/
//	{S83M, VDD2_OPP1, 0x1e, 0x0, 0x0},
	/*OPP2 (OPP100) - 1.1625V*/
//	{S166M, VDD2_OPP2, 0x2d, 0x0, 0x0},
//};


#endif