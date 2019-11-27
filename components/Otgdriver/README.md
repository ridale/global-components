# USB driver work

## Current USB issues

Not sure if these should be cached, the usb driver in ```ps_dma_alloc_pinned()``` does not set the cached flag.

```c
static void *malloc_dma_alloc(void *cookie, size_t size, int align, int cached, ps_mem_flags_t flags)
{
    //assert(cached);
```

Looks like we are missing a mapping for the HW interrupt, not sure if this should be in the driver or what is going on.

```bash
UsbDriver__run@UsbDriver_seL4DTBHardware_0.c:277 No mechanism exists to handle interrupt number 72, this interrupt will be ignored
```
