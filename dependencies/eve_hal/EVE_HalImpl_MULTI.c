/**
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
*   http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
*/

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(EVE_MULTI_TARGET)

static size_t s_DeviceCount;
static EVE_DeviceInfo s_DeviceList[256];

/*********
** INIT **
*********/

/* Initialize HAL platform */
void EVE_HalImpl_BT8XXEMU_initialize();
void EVE_HalImpl_FT4222_initialize();
void EVE_HalImpl_MPSSE_initialize();
void EVE_HalImpl_initialize()
{
	EVE_HalImpl_BT8XXEMU_initialize();
	EVE_HalImpl_FT4222_initialize();
	EVE_HalImpl_MPSSE_initialize();
}

/* Release HAL platform */
void EVE_HalImpl_BT8XXEMU_release();
void EVE_HalImpl_FT4222_release();
void EVE_HalImpl_MPSSE_release();
void EVE_HalImpl_release()
{
	EVE_HalImpl_BT8XXEMU_release();
	EVE_HalImpl_FT4222_release();
	EVE_HalImpl_MPSSE_release();
}

void EVE_HalImpl_BT8XXEMU_list(EVE_DeviceInfo *deviceList, size_t *deviceCount);
EVE_DeviceInfo *EVE_Hal_list(size_t *deviceCount)
{
	memset(s_DeviceList, 0, sizeof(s_DeviceList));
	s_DeviceCount = 0;
	EVE_HalImpl_BT8XXEMU_list(s_DeviceList, &s_DeviceCount);
	*deviceCount = s_DeviceCount;
	return s_DeviceList;
}

/* Get the default configuration parameters */
void EVE_HalImpl_BT8XXEMU_defaults(EVE_HalParameters *parameters, EVE_DeviceInfo *device);
void EVE_HalImpl_FT4222_defaults(EVE_HalParameters *parameters, EVE_DeviceInfo *device);
void EVE_HalImpl_MPSSE_defaults(EVE_HalParameters *parameters, EVE_DeviceInfo *device);
void EVE_HalImpl_defaults(EVE_HalParameters *parameters, EVE_DeviceInfo *device)
{
	switch (device->Type)
	{
	case EVE_DEVICE_BT8XXEMU:
		EVE_HalImpl_BT8XXEMU_defaults(parameters, device);
		break;
	case EVE_DEVICE_FT4222:
		EVE_HalImpl_FT4222_defaults(parameters, device);
		break;
	case EVE_DEVICE_MPSSE:
		EVE_HalImpl_MPSSE_defaults(parameters, device);
		break;
	}
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_BT8XXEMU_open(EVE_HalContext *phost, EVE_HalParameters *parameters);
bool EVE_HalImpl_FT4222_open(EVE_HalContext *phost, EVE_HalParameters *parameters);
bool EVE_HalImpl_MPSSE_open(EVE_HalContext *phost, EVE_HalParameters *parameters);
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		return EVE_HalImpl_BT8XXEMU_open(phost, parameters);
	case EVE_DEVICE_FT4222:
		return EVE_HalImpl_FT4222_open(phost, parameters);
	case EVE_DEVICE_MPSSE:
		return EVE_HalImpl_MPSSE_open(phost, parameters);
	}
}

/* Close a HAL context */
void EVE_HalImpl_BT8XXEMU_close(EVE_HalContext *phost);
void EVE_HalImpl_FT4222_close(EVE_HalContext *phost);
void EVE_HalImpl_MPSSE_close(EVE_HalContext *phost);
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		EVE_HalImpl_BT8XXEMU_close(phost);
		break;
	case EVE_DEVICE_FT4222:
		EVE_HalImpl_FT4222_close(phost);
		break;
	case EVE_DEVICE_MPSSE:
		EVE_HalImpl_MPSSE_close(phost);
		break;
	}
}

/* Idle. Call regularly to update frequently changing internal state */
void EVE_HalImpl_BT8XXEMU_idle(EVE_HalContext *phost);
void EVE_HalImpl_FT4222_idle(EVE_HalContext *phost);
void EVE_HalImpl_MPSSE_idle(EVE_HalContext *phost);
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		EVE_HalImpl_BT8XXEMU_idle(phost);
		break;
	case EVE_DEVICE_FT4222:
		EVE_HalImpl_FT4222_idle(phost);
		break;
	case EVE_DEVICE_MPSSE:
		EVE_HalImpl_MPSSE_idle(phost);
		break;
	}
}

/*************
** TRANSFER **
*************/

void EVE_HalImpl_BT8XXEMU_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr);
void EVE_HalImpl_FT4222_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr);
void EVE_HalImpl_MPSSE_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr);
void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		EVE_HalImpl_BT8XXEMU_startTransfer(phost, rw, addr);
		break;
	case EVE_DEVICE_FT4222:
		EVE_HalImpl_FT4222_startTransfer(phost, rw, addr);
		break;
	case EVE_DEVICE_MPSSE:
		EVE_HalImpl_MPSSE_startTransfer(phost, rw, addr);
		break;
	}
}

