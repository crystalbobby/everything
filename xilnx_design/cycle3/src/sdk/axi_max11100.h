#ifndef AXI_MAX11100_H
#define AXI_MAX11100_H

/***************************** Include Files *******************************/
#include "xil_types.h"
#include "xstatus.h"
#include "xil_io.h"

/************************** Constant Definitions ***************************/


/**
 * User Logic Slave Space Offsets
 * -- CONV_START_REG : user logic slave module register 0
 * -- DELAY_REG : user logic slave module register 1
 * -- CLK_DIV_REG : user logic slave module register 2
 * -- INTR_STATUS_REG : user logic slave module register 3
 * -- INTR_ENABLE_REG : user logic slave module register 4
 * -- DATA_REG : user logic slave module register 5
 */

#define AXI_MAX11100_USER_SLV_SPACE_OFFSET (0x00000000)
#define AXI_MAX11100_CONV_START_REG_OFFSET (AXI_MAX11100_USER_SLV_SPACE_OFFSET + 0x00000000)
#define AXI_MAX11100_DELAY_REG_OFFSET (AXI_MAX11100_USER_SLV_SPACE_OFFSET + 0x00000004)
#define AXI_MAX11100_CLK_DIV_REG_OFFSET (AXI_MAX11100_USER_SLV_SPACE_OFFSET + 0x00000008)
#define AXI_MAX11100_INTR_STATUS_REG_OFFSET (AXI_MAX11100_USER_SLV_SPACE_OFFSET + 0x0000000C)
#define AXI_MAX11100_INTR_ENABLE_REG_OFFSET (AXI_MAX11100_USER_SLV_SPACE_OFFSET + 0x00000010)
#define AXI_MAX11100_DATA_REG_OFFSET (AXI_MAX11100_USER_SLV_SPACE_OFFSET + 0x00000014)

/**************************** Type Definitions *****************************/


/***************** Macros (Inline Functions) Definitions *******************/

/**
 *
 * Write a value to a AXI_MAX11100 register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the AXI_MAX11100 device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void AXI_MAX11100_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define AXI_MAX11100_mWriteReg(BaseAddress, RegOffset, Data) \
 	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a AXI_MAX11100 register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the AXI_MAX11100_ADC device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 AXI_MAX11100_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define AXI_MAX11100_mReadReg(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (RegOffset))

void AXI_MAX11100_Start_Conversion(u32 BaseAddress);

void AXI_MAX11100_Stop_Conversion(u32 BaseAddress);

void AXI_MAX11100_Write_Clk_Div_Reg(u32 BaseAddress, u32 Value);

void AXI_MAX11100_Write_Delay_Reg(u32 BaseAddress, u32 Value);

void AXI_MAX11100_Interrupt_Enable(u32 BaseAddress);

void AXI_MAX11100_Interrupt_Disable(u32 BaseAddress);

u32 AXI_MAX11100_Read_Data(u32 BaseAddress);

u32 AXI_MAX11100_Read_Interrupt_Status(u32 BaseAddress);

u16 AXI_MAX11100_CONVERT(u32 BaseAddress);

#endif /** AXI_MAX11100_H */
