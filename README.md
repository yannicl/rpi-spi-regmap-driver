# Pico SPI Regmap Driver

An open-source Linux kernel module providing a regmap-based SPI driver for the Raspberry Pi (tested with Pico as peripheral). This project enables easy register access to SPI devices using the Linux regmap API.

## Features
- Linux kernel SPI driver with regmap support
- Device Tree overlay for easy binding
- Sysfs interface for register access
- Example for Raspberry Pi Pico as SPI peripheral

## Getting Started

### Prerequisites
- Raspberry Pi with SPI enabled
- Kernel headers matching your running kernel
- Build tools: `make`, `gcc`, etc.

### Build the Driver
```sh
make
```
If successful, this produces `pico_spi.ko`.

### Compile the overlay (.dtbo)
```sh
dtc -@ -I dts -O dtb -o pico-spi.dtbo pico-spi-overlay.dts
```

### Install the overlay
```sh
sudo cp pico-spi.dtbo /boot/overlays/
echo "dtoverlay=pico-spi" | sudo tee -a /boot/config.txt
sudo reboot
```

### Enable the overlay in /boot/config.txt
sudo nano /boot/config.txt

### Load the Driver
```sh
sudo insmod pico_spi.ko
dmesg | tail
# Should see: pico-spi spi0.0: Pico SPI regmap driver loaded
```

### Verify Device Binding
```sh
ls /sys/bus/spi/devices/
# Should show: spi0.0
ls -l /sys/bus/spi/devices/spi0.0/driver
```

### Unload/Reload During Development
```sh
sudo rmmod pico_spi
sudo insmod pico_spi.ko
```

### Autoload at Boot (Optional)
```sh
sudo cp pico_spi.ko /lib/modules/$(uname -r)/kernel/drivers/spi/
sudo depmod -a
echo pico_spi | sudo tee /etc/modules-load.d/pico_spi.conf
```

## Device Tree Overlay
See `pico-spi-overlay.dts` for an example overlay to bind the driver.

## Contributing
No contribution wanted. This work is given as is.

## License
GPL v2. See LICENSE file.