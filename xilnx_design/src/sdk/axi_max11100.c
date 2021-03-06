/***************************** Include Files *******************************/

#include "axi_max11100.h"

/************************** Function Definitions ***************************/
static u16 AXI_MAX11100_CONVERT(u32 BaseAddress);

u16  get_sample(unsigned char channel)

{
	if(channel == 2) return AXI_MAX11100_CONVERT(XPAR_AXI_MAX11100_2_BASEADDR);
	else if(channel == 1) return AXI_MAX11100_CONVERT(XPAR_AXI_MAX11100_1_BASEADDR);
	else return AXI_MAX11100_CONVERT(XPAR_AXI_MAX11100_0_BASEADDR);
}

static void AXI_MAX11100_Start_Conversion(u32 BaseAddress)
/**
* \brief       Start ADC Conversion
* \par         Details
*              This function writes a 1 to the start conversion register
*              of the AXI_MAX11100 ip core to start the conversion
*
* \param[in]   BaseAddress      - Base address of the AXI_MAX11100 IP core
*
* \retval      None
*/
{
 	Xil_Out32((BaseAddress) + (AXI_MAX11100_CONV_START_REG_OFFSET), (u32)(1));
}

static void AXI_MAX11100_Stop_Conversion(u32 BaseAddress)
/**
* \brief       Stop ADC Conversion
* \par         Details
*              This function writes a 0 to the start conversion register
*              of the AXI_MAX11100 ip core to stop the conversion
*
* \param[in]   BaseAddress      - Base address of the AXI_MAX11100 IP core
*
* \retval      None
*/
{
 	Xil_Out32((BaseAddress) + (AXI_MAX11100_CONV_START_REG_OFFSET), (u32)(0));
}

static void AXI_MAX11100_Write_Clk_Div_Reg(u32 BaseAddress, u32 Value)
/**
* \brief       Write Value to the clock divider register
* \par         Details
*              This function writes a value to the clock divider register.
*              SCLK rate = axi_bus_clock/(2*(clock_divider+1))
*
* \param[in]   BaseAddress      - Base address of the AXI_MAX11100 IP core
* \param[in]   Value      - a value to be written to the clock divider register
*
* \retval      None
*/
{
 	Xil_Out32((BaseAddress) + (AXI_MAX11100_CLK_DIV_REG_OFFSET), (u32)(Value));
}

static void AXI_MAX11100_Write_Delay_Reg(u32 BaseAddress, u32 Value)
/**
* \brief       Write Value to the delay register
* \par         Details
*              This function writes a value to the delay register.
*              The delay register is used to control the sampling rate by
*              inserting delay between each SPI frames.
*              Sampling rate = 2*(SCLK rate)/(Delay_register+48)
*
* \param[in]   BaseAddress      - Base address of the AXI_MAX11100 IP core
* \param[in]   Value      - a value to be written to the delay register
*
* \retval      None
*/
{
 	Xil_Out32((BaseAddress) + (AXI_MAX11100_DELAY_REG_OFFSET), (u32)(Value));
}

static void AXI_MAX11100_Interrupt_Enable(u32 BaseAddress)
/**
* \brief       Enable the interrupt
* \par         Details
*              This function writes a 1 to the interrupt enable register
*              to enable the interrupt.
*              When the interrupt is enabled, the AXI_MAX11100 IP core will
*              raise an interrupt when the sampled data is available.
*
* \param[in]   BaseAddress      - Base address of the AXI_MAX11100 IP core
*
* \retval      None
*/
{
 	Xil_Out32((BaseAddress) + (AXI_MAX11100_INTR_ENABLE_REG_OFFSET), (u32)(1));
}

static void AXI_MAX11100_Interrupt_Disable(u32 BaseAddress)
/**
* \brief       Disable the interrupt
* \par         Details
*              This function writes a 0 to the interrupt enable register
*              to disable the interrupt
*
* \param[in]   BaseAddress      - Base address of the AXI_MAX11100 IP core
*
* \retval      None
*/
{
 	Xil_Out32((BaseAddress) + (AXI_MAX11100_INTR_ENABLE_REG_OFFSET), (u32)(0));
}

static u32 AXI_MAX11100_Read_Data(u32 BaseAddress)
/**
* \brief       Read sampled data
* \par         Details
*              This function reads the ADC sampled data register.
*              The data register will be overwritten when the new sampled data
*              is available.
*
* \param[in]   BaseAddress      - Base address of the AXI_MAX11100 IP core
*
* \retval      ADC sampled data
*/
{
 	return Xil_In32((BaseAddress) + (AXI_MAX11100_DATA_REG_OFFSET));
}

static u32 AXI_MAX11100_Read_Interrupt_Status(u32 BaseAddress)
/**
* \brief       Read interrupt status
* \par         Details
*              This function reads the interrupt status register.
*              When the value is one, sampled data is available in the
*              data register.
*              Interrupt status register is a clear-on-read register
*
* \param[in]   BaseAddress      - Base address of the AXI_MAX11100 IP core
*
* \retval      Interrupt status
*/
{
 	return Xil_In32((BaseAddress) + (AXI_MAX11100_INTR_STATUS_REG_OFFSET));
}

static u16 AXI_MAX11100_CONVERT(u32 BaseAddress)
/**
* \brief       Read one ADC sample
* \par         Details
*              This function starts an ADC conversion and returns one ADC sample.
*
* \param[in]   BaseAddress      - Base address of the AXI_MAX11100 IP core
*
* \retval      One ADC sample
*/
{
	u16 temp;
	AXI_MAX11100_Stop_Conversion(BaseAddress);
	AXI_MAX11100_Interrupt_Disable(BaseAddress);
	AXI_MAX11100_Read_Interrupt_Status(BaseAddress);
	AXI_MAX11100_Start_Conversion(BaseAddress);
	AXI_MAX11100_Write_Clk_Div_Reg(BaseAddress, 100);

	while(AXI_MAX11100_Read_Interrupt_Status(BaseAddress)==0);
	temp=AXI_MAX11100_Read_Data(BaseAddress);

	AXI_MAX11100_Stop_Conversion(BaseAddress);
	AXI_MAX11100_Read_Interrupt_Status(BaseAddress);

	return temp;
}
