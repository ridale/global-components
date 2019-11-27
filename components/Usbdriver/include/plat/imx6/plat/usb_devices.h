/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */
#pragma once

#define HARDWARE_USB_COMPONENT

#define HARDWARE_USB_INTERFACES     \
    consumes Dummy UsbDriver;       \
    consumes Dummy UsbMisc;         \
    consumes Dummy uiomux;           \
    consumes Dummy UsbPhy1;         \
    consumes Dummy uccm;            \
    consumes Dummy uanalog;         \
    consumes Dummy ugpio3;           \
    consumes Dummy ugpio6;           \
    emits Dummy dummy_source;

#define HARDWARE_USB_COMPOSITION                                               \
    connection seL4DTBHardware usb_conn(from dummy_source, to UsbDriver);      \
    connection seL4DTBHardware usbmisc_conn(from dummy_source, to UsbMisc);    \
    connection seL4DTBHardware uiomux_conn(from dummy_source, to uiomux);        \
    connection seL4DTBHardware uccm_conn(from dummy_source, to uccm);          \
    connection seL4DTBHardware phy1_conn(from dummy_source, to UsbPhy1);       \
    connection seL4DTBHardware uanalog_conn(from dummy_source, to uanalog);    \
    connection seL4DTBHardware ugpio3_conn(from dummy_source, to ugpio3);        \
    connection seL4DTBHardware ugpio6_conn(from dummy_source, to ugpio6);

#define HARDWARE_USB_CONFIG                                                     \
    UsbDriver.dtb  = dtb({ "path" : "/soc/aips-bus@2100000/usb@2184200" });     \
    UsbMisc.dtb    = dtb({ "path" : "/soc/aips-bus@2100000/usbmisc@2184800" }); \
    uiomux.dtb      = dtb({ "path" : "/soc/aips-bus@2000000/iomuxc@20e0000" });  \
    UsbPhy1.dtb    = dtb({ "path" : "/soc/aips-bus@2000000/usbphy@20ca000" });  \
    uccm.dtb       = dtb({ "path" : "/soc/aips-bus@2000000/ccm@20c4000" });     \
    uanalog.dtb    = dtb({ "path" : "/soc/aips-bus@2000000/anatop@20c8000" });  \
    ugpio3.dtb      = dtb({ "path" : "/soc/aips-bus@2000000/gpio@20a4000" });    \
    ugpio6.dtb      = dtb({ "path" : "/soc/aips-bus@2000000/gpio@20b4000" });    \
    UsbDriver.generate_interrupts = 1;

