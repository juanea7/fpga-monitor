The application must load the device tree overlay and the monitor driver after the app bitstream is loaded.
This could be done by executing the setup_monitor.sh file with system().

 * The setup_monitor folder must be placed inside the application folder at its executable level*
 
 Remember to:
 1.- Place the compiled monitor kernel module (mmonitor.k) at setup/modules.
 2.- Place the monitor device tree overlay (monitor.dtbl) at setup/overlays.

