//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
`timescale 1 ns / 100 ps

module system_tb;

//----------------------------------------------------------------------------
// Parameter (may differ for physical synthesis)
//----------------------------------------------------------------------------
parameter tck              = 20;       // clock period in ns
parameter uart_baud_rate   = 115200;  // uart baud rate for simulation 

parameter clk_freq = 1000000000 / tck; // Frequenzy in HZ
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
reg        clk;
reg        rst;
wire       led;

//----------------------------------------------------------------------------
// UART STUFF (testbench uart, simulating a comm. partner)
//----------------------------------------------------------------------------
wire         uart_rxd;
wire         uart_txd;
wire         uart_rxd1;
wire         uart_txd1;

//----------------------------------------------------------------------------
// I2C
//----------------------------------------------------------------------------

wire 			i2c_scl, i2c_sda;
   	
reg sda_out = 1'bz;
assign i2c_sda = sda_out;
//----------------------------------------------------------------------------
// Device Under Test 
//----------------------------------------------------------------------------
system #(
	.clk_freq(           clk_freq         ),
	.uart_baud_rate(     uart_baud_rate   )
) dut  (
	.clk(          clk    ),
	// Debug
	.rst(          rst    ),
	.led(          led    ),
	// UART0
	.uart_rxd(  uart_rxd  ),
	.uart_txd(  uart_txd  ),
	// UART1
	.uart_rxd1(  uart_rxd1  ),
	.uart_txd1(  uart_txd1  ),
	// I2C Wires
	.sda(i2c_sda),
	.scl(i2c_scl)
);

/* Clocking device */
initial begin
  clk <= 0;
end

always #(tck/2) clk <= ~clk;

/* Simulation setup */
initial begin



	$dumpfile("system_tb.vcd");
	//$monitor("%b,%b,%b,%b",clk,rst,uart_txd,uart_rxd);
	$dumpvars(-1, dut);
	//$dumpvars(-1,clk,rst,uart_txd);
	// reset
	#0  rst <= 0;
	#80 rst <= 1;

	#(tck*100000) $finish;
end



endmodule
