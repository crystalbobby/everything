#include "xscugic.h"
#include "axi_max11100.h"  //AXI_MAX11100 custom ip core driver
#include "utilities.h"
#include "xil_printf.h"
#include "stdio.h"

#define INTC_DEVICE_INT_ID 91	//interrupt ID for the AXI_MAX11100 IP core defined in the XPS

static void ReadADCHandler(void *CallBackRef);
int SetupInterruptSystem(XScuGic *IntcInstancePtr);

u32 g_unCount=0;
u16 *g_auSamples;	//a pointer that stores the sampled data
u32 g_unSampleSize;	//requested sample size
u8 g_uchReadADCHandlerStop=1;	//a status bit that shows whether the sampling is done

u32 start_sampling(u32 unSampleSize, int nSampleRate, u16 *auSamples)
/**
* \brief       Receive a block of samples at a constant rate
* \par         Details
*              This function is used to receive a block of samples at a constant
* \n           sampling rate.  The size of the block is defined in u16 *auSampleSize.
* \n		   And the sampling rate is defined in int nSampleRate. The Sampled data
* \n		   will be stored in an array *auSamples
*
* \param[in]   unSampleSize      - Sample size
* \param[in]   nSampleRate       - Sample rate
* \param[out]  *auSamples        - An array
*
* \retval      Number of samples received
*/
{
	u32 unClkDiv;
	u32 unDelay;

	if(nSampleRate==5)	//sample rate = 189.4ksps
	{
		unClkDiv=10;	//SCLK rate = 4.54MHz
		unDelay=0;
	}
	else
		unClkDiv=19;	//SCLK rate = 2.5MHz
	if(nSampleRate==4)  //sample rate = 100ksps
		unDelay=2;
	else if(nSampleRate==3)  //sample rate = 20ksps
		unDelay=202;
	else if(nSampleRate==2)  //sample rate = 10ksps
		unDelay=452;
	else if(nSampleRate==1)  //sample rate = 1ksps
		unDelay=4952;
	else if(nSampleRate==0)  //sample rate = 100sps
		unDelay=49952;

	g_unCount=0;
	g_auSamples=auSamples;
	g_unSampleSize=unSampleSize;

	XScuGic InterruptController;

	/*
	 * Set up interrupt handler for the AXI_MAX11100 IP Core
	 */
	SetupInterruptSystem(&InterruptController);

	/*
	 * Enable the interrupt for AXI_MAX11100 IP Core
	 */
	XScuGic_Enable(&InterruptController, INTC_DEVICE_INT_ID);

	g_uchReadADCHandlerStop=0;  //this signal goes high when the sampling is done

	/*
	 * Write to clock divider register, SCLK rate = (AXI bus clock)/(2*(unClkDiv+1))
	 */
	AXI_MAX11100_Write_Clk_Div_Reg(XPAR_AXI_MAX11100_0_BASEADDR, unClkDiv);

	/*
	 * Enable AXI_MAX11100 IP core interrupt
	 */
	AXI_MAX11100_Interrupt_Enable(XPAR_AXI_MAX11100_0_BASEADDR);

	/*
	 * Write to the delay register
	 * the sampling rate = 2*(SCLK rate)/(unDelay+48)
	 */
	AXI_MAX11100_Write_Delay_Reg(XPAR_AXI_MAX11100_0_BASEADDR, unDelay);

	/*
	 * Start the ADC conversion at the rate defined above
	 * When a sample is available, AXI_MAX11100 raises an interrupt
	 * the interrupt handler (ReadADCHandler) reads/stores the sampled data
	 *
	 * The AXI_MAX11100 stops sampling when AXI_MAX11100_Stop_Conversion() is called
	 */
	AXI_MAX11100_Start_Conversion(XPAR_AXI_MAX11100_0_BASEADDR);

	/*
	 * wait until the sampling is done
	 */
	while(g_uchReadADCHandlerStop==0);

	/*
	 * return the number of samples collected
	 */
	return g_unCount;
}

