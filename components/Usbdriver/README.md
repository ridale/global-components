# USB driver work

This exercises the libusb code for imx6.

## Current USB issues

We need a way of attaching drivers to the endpoints.

We need something like the ethdriver.idl4 or some seL4RPCDataport connectors to do something like the rx/tx for eth.

RX/TX bound to a driver class? or something like that.