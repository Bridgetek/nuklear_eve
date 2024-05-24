/*
ESD Framework
Copyright (C) 2022  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

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

/*
LittleFS for EVE Screen Designer
*/

#ifndef ESD_LITTLEFS__H
#define ESD_LITTLEFS__H

#include "Esd_Base.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ESD_LITTLEFS_FLASH

// Mount filesystem
ESD_CORE_EXPORT bool Esd_LittleFS_Mount();

// Unmount filesystem
ESD_CORE_EXPORT void Esd_LittleFS_Unmount();

// Read resource metadata file
ESD_CORE_EXPORT bool Esd_LittleFs_ReadMetadataFile(uint8_t *metadata, const char *resourceFile);

#endif

#ifdef EVE_FLASH_AVAILABLE
#ifdef ESD_LITTLEFS_FLASH
ESD_CORE_EXPORT int32_t Esd_LittleFs_LoadFlashAddress(int32_t *addr, const char *file, uint8_t *metadata);
#else
#define Esd_LittleFs_LoadFlashAddress(addr, file, metadata) (*(addr))
#endif
#else
#define Esd_LittleFs_LoadFlashAddress(addr, file, metadata) (FA_INVALID)
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ESD_LITTLEFS__H */

/* end of file */
