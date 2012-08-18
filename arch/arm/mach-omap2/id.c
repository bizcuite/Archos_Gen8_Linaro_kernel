/*
 * linux/arch/arm/mach-omap2/id.c
 *
 * OMAP2 CPU identification code
 *
 * Copyright (C) 2005 Nokia Corporation
 * Written by Tony Lindgren <tony@atomide.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#include <asm/cputype.h>

#include <mach/common.h>
#include <mach/control.h>
#include <mach/cpu.h>

static struct omap_chip_id omap_chip;

static unsigned int omap_revision, omap_revision_id;
char *rev_name = "ES1.0                         ";

char * omap_rev_name(void)
{
	return rev_name;
}
EXPORT_SYMBOL(rev_name);

unsigned int omap_rev(void)
{
	return omap_revision;
}
EXPORT_SYMBOL(omap_rev);

unsigned int omap_rev_id(void)
{
	return omap_revision_id;
}
EXPORT_SYMBOL(omap_rev_id);

/**
 * omap_chip_is - test whether currently running OMAP matches a chip type
 * @oc: omap_chip_t to test against
 *
 * Test whether the currently-running OMAP chip matches the supplied
 * chip type 'oc'.  Returns 1 upon a match; 0 upon failure.
 */
int omap_chip_is(struct omap_chip_id oci)
{
	return (oci.oc & omap_chip.oc) ? 1 : 0;
}
EXPORT_SYMBOL(omap_chip_is);

int omap_type(void)
{
	u32 val = 0;

	if (cpu_is_omap24xx()) {
		val = omap_ctrl_readl(OMAP24XX_CONTROL_STATUS);
	} else if (cpu_is_omap34xx()) {
		val = omap_ctrl_readl(OMAP343X_CONTROL_STATUS);
	} else {
		pr_err("Cannot detect omap type!\n");
		goto out;
	}

	val &= OMAP2_DEVICETYPE_MASK;
	val >>= 8;

out:
	return val;
}
EXPORT_SYMBOL(omap_type);


/*----------------------------------------------------------------------------*/

#define OMAP_TAP_IDCODE		0x0204
#define OMAP_TAP_DIE_ID_0	0x0218
#define OMAP_TAP_DIE_ID_1	0x021C
#define OMAP_TAP_DIE_ID_2	0x0220
#define OMAP_TAP_DIE_ID_3	0x0224
#define DIE_ID_REG_BASE 	(L4_WK_34XX_PHYS + 0xA000)
#define DIE_ID_REG_OFFSET	0x218

#define read_tap_reg(reg)	__raw_readl(tap_base  + (reg))

struct omap_id {
	u16	hawkeye;	/* Silicon type (Hawkeye id) */
	u8	dev;		/* Device type from production_id reg */
	u32	type;		/* Combined type id copied to omap_revision */
};

/* Register values to detect the OMAP version */
static struct omap_id omap_ids[] __initdata = {
	{ .hawkeye = 0xb5d9, .dev = 0x0, .type = 0x24200024 },
	{ .hawkeye = 0xb5d9, .dev = 0x1, .type = 0x24201024 },
	{ .hawkeye = 0xb5d9, .dev = 0x2, .type = 0x24202024 },
	{ .hawkeye = 0xb5d9, .dev = 0x4, .type = 0x24220024 },
	{ .hawkeye = 0xb5d9, .dev = 0x8, .type = 0x24230024 },
	{ .hawkeye = 0xb68a, .dev = 0x0, .type = 0x24300024 },
};

static void __iomem *tap_base;
static u16 tap_prod_id;

