mkdir -p /lib/firmware/overlays
mv overlays/monitor.dtbo /lib/firmware/overlays
mkdir -p /lib/modules/$(uname -r)
mv modules/mmonitor.ko /lib/modules/$(uname -r)
touch /lib/modules/$(uname -r)/modules.order
touch /lib/modules/$(uname -r)/modules.builtin
depmod
# rm -f monitor_init.sh
