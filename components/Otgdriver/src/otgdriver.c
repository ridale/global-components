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

#define ZF_LOG_LEVEL ZF_LOG_DEBUG

#include <autoconf.h>

#include <camkes.h>
#include <camkes/dma.h>
#include <camkes/io.h>
#include <camkes/irq.h>
#include <camkes/sync.h>

#include <usb/otg.h>
#include <usb/drivers/otgusbtty.h>

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
static usb_otg_t otg = NULL;
static ps_mutex_ops_t msync = {0};
static ps_io_ops_t io_ops = {0};

#define MAX_USB_OTG_IRQS  10

static ps_irq_t irq_info[MAX_USB_OTG_IRQS];
static ps_irq_ops_t irq_ops;
static int nirqs = 0;


void pre_init(void)
{
    ZF_LOGD("pre_init");
}

static void otg_irq_handle(void *data, ps_irq_acknowledge_fn_t acknowledge_fn, void *ack_data)
{
    int error = 0;
    error = otgdriver_lock();
    ZF_LOGF_IF(error, "Failed to obtain lock for Otgdriver");

    ps_irq_t *irq = data;

    if (0 == error && NULL != irq) {
        otg_handle_irq(otg);
    }

    error = acknowledge_fn(irq);
    ZF_LOGF_IF(error, "Failed to acknowledge IRQ");

    error = otgdriver_lock();
    ZF_LOGF_IF(error, "Failed to release lock for Otgdriver");
}

static  otg_usbtty_t usbtty = NULL;

void post_init(void)
{
    ZF_LOGD("post_init");
    int error = 0;
    error = otgdriver_lock();
    ZF_LOGF_IF(error, "Failed to obtain lock for Otgdriver");

    error = init_camkes_mutex_ops(&msync);
    ZF_LOGF_IF(error, "Failed to initialise MUTEX ops");

    error = camkes_io_ops(&io_ops);
    ZF_LOGF_IF(error, "Failed to initialise IO ops");

    error = otg_postinit(USB_OTG_DEFAULT, &io_ops); // iomux setup
    ZF_LOGF_IF(error, "Failed to initialise IO mux");

    error = camkes_irq_ops(&irq_ops);
    ZF_LOGF_IF(error, "Failed to initialise IRQ ops");

    const int* irqs = usb_otg_irqs(USB_OTG_DEFAULT, &nirqs);
    ZF_LOGF_IF((NULL == irqs), "Failed to initialise IRQ ops");
    ZF_LOGF_IF((0 == nirqs), "No IRQs mapped");

    for (int i=0; i < nirqs && i < MAX_USB_OTG_IRQS; i++) {
        irq_info[i]  = (ps_irq_t) {
            .type = PS_INTERRUPT, 
            .irq = { .number = irqs[i] }
        };
        irq_id_t irq_id = ps_irq_register(&irq_ops, irq_info[i], otg_irq_handle, &irq_info[i]);
        if (irq_id < 0) {
            ZF_LOGF("Failed to initialise OTG interrupt");
            break;
        } else {
            ZF_LOGW("registered irq %d", irq_info[i].irq.number);
        }
    }

    error = usb_otg_init(USB_OTG_DEFAULT, &otg, io_ops);
    ZF_LOGF_IF(error, "Failed to initialise USB host");

    error = otg_usbtty_init(otg, &io_ops.dma_manager, &usbtty);
    ZF_LOGF_IF(error, "Failed to initialise tty ep");


    error = otgdriver_unlock();
    ZF_LOGF_IF(error, "Failed to release lock for Otgdriver");
}
