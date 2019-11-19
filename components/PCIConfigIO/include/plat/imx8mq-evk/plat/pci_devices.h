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

#define HARDWARE_PCI_COMPONENT

#define HARDWARE_PCI_INTERFACES                                                \
    consumes Dummy PCIDriver;                                                  \
    emits Dummy dummy_source;

#define HARDWARE_PCI_COMPOSITION                                               \
    connection seL4DTBHardware pci_conn(from dummy_source, to PCIDriver);      

#define HARDWARE_PCI_CONFIG                                   \
    PCIDriver.dtb = dtb({ "path" : "/pcie@0x33800000" });     \
    PCIDriver.generate_interrupts = 1;

