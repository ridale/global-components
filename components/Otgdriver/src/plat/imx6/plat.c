#include <platsupport/io.h>
#include "../../plat.h"
#include "uboot/mx6qsabrelite.h"

int otg_postinit(const int id, ps_io_ops_t *io_ops)
{
    return setup_iomux_usb(id, io_ops); // TODO change up for OTG???
}