void __init omap24xx_check_revision(void)
{
	int i, j;
	u32 idcode, prod_id;
	u16 hawkeye;
	u8  dev_type, rev;

	idcode = read_tap_reg(OMAP_TAP_IDCODE);
	prod_id = read_tap_reg(tap_prod_id);
	hawkeye = (idcode >> 12) & 0xffff;
	rev = (idcode >> 28) & 0x0f;
	dev_type = (prod_id >> 16) & 0x0f;

	pr_debug("OMAP_TAP_IDCODE 0x%08x REV %i HAWKEYE 0x%04x MANF %03x\n",
		 idcode, rev, hawkeye, (idcode >> 1) & 0x7ff);
	pr_debug("OMAP_TAP_DIE_ID_0: 0x%08x\n",
		 read_tap_reg(OMAP_TAP_DIE_ID_0));
	pr_debug("OMAP_TAP_DIE_ID_1: 0x%08x DEV_REV: %i\n",
		 read_tap_reg(OMAP_TAP_DIE_ID_1),
		 (read_tap_reg(OMAP_TAP_DIE_ID_1) >> 28) & 0xf);
	pr_debug("OMAP_TAP_DIE_ID_2: 0x%08x\n",
		 read_tap_reg(OMAP_TAP_DIE_ID_2));
	pr_debug("OMAP_TAP_DIE_ID_3: 0x%08x\n",
		 read_tap_reg(OMAP_TAP_DIE_ID_3));
	pr_debug("OMAP_TAP_PROD_ID_0: 0x%08x DEV_TYPE: %i\n",
		 prod_id, dev_type);

	/* Check hawkeye ids */
	for (i = 0; i < ARRAY_SIZE(omap_ids); i++) {
		if (hawkeye == omap_ids[i].hawkeye)
			break;
	}

	if (i == ARRAY_SIZE(omap_ids)) {
		printk(KERN_ERR "Unknown OMAP CPU id\n");
		return;
	}

	for (j = i; j < ARRAY_SIZE(omap_ids); j++) {
		if (dev_type == omap_ids[j].dev)
			break;
	}

	if (j == ARRAY_SIZE(omap_ids)) {
		printk(KERN_ERR "Unknown OMAP device type. "
				"Handling it as OMAP%04x\n",
				omap_ids[i].type >> 16);
		j = i;
	}

	pr_info("OMAP%04x", omap_rev() >> 16);
	if ((omap_rev() >> 8) & 0x0f)
		pr_info("ES%x", (omap_rev() >> 12) & 0xf);
	pr_info("\n");
}

void __init omap34xx_check_revision(void)
{
	u32 cpuid, idcode;
	u16 hawkeye;
	u8 rev;

	/* Exports DIE id control */
	unsigned int val[4];
	unsigned int reg;
	reg = DIE_ID_REG_BASE + DIE_ID_REG_OFFSET;

	val[0] = omap_readl(reg);
	val[1] = omap_readl(reg + 0x4);
	val[2] = omap_readl(reg + 0x8);
	val[3] = omap_readl(reg + 0xC);

	/*
	 * We cannot access revision registers on ES1.0.
	 * If the processor type is Cortex-A8 and the revision is 0x0
	 * it means its Cortex r0p0 which is 3430 ES1.0.
	 */
	cpuid = read_cpuid(CPUID_ID);
	if ((((cpuid >> 4) & 0xfff) == 0xc08) && ((cpuid & 0xf) == 0x0)) {
		omap_revision = OMAP3430_REV_ES1_0;
		goto out;
	}

	/*
	 * Detection for 34xx ES2.0 and above can be done with just
	 * hawkeye and rev. See TRM 1.5.2 Device Identification.
	 * Note that rev does not map directly to our defined processor
	 * revision numbers as ES1.0 uses value 0.
	 */
	idcode = read_tap_reg(OMAP_TAP_IDCODE);
	hawkeye = (idcode >> 12) & 0xffff;
	rev = (idcode >> 28) & 0xff;

	switch (hawkeye) {
	case 0xb7ae:
		/* Handle 34xx/35xx devices */
		switch (rev) {
		case 0:
			omap_revision = OMAP3430_REV_ES2_0;
			rev_name = "ES2.0";
			break;
		case 2:
			omap_revision = OMAP3430_REV_ES2_1;
			rev_name = "ES2.1";
			break;
		case 3:
			omap_revision = OMAP3430_REV_ES3_0;
			rev_name = "ES3.0";
			break;
		case 4:
			omap_revision = OMAP3430_REV_ES3_1;
			rev_name = "ES3.1";
			break;
		default:
			/* Use the latest known revision as default */
			omap_revision = OMAP3430_REV_ES3_1;
			rev_name = "Unknown revision\n";
		}
		break;
	case 0xb891:
		/* Handle 36xx devices */
		switch (rev) {
		case 0:
			omap_revision = OMAP3630_REV_ES1_0;
			rev_name = "ES1.0";
			break;
		case 1:
			omap_revision = OMAP3630_REV_ES1_1;
			rev_name = "ES1.1";
			break;
		case 2:
			omap_revision = OMAP3630_REV_ES1_2;
			rev_name = "ES1.2";
			break;
		default:
			/* Use the latest known revision as default */
			omap_revision = OMAP3630_REV_ES1_2;
			rev_name = "Unknown revision\n";
			break;
		}
		break;
	default:
		/* Unknown default to latest silicon rev as default*/
		omap_revision = OMAP3630_REV_ES1_2;
		rev_name = "Unknown revision\n";
	}

out:
	pr_info("OMAP%04x %s\n", omap_rev() >> 16, rev_name);
	pr_info("DIE ID: %08X%08X%08X%08X \n", val[3], val[2], val[1], val[0]);
	pr_info("FEATURE_STATUS: %08x\n", omap_ctrl_readl(OMAP36XX_CONTROL_FEATURE_OMAP_STATUS));
}