void continuous_sampling()
{
	u16 uSample[4];
	int j;
	xil_printf("taking samples\r\n");
	uSample[0]=AXI_MAX11100_CONVERT(XPAR_AXI_MAX11100_0_BASEADDR);
	uSample[1]=AXI_MAX11100_CONVERT(XPAR_AXI_MAX11100_1_BASEADDR);
	uSample[2]=AXI_MAX11100_CONVERT(XPAR_AXI_MAX11100_2_BASEADDR);
	uSample[3]=AXI_MAX11100_CONVERT(XPAR_AXI_MAX11100_3_BASEADDR);
	for(j =0; j<4;j++)
	{
		xil_printf("%i\r\n", uSample[j]);
		xil_printf("CODE: 0x%0.4X\r\n",uSample[j]);
		printf("VOLTAGE:%7.4fV\r\n",(float)4.096*uSample[j]/65536);
	}
}

int SetupInterruptSystem(XScuGic *IntcInstancePtr)
/**
* \brief       Setup the interrupt handler
* \par         Details
*              This function connects the interrupt handler to the processor
*
* \param[in]	*IntcInstancePtr	- IntcInstancePtr is the instance of the interrupt controller
*
* \retval		XST_SUCCESS to indicate success, otherwise XST_FAILURE
*/
{
	int nStatus;
	XScuGic_Config *GicConfig;

	GicConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (NULL == GicConfig) {
		return XST_FAILURE;
	}

	nStatus = XScuGic_CfgInitialize(IntcInstancePtr, GicConfig,
						GicConfig->CpuBaseAddress);
	if (nStatus != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built
	 * correctly
	 */
	nStatus = XScuGic_SelfTest(IntcInstancePtr);
	if (nStatus != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			IntcInstancePtr);

	/*
	 * Enable interrupts in the ARM
	 */
	Xil_ExceptionEnable();

	/*
	 * Connect a device driver handler that will be called when an
	 * interrupt for the device occurs, the device driver handler performs
	 * the specific interrupt processing for the device
	 */
	nStatus = XScuGic_Connect(IntcInstancePtr, INTC_DEVICE_INT_ID,
			   (Xil_ExceptionHandler)ReadADCHandler,
			   (void *)IntcInstancePtr);

	return nStatus;
}

void ReadADCHandler(void *CallBackRef)
/**
* \brief       Interrupt handler
* \par         Details
*              When the AXI_MAX11100 ip core raises an interrupt, the interrupt handler
*              clears the interrupt and reads the ADC data from the AXI_MAX11100 data register.
*              When enough samples are collected or the Esc key is pressed, the interrupt
*              handler will stop the ADC conversion.
*
* \param[in]	*CallBackRef	- CallbackRef is passed back to the device driver's interrupt
* \n			handler by the XScuGic driver.  It was given to the XScuGic
* \n			driver in the XScuGic_Connect() function call.  It is typically
* \n			a pointer to the device driver instance variable.
*
* \retval		None
*/
{
	u32 nStatus;

	/*
	 * Read the interrupt status register to clear the interrupt
	 */
	nStatus=AXI_MAX11100_Read_Interrupt_Status(XPAR_AXI_MAX11100_0_BASEADDR);

	/*
	 * Read the sampled data and store it in the *g_suSamples array
	 */
	*(g_auSamples+g_unCount)=AXI_MAX11100_Read_Data(XPAR_AXI_MAX11100_0_BASEADDR);
	g_unCount++;

	/*
	 * Stop sampling when enough samples are collected
	 */
	if(g_unCount==g_unSampleSize)
	{
		/*
		 * stops sampling
		 */
		AXI_MAX11100_Stop_Conversion(XPAR_AXI_MAX11100_0_BASEADDR);
		g_uchReadADCHandlerStop=1;
	}

	/*
	 * End sampling if the ESC key is pressed
	 */
	if((Xil_In32(XPAR_PS7_UART_1_BASEADDR+0x2C) & 0x02) == 0) //if UART Rx buffer is not empty
	{
		u8 uchInput = Xil_In32(XPAR_PS7_UART_1_BASEADDR+0x30);	// read the FIFO (only the LSB is valid).
		if(uchInput==0x1B)
		{
			AXI_MAX11100_Stop_Conversion(XPAR_AXI_MAX11100_0_BASEADDR);
			g_uchReadADCHandlerStop=1;
		}
	}
}
