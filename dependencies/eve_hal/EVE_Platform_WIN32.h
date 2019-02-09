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

#ifndef EVE_PLATFORM_WIN32__H
#define EVE_PLATFORM_WIN32__H

#include "EVE_Config.h"
#if defined(_WIN32)

/* Disable legacy C functions under TCC */
#ifndef NO_OLDNAMES
#define NO_OLDNAMES
#endif

/* Sane options for Windows header */
#ifndef NOMINMAX
#define NOMINMAX
#endif
#if !defined(NTDDI_VERSION) && !defined(_WIN32_WINNT) && !defined(WINVER)
#define NTDDI_VERSION 0x05010000 /* NTDDI_WINXP */
#define _WIN32_WINNT 0x0501 /* _WIN32_WINNT_WINXP */
#define WINVER 0x0501 /* _WIN32_WINNT_WINXP */
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/* C library inclusions */
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#define _USE_MATH_DEFINES 1
#include <math.h>

#include <direct.h>
#include <time.h>
#include <io.h>

#endif
#endif /* #ifndef EVE_PLATFORM_WIN32__H */

/* end of file */