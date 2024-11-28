#
# 1. Copy required files to the board
#

- HW: design_1_wrapper.bit
- SW: dummy
- Firmware: setup_monitor/

#
# 2. Load bitstream using configfs
#

# Copy the bitstream to /lib/firmware/
cp design_1_wrapper.bit /lib/firmware

# Load the full bitstream (note this step won't work without previously copying the file to /lib/firmware/)
echo design_1_wrapper.bit > /sys/class/fpga_manager/fpga0/firmware

# Check the reconfiguration state (must be Operating)
cat /sys/class/fpga_manager/fpga0/state

#
# 2. Load dt overlay (it includes the monitor on the @amba node of the dt)
#

# Copy the overlay to /lib/firmware/overlays
mkdir /sys/kernel/config/device-tree/overlays/monitor
cp setup_monitor/overlays/monitor.dtbo /lib/firmware/overlays

# Load the overlay (requires previous copy)
echo overlays/monitor.dtbo > /sys/kernel/config/device-tree/overlays/monitor/path

#
# 3. Run app
#

./dummy

# The output must show the elapsed time (~100ms) and the value of the signals b0101.
# There is a visualization tool to se the output as a waveform but since the signals are static there isn't much to see
