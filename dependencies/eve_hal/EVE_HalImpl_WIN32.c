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
#if defined(_WIN32)

/*********
** MISC **
*********/

static DWORD s_Millis_Start;

void EVE_Millis_initialize()
{
	s_Millis_Start = GetTickCount();
}

void EVE_Millis_release()
{
	/* no-op */
}

/* global counter to loopback after ~49.71 days */
uint32_t EVE_millis()
{
	return GetTickCount() - s_Millis_Start;
}

#if defined(ESD_SIMULATION)
int Ft_Sleep__ESD(int ms);
#endif

void EVE_sleep(uint32_t ms)
{
#if defined(ESD_SIMULATION)
	Ft_Sleep__ESD(ms);
#else
	Sleep(ms);
#endif
}

#endif

/* end of file */
