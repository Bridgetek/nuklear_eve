
#include "ESD_Utility.h"

#include "ESD_Base.h"
#include "ESD_Scissor.h"
#include "ESD_GpuAlloc.h"
#include "ESD_TouchTag.h"
#include "ESD_Context.h"

extern ESD_CORE_EXPORT EVE_HalContext *ESD_Host;
extern ESD_CORE_EXPORT ESD_GpuAlloc *ESD_GAlloc;

#if defined(EVE_FLASH_AVAILABLE)
#ifndef NDEBUG
static uint32_t s_FlashErrorLast = ~0;
#endif
#endif

#if defined(EVE_FLASH_AVAILABLE)
#ifdef ESD_SIMULATION
extern void ESD_SetFlashStatus__ESD(int status);
extern void ESD_SetFlashSize__ESD(int size);
#else
#define ESD_SetFlashStatus__ESD(status) \
	do                                  \
	{                                   \
	} while (false)
#define ESD_SetFlashSize__ESD(status) \
	do                                \
	{                                 \
	} while (false)
#endif

ESD_CORE_EXPORT void ESD_AttachFlashFast()
{
	// Wait for flash status to move on from FLASH_STATUS_INIT
	EVE_HalContext *phost = ESD_GetHost();
	if (!EVE_Hal_supportFlash(phost))
		return;

	uint32_t flashStatus;
	while (!(flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS)))
	{
#ifndef NDEBUG
		eve_printf_debug("Waiting for REG_FLASH_STATUS (%u)\n", flashStatus);
#endif
		EVE_sleep(100);
	}

	// No need to continue if flash is okay
	if (flashStatus < FLASH_STATUS_FULL)
	{
		uint32_t error;

		ESD_SetFlashStatus__ESD(flashStatus);

		flashStatus = EVE_CoCmd_flashAttach(phost);
		ESD_SetFlashStatus__ESD(flashStatus);
		ESD_SetFlashSize__ESD(EVE_Hal_rd32(phost, REG_FLASH_SIZE));

		flashStatus = EVE_CoCmd_flashFast(phost, &error);
		ESD_SetFlashStatus__ESD(flashStatus);

#ifndef NDEBUG
		if (error != s_FlashErrorLast)
		{
			s_FlashErrorLast = error;
			switch (error)
			{
			case 0:
				eve_printf_debug("Flash OK\n");
				break;
			case FLASH_ERROR_NOTATTACHED: // 0xE001 :
				eve_printf_debug("Flash is not attached\n");
				break;
			case FLASH_ERROR_HEADERMISSING: // 0xE002:
				eve_printf_debug("No header detected in sector 0. Is flash blank?\n");
				break;
			case FLASH_ERROR_HEADERCRCFAILED: // 0xE003:
				eve_printf_debug("Sector 0 flash data failed integrity check\n");
				break;
			case FLASH_ERROR_FIRMWAREMISMATCH: // 0xE004:
				eve_printf_debug("Flash device/blob mismatch. Was correct blob loaded?\n");
				break;
			case FLASH_ERROR_FULLSPEEDFAILED: // 0xE005:
				eve_printf_debug("Failed full-speed flash test. Check board wiring\n");
				break;
			default:
				eve_printf_debug("Unknown flash error (%u)\n", error);
			}
		}
#endif
	}
}
#else
#define ESD_AttachFlashFast() eve_noop()
#endif

ESD_CORE_EXPORT void ESD_BeginLogo()
{
	EVE_HalContext *phost = ESD_GetHost();
	ESD_GpuAlloc_Reset(ESD_GAlloc);
	ESD_GpuAlloc_Alloc(ESD_GAlloc, RAM_G_SIZE, 0); // Block allocation
	EVE_CoCmd_dlStart(phost);
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_CoCmd_dl(phost, CLEAR(1, 0, 0));
	EVE_CoCmd_dl(phost, DISPLAY());
	EVE_CoCmd_swap(phost);
	EVE_CoCmd_dlStart(phost);
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_CoCmd_dl(phost, CLEAR(1, 0, 0));
	EVE_CoCmd_dl(phost, DISPLAY());
	EVE_CoCmd_swap(phost);
	EVE_CoCmd_dlStart(phost);
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_CoCmd_dl(phost, CLEAR(1, 0, 0));
	EVE_CoCmd_dl(phost, DISPLAY());
	// EVE_CoCmd_memSet(ESD_Host, 0, 0xFF, RAM_G_SIZE);
	EVE_Cmd_waitFlush(phost);
	EVE_CoCmd_logo(phost);
	EVE_Cmd_waitLogo(phost);
	EVE_sleep(3000);
}

ESD_CORE_EXPORT void ESD_EndLogo()
{
	EVE_HalContext *phost = ESD_GetHost();
	EVE_CoCmd_dlStart(phost);
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(255, 255, 255));
	EVE_CoCmd_dl(phost, CLEAR(1, 0, 0));
	EVE_CoCmd_dl(phost, DISPLAY());
	EVE_CoCmd_swap(phost);
	EVE_Cmd_waitFlush(phost);
	ESD_GpuAlloc_Reset(ESD_GAlloc);
}

ESD_CORE_EXPORT void ESD_ShowLogo()
{
	ESD_CurrentContext->ShowLogo = true;
}

/// Run calibrate procedure
ESD_CORE_EXPORT bool ESD_Calibrate()
{
	EVE_HalContext *phost = ESD_GetHost();
	uint32_t result;
	uint32_t transMatrix[6];

#if defined(EVE_SCREEN_CAPACITIVE)
	EVE_Hal_wr8(phost, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif

	eve_printf_debug("ESD_Calibrate: Start Frame\n");

	EVE_CoCmd_dlStart(phost);
	EVE_CoDl_clearColorRgb(phost, 64, 64, 64);
	EVE_CoDl_clear(phost, true, true, true);
	EVE_CoDl_colorRgb(phost, 0xff, 0xff, 0xff);

	// EVE_CoCmd_text(phost, (ESD_Host->Parameters.Display.Width / 2), (ESD_Host->Parameters.Display.Height / 2), 27, OPT_CENTER, "Please Tap on the dot");

	result = EVE_CoCmd_calibrate(phost);

	eve_printf_debug("ESD_Calibrate: End Frame\n");

	// Print the configured values
	EVE_Hal_rdMem(phost, (uint8_t *)transMatrix, REG_TOUCH_TRANSFORM_A, 4 * 6); //read all the 6 coefficients
	eve_printf_debug("Touch screen transform values are A 0x%lx,B 0x%lx,C 0x%lx,D 0x%lx,E 0x%lx, F 0x%lx\n",
	    transMatrix[0], transMatrix[1], transMatrix[2], transMatrix[3], transMatrix[4], transMatrix[5]);

	return result != 0;
}

/* end of file */