/*
 * Try to detect the exact revision of the omap we're running on
 */
void __init omap2_check_revision(void)
{
	/*
	 * At this point we have an idea about the processor revision set
	 * earlier with omap2_set_globals_tap().
	 */
	if (cpu_is_omap24xx())
		omap24xx_check_revision();
	else if (cpu_is_omap34xx())
		omap34xx_check_revision();
	else
		pr_err("OMAP revision unknown, please fix!\n");

	/*
	 * OK, now we know the exact revision. Initialize omap_chip bits
	 * for powerdowmain and clockdomain code.
	 */
	if (cpu_is_omap243x()) {
		/* Currently only supports 2430ES2.1 and 2430-all */
		omap_chip.oc |= CHIP_IS_OMAP2430;
	} else if (cpu_is_omap242x()) {
		/* Currently only supports 2420ES2.1.1 and 2420-all */
		omap_chip.oc |= CHIP_IS_OMAP2420;
	} else if (cpu_is_omap343x()) {
		omap_chip.oc = CHIP_IS_OMAP3430;
		if (omap_rev() == OMAP3430_REV_ES1_0)
			omap_chip.oc |= CHIP_IS_OMAP3430ES1;
		else if (omap_rev() >= OMAP3430_REV_ES2_0 &&
			 omap_rev() <= OMAP3430_REV_ES2_1)
			omap_chip.oc |= CHIP_IS_OMAP3430ES2;
		else if (omap_rev() == OMAP3430_REV_ES3_0)
			omap_chip.oc |= CHIP_IS_OMAP3430ES3_0;
		else if (omap_rev() == OMAP3430_REV_ES3_1)
			omap_chip.oc |= CHIP_IS_OMAP3430ES3_1;
		else if (omap_rev() == OMAP3630_REV_ES1_0)
			omap_chip.oc |= CHIP_IS_OMAP3630ES1;
		else if (omap_rev() == OMAP3630_REV_ES1_1)
			omap_chip.oc |= CHIP_IS_OMAP3630ES1_1;
		else if (omap_rev() == OMAP3630_REV_ES1_2)
			omap_chip.oc |= CHIP_IS_OMAP3630ES1_2;
	} else {
		pr_err("Uninitialized omap_chip, please fix!\n");
	}
}

