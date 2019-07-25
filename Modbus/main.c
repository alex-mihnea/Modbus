#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

typedef struct
{
	uint8_t m_ucAddress;
	uint8_t m_ucFunctionCode;
	uint8_t m_aucData[255];
}MODBUS_ADU_T;

typedef struct
{
	uint8_t m_ucAddress;
	uint8_t m_ucFunctionCode;
	uint16_t m_unStartingAddress;
	uint16_t m_unNumberOfCoils;
}MODBUS_COILS_REQUEST_T;

typedef struct
{
	uint8_t m_ucAddress;
	uint8_t m_ucFunctionCode;
	uint8_t m_ucByteCount;
	uint8_t m_ucCoilStatus[255]; 
}MODBUS_COILS_RESPONSE_T;

typedef struct
{
	uint8_t m_ucExceptionFunctionCode;
	uint8_t m_ucExceptionCode;
}EXCEPTION_RESPONSE_T;


uint8_t g_aucCoils[3000] = { 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1,1,1,1,1,1 };
MODBUS_COILS_REQUEST_T g_stRequest;
MODBUS_COILS_RESPONSE_T g_stResponse;
uint8_t g_ucAddress = 0xEC;
uint8_t g_ucExceptionCode;
uint8_t g_aucADU[255];

void Modbus_ReadCoilsResponse(MODBUS_COILS_REQUEST_T *p_pstRequest)
{
	if (0x0001 <= p_pstRequest->m_unNumberOfCoils && p_pstRequest->m_unNumberOfCoils <= 0x07D0)
	{	
		if ( (p_pstRequest->m_unStartingAddress >= 0x0000 && p_pstRequest->m_unStartingAddress <= 0xFFFF) &&
			 (p_pstRequest->m_unStartingAddress + p_pstRequest->m_unNumberOfCoils <= 0xFFFF) )
		{
			//(m_unNumberOfCoils % 8 == 0) ? (g_stResponse.m_ucByteCount = m_unNumberOfCoils / 8) : (g_stResponse.m_ucByteCount = m_unNumberOfCoils / 8 + 1);
			if (p_pstRequest->m_unNumberOfCoils % 8 == 0)
			{
				g_stResponse.m_ucByteCount = p_pstRequest->m_unNumberOfCoils / 8;
			}
			else
			{
				g_stResponse.m_ucByteCount = p_pstRequest->m_unNumberOfCoils / 8 + 1;
			}

			g_stResponse.m_ucAddress = p_pstRequest->m_ucAddress;
			g_stResponse.m_ucFunctionCode = p_pstRequest->m_ucFunctionCode;
			uint16_t i, j;
			//printf("byte count: %d\n", g_stResponse.m_ucByteCount);
			for (i = 0; i < g_stResponse.m_ucByteCount; i++) 
			{
				for (j = 0; j < 8; j++) 
				{
					if (i * 8 + j >= p_pstRequest->m_unNumberOfCoils)
					{
						break;
					}
					g_stResponse.m_ucCoilStatus[i] += g_aucCoils[i * 8 + j] << j;
					printf("gauc coils: %x\t", g_aucCoils[i * 8 + j]);
					printf("response coils: %x\n", g_stResponse.m_ucCoilStatus[i]);
				}
			}
			// copiere in adu <- response
			memcpy(g_aucADU, &g_stResponse, (3 + g_stResponse.m_ucByteCount));
		}
		else
		{
			g_ucExceptionCode = 0x02;
			printf("out of bounds");
		}
	}
	else
	{
		g_ucExceptionCode = 0x03;
		printf("coils > 2000");
	}
}


int main(void)
{
	g_stRequest.m_ucAddress = 0xEC;
	g_stRequest.m_ucFunctionCode = 0x01;
	g_stRequest.m_unStartingAddress = 0x00;
	g_stRequest.m_unNumberOfCoils = 10;


	Modbus_ReadCoilsResponse(&g_stRequest);
	uint16_t i;
	for (i = 0; i < g_stResponse.m_ucByteCount; i++) {
		printf("%x ", g_stResponse.m_ucCoilStatus[i]);
	}

	printf("\nADU: \n");
	for(int i = 0; i < 255; i++)
	{
		printf("%02X ", g_aucADU[i]);
	}
	/*
	if (g_stRequest.m_ucAddress == g_ucAddress) 
	{
		switch(g_stRequest.m_ucFunctionCode) 
		{
		case 0x01: // read coils
			Modbus_ReadCoilsResponse(g_stRequest.m_unStartingAddress, g_stRequest.m_unNumberOfCoils);
			break;
		case 0x04: // write single coil
			
			break;
		case 0x0F: // write multiple coils
		
			break;
		default:
		}
	}
	else
	{

	}
	*/
/*
	g_aucCoils[0] = 0;
	g_aucCoils[1] = 1;
	g_aucCoils[2] = 0;
	g_aucCoils[3] = 0;
	g_aucCoils[4] = 1;
	g_aucCoils[5] = 1;
	g_aucCoils[6] = 1;
	g_aucCoils[7] = 0;
	g_aucCoils[8] = 0;
	g_aucCoils[9] = 1;
*/
	return 0;
}