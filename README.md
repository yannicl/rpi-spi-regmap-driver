6. Build the module
make


Success output:

  CC [M]  /home/pi/pico_spi_driver/pico_spi.o
  LD [M]  /home/pi/pico_spi_driver/pico_spi.ko


If you see pico_spi.ko → build succeeded.

7. Load the driver
Insert module
sudo insmod pico_spi.ko

Check kernel log
dmesg | tail


Expected:

pico-spi spi0.0: Pico SPI regmap driver loaded

8. Verify device binding
ls /sys/bus/spi/devices/


You should see:

spi0.0


Check driver:

ls -l /sys/bus/spi/devices/spi0.0/driver

9. Unload / reload during development
sudo rmmod pico_spi
sudo insmod pico_spi.ko

10. Common build errors & fixes
❌ fatal error: linux/spi/spi.h: No such file

➡ Kernel headers missing or mismatched
➡ Reinstall headers

❌ invalid module format

➡ Kernel version mismatch
➡ uname -r ≠ headers

❌ Driver loads but probe() not called
Cause	Fix
Wrong compatible	Match DT overlay
spidev bound	Remove spidev
SPI disabled	Enable SPI in config.txt

11. Autoload at boot (optional)
sudo cp pico_spi.ko /lib/modules/$(uname -r)/extra/
sudo depmod

Add:

echo pico_spi | sudo tee /etc/modules-load.d/pico_spi.conf

12. Debugging tips

Enable dynamic debug:

echo 'module pico_spi +p' | sudo tee /sys/kernel/debug/dynamic_debug/control


SPI bus visibility:

ls /sys/class/spi_master/