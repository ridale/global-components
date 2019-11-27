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

#include <autoconf.h>

#include <camkes.h>
#include <camkes/dma.h>
#include <camkes/io.h>
#include <camkes/irq.h>
#include <camkes/sync.h>

#include <usb/usb.h>

#include <platsupport/io.h>
#include <platsupport/irq.h>
#include <vka/vka.h>
#include <simple/simple.h>
#include <simple/simple_helpers.h>
#include <allocman/allocman.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>
#include <sel4utils/vspace.h>
#include <sel4utils/sel4_zf_logif.h>

#include "plat.h"
static usb_t host = {0};
static ps_mutex_ops_t msync = {0};
static ps_io_ops_t io_ops = {0};

#define MAX_USB_HOST_IRQS  10

static ps_irq_t irq_info[MAX_USB_HOST_IRQS];
static ps_irq_ops_t irq_ops;
static int nirqs = 0;


void pre_init(void)
{
    ZF_LOGD("pre_init");
}

static void usb_irq_handle(void *data, ps_irq_acknowledge_fn_t acknowledge_fn, void *ack_data)
{
    int error = 0;
    error = usbdriver_lock();
    ZF_LOGF_IF(error, "Failed to obtain lock for Usbdriver");

    ps_irq_t *irq = data;

    if (0 == error && NULL != irq) {
        usb_handle_irq(&host);
    }

    error = acknowledge_fn(irq);
    ZF_LOGF_IF(error, "Failed to acknowledge IRQ");

    error = usbdriver_unlock();
    ZF_LOGF_IF(error, "Failed to release lock for Usbdriver");
}

void post_init(void)
{
    int error = 0;
    error = usbdriver_lock();
    ZF_LOGF_IF(error, "Failed to obtain lock for Usbdriver");

    host.hdev.id = USB_HOST_DEFAULT;

    error = init_camkes_mutex_ops(&msync);
    ZF_LOGF_IF(error, "Failed to initialise MUTEX ops");

    error = camkes_io_ops(&io_ops);
    ZF_LOGF_IF(error, "Failed to initialise IO ops");

    error = usb_postinit(host.hdev.id, &io_ops);
    ZF_LOGF_IF(error, "Failed to initialise IO mux");

    error = camkes_irq_ops(&irq_ops);
    ZF_LOGF_IF(error, "Failed to initialise IRQ ops");

    const int* irqs = usb_host_irqs(&host.hdev, &nirqs);
    ZF_LOGF_IF((NULL == irqs), "Failed to initialise IRQ ops");
    ZF_LOGF_IF((0 == nirqs), "No IRQs mapped");

    for (int i=0; i < nirqs && i < MAX_USB_HOST_IRQS; i++) {
        irq_info[i]  = (ps_irq_t) {
            .type = PS_INTERRUPT, 
            .irq = { .number = irqs[i] }
        };
        irq_id_t irq_id = ps_irq_register(&irq_ops, irq_info[i], usb_irq_handle, &irq_info[i]);
        if (irq_id < 0) {
            ZF_LOGF("Failed to initialise USB interrupt");
            break;
        } else {
            ZF_LOGW("registered irq %d", irq_info[i].irq.number);
        }
    }
    error = usb_init(USB_HOST_DEFAULT, &io_ops, &msync, &host);
    ZF_LOGF_IF(error, "Failed to initialise USB host");

    error = usbdriver_unlock();
    ZF_LOGF_IF(error, "Failed to release lock for Usbdriver");
}

