# Enable kernel messages from serial port
#echo 8 > /proc/sys/kernel/printk

# Load Monitor device tree overlay
echo "Loading Monitor device tree overlay..."
mkdir /sys/kernel/config/device-tree/overlays/monitor
echo overlays/monitor.dtbo > /sys/kernel/config/device-tree/overlays/monitor/path

# Load Monitor driver
#echo "Loading Monitor proxy driver..."
#modprobe mmonitor
