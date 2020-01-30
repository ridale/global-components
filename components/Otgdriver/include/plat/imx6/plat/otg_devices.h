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

#define HARDWARE_OTG_COMPONENT

#define HARDWARE_OTG_INTERFACES     \
    consumes Dummy OtgDriver;       \
    consumes Dummy OtgMisc;         \
    consumes Dummy iomux;           \
    consumes Dummy UsbPhy;          \
    consumes Dummy uccm;            \
    consumes Dummy uanalog;         \
    consumes Dummy gpio3;           \
    consumes Dummy gpio6;           \
    emits Dummy dummy_source;

#define HARDWARE_OTG_COMPOSITION                                               \
    connection seL4DTBHardware iomux_conn(from dummy_source, to iomux);        \
    connection seL4DTBHardware otgmisc_conn(from dummy_source, to OtgMisc);    \
    connection seL4DTBHardware uccm_conn(from dummy_source, to uccm);          \
    connection seL4DTBHardware phy_conn(from dummy_source, to UsbPhy);         \
    connection seL4DTBHardware uanalog_conn(from dummy_source, to uanalog);    \
    connection seL4DTBHardware gpio3_conn(from dummy_source, to gpio3);        \
    connection seL4DTBHardware gpio6_conn(from dummy_source, to gpio6);        \
    connection seL4DTBHardware otg_conn(from dummy_source, to OtgDriver);

#define HARDWARE_OTG_CONFIG                                                    \
    OtgDriver.dtb = dtb({ "path" : "/soc/aips-bus@2100000/usb@2184000" });     \
    OtgMisc.dtb = dtb({ "path" : "/soc/aips-bus@2100000/usbmisc@2184800" });   \
    iomux.dtb = dtb({ "path" : "/soc/aips-bus@2000000/iomuxc@20e0000" });      \
    UsbPhy.dtb = dtb({ "path" : "/soc/aips-bus@2000000/usbphy@20c9000" });     \
    uccm.dtb = dtb({ "path" : "/soc/aips-bus@2000000/ccm@20c4000" });          \
    uanalog.dtb = dtb({ "path" : "/soc/aips-bus@2000000/anatop@20c8000" });    \
    gpio3.dtb = dtb({ "path" : "/soc/aips-bus@2000000/gpio@20a4000" });        \
    gpio6.dtb = dtb({ "path" : "/soc/aips-bus@2000000/gpio@20b4000" });        \
    OtgDriver.generate_interrupts = 1;