/*
 * Set up things for map_io and processor detection later on. Gets called
 * pretty much first thing from board init. For multi-omap, this gets
 * cpu_is_omapxxxx() working accurately enough for map_io. Then we'll try to
 * detect the exact revision later on in omap2_detect_revision() once map_io
 * is done.
 */
void __init omap2_set_globals_tap(struct omap_globals *omap2_globals)
{
	omap_revision = omap2_globals->class;
	tap_base = omap2_globals->tap;

	if (cpu_is_omap34xx())
		tap_prod_id = 0x0210;
	else
		tap_prod_id = 0x0208;
}

void __init omap_l2cache_enable(void)
{
	u32 l2_val;

	asm volatile("mrc p15, 0, %0, c1, c0, 1":"=r" (l2_val));
	if ((l2_val & 0x2) == 0) {
		printk(KERN_WARNING "L2 CACHE is not enabled in bootloader\n"
				"Enable the L2 Cache here\n");
#ifndef CONFIG_L2CACHE_OMAP3_DISABLE
		l2_val |= 0x2;
		asm volatile("mcr p15, 0, %0, c1, c0, 1"::"r" (l2_val));
#endif
	} else
		printk(KERN_WARNING "L2 CACHE is enabled in bootloader\n");
}

/*
  * Get OMAP chip version details from bootargs
 */
static int  omap34xx_get_omap_version(char *str)
{
	unsigned int rev_id;
	if (get_option(&str, &rev_id) == 1)	{
		switch (rev_id) {
		case 3420:
			omap_revision_id = OMAP_3420;
			omap_revision = OMAP3430_REV_ES3_1;
			omap_chip.oc |= CHIP_IS_OMAP3430ES3_1;
			rev_name = "ES3.1";
			break;
		case 3430:
			omap_revision_id = OMAP_3430;
			omap_revision = OMAP3430_REV_ES3_1;
			omap_chip.oc |= CHIP_IS_OMAP3430ES3_1;
			rev_name = "ES3.1";
			break;
		case 3440:
			omap_revision_id = OMAP_3440;
			omap_revision = OMAP3430_REV_ES3_1_1;
			omap_chip.oc |= CHIP_IS_OMAP3430ES3_1_1;
			rev_name = "ES3.1.1";
			break;
		case 3630:
			omap_revision_id = OMAP_3630;
			omap_chip.oc |= CHIP_IS_OMAP3630ES1;
			omap_revision = OMAP3630_REV_ES1_0;
			break;
		case 3630300:
			omap_revision_id = OMAP_3630_300;
			omap_chip.oc |= CHIP_IS_OMAP3630ES1;
			omap_revision = OMAP3630_REV_ES1_0;
			break;
		case 3630600:
			omap_revision_id = OMAP_3630_600;
			omap_chip.oc |= CHIP_IS_OMAP3630ES1;
			omap_revision = OMAP3630_REV_ES1_0;
			break;
		case 3630800:
			omap_revision_id = OMAP_3630_800;
			omap_chip.oc |= CHIP_IS_OMAP3630ES1;
			omap_revision = OMAP3630_REV_ES1_0;
			break;
		case 36301000:
			omap_revision_id = OMAP_3630_1000;
			omap_chip.oc |= CHIP_IS_OMAP3630ES1;
			omap_revision = OMAP3630_REV_ES1_0;
			break;
		case 36301200:
			omap_revision_id = OMAP_3630_1200;
			omap_chip.oc |= CHIP_IS_OMAP3630ES1;
			omap_revision = OMAP3630_REV_ES1_0;
			break;
		default:
			pr_err("OMAP revision unknown, please fix!\n");
			return 1;
		}
		pr_info("OMAP%04x %s\n", omap_rev() >> 16, rev_name);
		pr_info("OMAP Version is OMAP%04d\n", rev_id);
	}

	return 1;
}

__setup("omap_version=", omap34xx_get_omap_version);

