
EXTRA_CFLAGS += -DLINUX \
                -DBCMDRIVER \
                -DBCMDONGLEHOST \
                -DDHDTHREAD \
                -DBCMWPA2 \
                -DBCMWAPI_WPI \
                -DUNRELEASEDCHIP \
                -DDHD_GPL \
                -DDHD_SCHED \
                -DDHD_DEBUG \
                -DSDTEST \
                -DBDC \
                -DTOE \
                -DEMBEDDED_PLATFORM \
                -DDHD_BCMEVENTS \
                -DSHOW_EVENTS \
                -DSDIO_ISR_THREAD \
                -DBCMSDIO \
                -DDHD_GPL \
                -DBCMLXSDMMC \
                -DBCMPLATFORM_BUS \
                -DCC_HAVE_ASM_GOTO \
                -DDOWNLOAD_FIRMWARE_CHECK \
                -DCUSTOMER_HW \
                -DEJECT_SAFE \
                -Idrivers/net/wireless/arcadyan/include

obj-$(CONFIG_ARCADYAN_BCM4319) += dhd.o
dhd-objs := aiutils.o bcmsdh.o \
            bcmsdh_linux.o bcmsdh_sdmmc.o \
            bcmsdh_sdmmc_linux.o bcmutils.o \
            bcmwifi.o dhd_cdc.o \
            dhd_common.o dhd_custom_gpio.o \
            dhd_linux.o dhd_linux_sched.o \
            dhd_sdio.o hndpmu.o \
            linux_osl.o sbutils.o \
            siutils.o wl_iw.o

