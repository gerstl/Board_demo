HW/SW Co-Design Demo for Xilinx Board
=====================================

This package contains a simple HW/SW co-design example for use with Xilinx 
Zynq-based ARM+FPGA prototyping boards. It includes a small hardware block 
for mapping into the programmable fabric using high-level synthesis (HLS) 
as well as a software application for interaction with the hardware block 
on top of Linux.

Contents:
---------

 hls_mac/ - HLS example for a hardware MAC unit

   Example C code (`hls_macc.c/.h`) annotated with Xilinx-specific synthesis 
   directives (pragmas), which are used to automatically infer a bus and 
   register interface when synthesized with HLS. The application code also 
   comes with a testbench (`hls_macc_test.c`).

 board_app/ - Software application to interface with the hardware MAC unit

   Application example (`example.c`) that initializes the hardware IP, feeds 
   two operands into the hardware, waits for the result, and reads/prints 
   the output. The  application includes and uses a kernel module and driver 
   (`fpga_drv.c`) to access hardware registers and install an interrupt 
   handler for hardware synchronization. 

-- 
Andreas Gerstlauer <gerstl@ece.utexas.edu>
