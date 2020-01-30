#
# Copyright 2019, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)
#

cmake_minimum_required(VERSION 3.8.2)

project(Otgdriver C)

set(CAmkESCPP ON CACHE BOOL "" FORCE)

CAmkESAddCPPInclude("${CMAKE_CURRENT_LIST_DIR}/include/plat/${KernelPlatform}/")

set(USBDRIVER_LIBS sel4utils sel4vka sel4allocman sel4vspace sel4simple sel4platsupport usbdrivers)



DeclareCAmkESComponent(
    OtgdriverARMPlat
    SOURCES
    src/otgdriver.c
    src/plat/${KernelPlatform}/plat.c
    src/plat/${KernelPlatform}/uboot/mx6qsabrelite.c
    src/plat/${KernelPlatform}/uboot/mxc_gpio.c
    INCLUDES
    include
    LIBS
    "${USBDRIVER_LIBS}"
)
