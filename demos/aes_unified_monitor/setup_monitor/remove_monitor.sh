# Remove Monitor driver
echo "Removing Monitor proxy driver..."
modprobe -r mmonitor

# Remove Monitor device tree overlay
echo "Removing Monitor device tree overlay..."
rmdir /sys/kernel/config/device-tree/overlays/monitor