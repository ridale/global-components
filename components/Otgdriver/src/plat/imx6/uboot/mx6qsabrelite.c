// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 * Copyright (C) 2013, Boundary Devices <info@boundarydevices.com>
 */
//
// Mostly pulled from u-boot files
//
#include "io.h"

#include "gpio.h"
#include "mx6x_pins.h"
#include "mx6qsabrelite.h"
#include <utils/util.h>
#include <platsupport/io.h>
#include <platsupport/delay.h>

#ifdef CONFIG_PLAT_IMX6
#define IOMUXC_PADDR 0x020E0000
#define IOMUXC_SIZE      0x4000
#endif
#ifdef CONFIG_PLAT_IMX8MQ_EVK
#define IOMUXC_PADDR 0x30330000
#define IOMUXC_SIZE      0x10000
#endif



#ifndef RESOURCE
#define RESOURCE(mapper, id) ps_io_map(mapper,  (uintptr_t) id##_PADDR, id##_SIZE, 0, PS_MEM_NORMAL)
#define UNRESOURCE(mapper, id, addr) ps_io_unmap(mapper, addr, id##_SIZE)
#endif


#define WEAK_PULLUP	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_SRE_SLOW)

#define WEAK_PULLDN	(PAD_CTL_PUS_100K_DOWN |		\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_SLOW)

static iomux_v3_cfg_t const usb_pads[] = {
	MX6Q_PAD_GPIO_17__GPIO_7_12         | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6Q_PAD_EIM_D30__USBOH3_USBH1_OC   | MUX_PAD_CTRL(WEAK_PULLUP),
};

static iomux_v3_cfg_t const misc_pads[] = {
	MX6Q_PAD_GPIO_1__USB_OTG_ID		    | MUX_PAD_CTRL(WEAK_PULLUP),
	MX6Q_PAD_KEY_COL4__USBOH3_USBOTG_OC | MUX_PAD_CTRL(WEAK_PULLUP),
	/* OTG Power enable */
	MX6Q_PAD_EIM_D22__GPIO_3_22		    | MUX_PAD_CTRL(WEAK_PULLDN),
};

/*
 * configures a single pad in the iomuxer
 */
static int imx_iomux_v3_setup_pad(void *base, iomux_v3_cfg_t pad)
{
    uint32_t mux_ctrl_ofs = (pad & MUX_CTRL_OFS_MASK) >> MUX_CTRL_OFS_SHIFT;
    uint32_t mux_mode = (pad & MUX_MODE_MASK) >> MUX_MODE_SHIFT;
    uint32_t sel_input_ofs = (pad & MUX_SEL_INPUT_OFS_MASK) >> MUX_SEL_INPUT_OFS_SHIFT;
    uint32_t sel_input = (pad & MUX_SEL_INPUT_MASK) >> MUX_SEL_INPUT_SHIFT;
    uint32_t pad_ctrl_ofs = (pad & MUX_PAD_CTRL_OFS_MASK) >> MUX_PAD_CTRL_OFS_SHIFT;
    uint32_t pad_ctrl = (pad & MUX_PAD_CTRL_MASK) >> MUX_PAD_CTRL_SHIFT;

    if (mux_ctrl_ofs) {
        __raw_writel(mux_mode, base + mux_ctrl_ofs);
    }

    if (sel_input_ofs) {
        __raw_writel(sel_input, base + sel_input_ofs);
    }

    if (!(pad_ctrl & NO_PAD_CTRL) && pad_ctrl_ofs) {
        __raw_writel(pad_ctrl, base + pad_ctrl_ofs);
    }

    return 0;
}

static int imx_iomux_v3_setup_multiple_pads(void *base, iomux_v3_cfg_t const *pad_list,
                                            unsigned count)
{
    iomux_v3_cfg_t const *p = pad_list;
    int i;
    int ret;

    for (i = 0; i < count; i++) {
        ret = imx_iomux_v3_setup_pad(base, *p);
        if (ret) {
            return ret;
        }
        p++;
    }
    return 0;
}
/**
 * Switches downstream power on for the USB-OTG port (micro-usb)
 */
int board_ehci_power(ps_io_ops_t *io_ops, int port, int on)
{
	if (port)
		return 0;

	gpio_direction_output(IMX_GPIO_NR(3, 22), 0, io_ops);
	ps_mdelay(2);
	gpio_set_value(IMX_GPIO_NR(3, 22), 1);

	return 0;
}
/**
 * Does the generic IOMUX stuff for USB
 */
int setup_iomux_usb(const int id, ps_io_ops_t *io_ops)
{
    int ret;
    void *base;
    int unmapOnExit = 0;

    if (mux_sys_valid(&io_ops->mux_sys)) {
        base = mux_sys_get_vaddr(&io_ops->mux_sys);
    } else {
        base = RESOURCE(&io_ops->io_mapper, IOMUXC);
        unmapOnExit = 1;
    }
    if (!base) {
        return 1;
    }

	ret = imx_iomux_v3_setup_multiple_pads(base, usb_pads, ARRAY_SIZE(usb_pads));
    if (ret) {
        return ret; // fail
    }
	ret = imx_iomux_v3_setup_multiple_pads(base, misc_pads, ARRAY_SIZE(misc_pads));
    if (ret) {
        return ret; // fail
    }
	/* Reset USB hub */
	gpio_direction_output(IMX_GPIO_NR(7, 12), 0, io_ops);
	ps_mdelay(2);
	gpio_set_value(IMX_GPIO_NR(7, 12), 1);

    // if we are starting otg power on
    board_ehci_power(io_ops, id, 1);

    if (unmapOnExit) {
        UNRESOURCE(&io_ops->io_mapper, IOMUXC, base);
    }

	return 0;
}

