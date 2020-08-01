#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "amdmmio.h"

#define mmMM_INDEX				0x0
#define mmMM_DATA				0x1
#define mmPCIE_INDEX2			0x0E
#define mmPCIE_DATA2			0x0F
#define mmRSMU_INDEX			0x14
#define mmRSMU_DATA				0x15

#define mmTHM_TMON0_RDIL0_DATA	0x1660D		// 0x59834
#define mmTHM_TMON0_RDIL1_DATA	0x1660E		// 0x59838
#define mmTHM_TMON0_RDIL2_DATA	0x1660F		// 0x5983C
#define mmTHM_TMON0_RDIL3_DATA	0x16610		// 0x59840
#define mmTHM_TMON0_RDIL4_DATA	0x16611		// 0x59844
#define mmTHM_TMON0_RDIL5_DATA	0x16612		// 0x59848
#define mmTHM_TMON0_RDIL6_DATA	0x16613		// 0x5984C
#define mmTHM_TMON0_RDIL7_DATA	0x16614		// 0x59850
#define mmTHM_TMON0_RDIL8_DATA	0x16615		// 0x59854
#define mmTHM_TMON0_RDIL9_DATA	0x16616		// 0x59858
#define mmTHM_TMON0_RDIL10_DATA	0x16617		// 0x5985C
#define mmTHM_TMON0_RDIL11_DATA	0x16618		// 0x59860
#define mmTHM_TMON0_RDIL12_DATA	0x16619		// 0x59864
#define mmTHM_TMON0_RDIL13_DATA	0x1661A		// 0x59868
#define mmTHM_TMON0_RDIL14_DATA	0x1661B		// 0x5986C
#define mmTHM_TMON0_RDIL15_DATA	0x1661C		// 0x59870

#define mmTHM_TMON0_RDIR0_DATA	0x1661D		// 0x59874
#define mmTHM_TMON0_RDIR1_DATA	0x1661E		// 0x59878
#define mmTHM_TMON0_RDIR2_DATA	0x1661F		// 0x5987C
#define mmTHM_TMON0_RDIR3_DATA	0x16620		// 0x59880
#define mmTHM_TMON0_RDIR4_DATA	0x16621		// 0x59884
#define mmTHM_TMON0_RDIR5_DATA	0x16622		// 0x59888
#define mmTHM_TMON0_RDIR6_DATA	0x16623		// 0x5988C
#define mmTHM_TMON0_RDIR7_DATA	0x16624		// 0x59890
#define mmTHM_TMON0_RDIR8_DATA	0x16625		// 0x59894
#define mmTHM_TMON0_RDIR9_DATA	0x16626		// 0x59898
#define mmTHM_TMON0_RDIR10_DATA	0x16627		// 0x5989C
#define mmTHM_TMON0_RDIR11_DATA	0x16628		// 0x598A0
#define mmTHM_TMON0_RDIR12_DATA	0x16629		// 0x598A4
#define mmTHM_TMON0_RDIR13_DATA	0x1662A		// 0x598A8
#define mmTHM_TMON0_RDIR14_DATA	0x1662B		// 0x598AC
#define mmTHM_TMON0_RDIR15_DATA	0x1662C		// 0x598B0

typedef struct _THM_TMON_DATA
{
	uint32_t Z : 11;
	uint32_t VALID : 1;
	uint32_t TEMP : 11;
	uint32_t Reserved : 9;
} THM_TMON_DATA;

typedef union _THM_REG
{
	uint32_t raw;
	THM_TMON_DATA asTMON;
} THM_REG;

#define AMD_TMON_CONSTANT			((int32_t)392)

float AMDTMONToDegC(const THM_TMON_DATA *TmpVal)
{
	return((TmpVal->TEMP - AMD_TMON_CONSTANT) /  8.0f);
}

int main(int argc, char **argv)
{
	AMDGPU GPU;
	int32_t ret;
	uint32_t GPUIdx;
	
	if(argc != 2)
	{
		printf("amdtempthing version %s by Wolf9466 (AKA Wolf0/OhGodAPet)\n", "0.1a");
		printf("Usage: %s <Card index>\n", argv[0]);
		return(1);
	}
	
	if(argv[1][0] == '0' && argv[1][1] == 'x') GPUIdx = strtoul(argv[1] + 2, NULL, 16);
	else GPUIdx = strtoul(argv[1], NULL, 10);
	
	ret = InitGPUByIndex(&GPU, AMD_MMIO_ACCESS_TYPE_DEBUGFS, GPUIdx);
	
	if(ret != AMD_MMIO_ERR_SUCCESS)
	{
		printf("InitGPUByIndex() returned %d!\n", ret);
		FreeGPU(&GPU);
		return(ret);
	}
	
	for(int i = 0; i < 16; ++i)
	{
		THM_REG TempData;
		
		ret = ReadMMIOReg(&GPU, mmTHM_TMON0_RDIL0_DATA + i, &TempData.raw);
		
		if(ret != AMD_MMIO_ERR_SUCCESS)
		{
			printf("ReadMMIOReg() returned %d!\n", ret);
			FreeGPU(&GPU);
			return(ret);
		}
		
		if(TempData.asTMON.VALID) printf("THM_TMON0_RDIL%d == %.3fC\n", i, AMDTMONToDegC(&TempData.asTMON));
		else printf("THM_TMON0_RDIL%d not valid.\n", i);
	
		ret = ReadMMIOReg(&GPU, mmTHM_TMON0_RDIR0_DATA + i, &TempData.raw);
		
		if(ret != AMD_MMIO_ERR_SUCCESS)
		{
			printf("ReadMMIOReg() returned %d!\n", ret);
			FreeGPU(&GPU);
			return(ret);
		}
		
		if(TempData.asTMON.VALID) printf("THM_TMON0_RDIR%d == %.3fC\n", i, AMDTMONToDegC(&TempData.asTMON));
		else printf("THM_TMON0_RDIR%d not valid.\n", i);
		
	}
	
	FreeGPU(&GPU);
	
	return(0);
}
	
