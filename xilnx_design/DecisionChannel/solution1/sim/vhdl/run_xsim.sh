
xelab xil_defaultlib.apatb_Decision_top -prj Decision.prj --initfile "/opt/Xilinx/Vivado/2016.2/bin/../data/xsim/ip/xsim_ip.ini" --lib "ieee_proposed=./ieee_proposed" -s Decision 
xsim --noieeewarnings Decision -tclbatch Decision.tcl