void EVE_HalImpl_BT8XXEMU_endTransfer(EVE_HalContext *phost);
void EVE_HalImpl_FT4222_endTransfer(EVE_HalContext *phost);
void EVE_HalImpl_MPSSE_endTransfer(EVE_HalContext *phost);
void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		EVE_HalImpl_BT8XXEMU_endTransfer(phost);
		break;
	case EVE_DEVICE_FT4222:
		EVE_HalImpl_FT4222_endTransfer(phost);
		break;
	case EVE_DEVICE_MPSSE:
		EVE_HalImpl_MPSSE_endTransfer(phost);
		break;
	}
}

void EVE_HalImpl_BT8XXEMU_flush(EVE_HalContext *phost);
void EVE_HalImpl_FT4222_flush(EVE_HalContext *phost);
void EVE_HalImpl_MPSSE_flush(EVE_HalContext *phost);
void EVE_Hal_flush(EVE_HalContext *phost)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		EVE_HalImpl_BT8XXEMU_flush(phost);
		break;
	case EVE_DEVICE_FT4222:
		EVE_HalImpl_FT4222_flush(phost);
		break;
	case EVE_DEVICE_MPSSE:
		EVE_HalImpl_MPSSE_flush(phost);
		break;
	}
}

uint8_t EVE_HalImpl_BT8XXEMU_transfer8(EVE_HalContext *phost, uint8_t value);
uint8_t EVE_HalImpl_FT4222_transfer8(EVE_HalContext *phost, uint8_t value);
uint8_t EVE_HalImpl_MPSSE_transfer8(EVE_HalContext *phost, uint8_t value);
uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		return EVE_HalImpl_BT8XXEMU_transfer8(phost);
	case EVE_DEVICE_FT4222:
		return EVE_HalImpl_FT4222_transfer8(phost);
	case EVE_DEVICE_MPSSE:
		return EVE_HalImpl_MPSSE_transfer8(phost);
	}
}

uint16_t EVE_HalImpl_BT8XXEMU_transfer16(EVE_HalContext *phost, uint16_t value);
uint16_t EVE_HalImpl_FT4222_transfer16(EVE_HalContext *phost, uint16_t value);
uint16_t EVE_HalImpl_MPSSE_transfer16(EVE_HalContext *phost, uint16_t value);
uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		return EVE_HalImpl_BT8XXEMU_transfer16(phost);
	case EVE_DEVICE_FT4222:
		return EVE_HalImpl_FT4222_transfer16(phost);
	case EVE_DEVICE_MPSSE:
		return EVE_HalImpl_MPSSE_transfer16(phost);
	}
}

uint32_t EVE_HalImpl_BT8XXEMU_transfer32(EVE_HalContext *phost, uint32_t value);
uint32_t EVE_HalImpl_FT4222_transfer32(EVE_HalContext *phost, uint32_t value);
uint32_t EVE_HalImpl_MPSSE_transfer32(EVE_HalContext *phost, uint32_t value);
uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		return EVE_HalImpl_BT8XXEMU_transfer32(phost);
	case EVE_DEVICE_FT4222:
		return EVE_HalImpl_FT4222_transfer32(phost);
	case EVE_DEVICE_MPSSE:
		return EVE_HalImpl_MPSSE_transfer32(phost);
	}
}

void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
}

void EVE_Hal_transferProgmem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size)
{
}

uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
}

/************
** UTILITY **
************/

void EVE_Hal_hostCommand(EVE_HalContext *phost, uint8_t cmd)
{
}

/* This API sends a 3byte command to the phost */
void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd)
{
}

/* Toggle PD_N pin of FT800 board for a power cycle */
void EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
}

void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy)
{
}

/*********
** MISC **
*********/

bool EVE_HalImpl_BT8XXEMU_bootupDisplayGpio(EVE_HalContext *phost);
bool EVE_HalImpl_FT4222_bootupDisplayGpio(EVE_HalContext *phost);
bool EVE_HalImpl_MPSSE_bootupDisplayGpio(EVE_HalContext *phost);
bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost)
{
	switch (phost->DeviceType)
	{
	case EVE_DEVICE_BT8XXEMU:
		return EVE_HalImpl_BT8XXEMU_bootupDisplayGpio(phost);
	case EVE_DEVICE_FT4222:
		return EVE_HalImpl_FT4222_bootupDisplayGpio(phost);
	case EVE_DEVICE_MPSSE:
		return EVE_HalImpl_MPSSE_bootupDisplayGpio(phost);
	}
}

#endif /* #if defined(BT8XXEMU_PLATFORM) */

/* end of file */
