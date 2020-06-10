/*
Bitmap info structure
*/

#ifndef ESD_BITMAPINFO__H
#define ESD_BITMAPINFO__H

#include "ESD_Base.h"
#include "ESD_GpuAlloc.h"
#include "ESD_ResourceInfo.h"

#pragma ESD_TYPE(ESD_BitmapInfo, Native = Struct) // TODO: Struct support, expose values
typedef struct ESD_BitmapInfo
{
	int32_t Width;
	int32_t Height;
	int32_t Stride;
	uint32_t Format;
	int32_t Size;
	union
	{
		const char *File;
		int32_t FlashAddress;
	};

	// (Runtime) Handle pointing to the address in RAM_G if it is allocated
	ESD_GpuHandle GpuHandle;

	// (Runtime) Bitmap handle that is being used
	uint32_t BitmapHandle;

	// Used for paletted format
	union
	{
		const char *PaletteFile;
		int32_t PaletteFlashAddress;
	};
	ESD_GpuHandle PaletteGpuHandle;

	// Used for DXT1 format
	union
	{
		const char *AdditionalFile;
		int32_t AdditionalFlashAddress;
	};
	struct ESD_BitmapInfo *AdditionalInfo;

	// Number of cells usable by the user. There may be additional internally used cells after this
	uint16_t Cells;

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
	// BT815 bitmap swizzle
	uint16_t SwizzleB : 3;
	uint16_t SwizzleG : 3;
	uint16_t SwizzleR : 3;
	uint16_t SwizzleA : 3;

	// Use swizzle
	bool Swizzle : 1;
#endif

	// Load file from compressed format using inflate
	bool Compressed : 1;

	// When this is set, the allocated ram is not free'd automatically
	// Use ESD_GpuAlloc_Free(GpuAlloc, GpuHandle) to free the GPU ram manually
	bool Persistent : 1;

	// Load from flash. Structure has flash addresses set, rather than file names
	bool Flash : 1;

	// Prefer loading the bitmap into RAM_G, even if it's an ASTC stored on Flash
	bool PreferRam : 1;

	// Load image using coprocessor (for JPEG and PNG)
	bool CoLoad : 1;

} ESD_BitmapInfo;

#pragma ESD_TYPE(ESD_BitmapInfo *, Native = Pointer, Edit = Library)

#pragma ESD_TYPE(ESD_BitmapCell, Native = Struct, Edit = Library) // TODO: Struct support, expose values
typedef struct
{
	ESD_BitmapInfo *Info;
	int32_t Cell;
} ESD_BitmapCell;

#pragma ESD_TYPE(ESD_BitmapCell *, Native = Pointer, Edit = Library)

/// A function to load bitmap data(not including palette data) into RAM_G
#pragma ESD_FUNCTION(ESD_LoadBitmap, Type = uint32_t, Attributes = ESD_CORE_EXPORT, Include = "ESD_BitmapInfo.h", DisplayName = "Load Bitmap to RAM_G", Category = EsdUtilities)
#pragma ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
ESD_CORE_EXPORT uint32_t ESD_LoadBitmap(ESD_BitmapInfo *bitmapInfo);

/// A function to load palette data of bitmap into RAM_G
#pragma ESD_FUNCTION(ESD_LoadPalette, Type = uint32_t, Attributes = ESD_CORE_EXPORT, Include = "ESD_BitmapInfo.h", DisplayName = "Load Palette to RAM_G", Category = EsdUtilities)
#pragma ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
ESD_CORE_EXPORT uint32_t ESD_LoadPalette(ESD_BitmapInfo *bitmapInfo);

#pragma ESD_ENUM(_BitmapResourceFormat, DisplayName = "Bitmap Format")
// Hardware bitmap formats
#pragma ESD_IDENTIFIER(ARGB1555)
#pragma ESD_IDENTIFIER(L1)
#pragma ESD_IDENTIFIER(L2)
#pragma ESD_IDENTIFIER(L4)
#pragma ESD_IDENTIFIER(L8)
#pragma ESD_IDENTIFIER(RGB332)
#pragma ESD_IDENTIFIER(ARGB2)
#pragma ESD_IDENTIFIER(ARGB4)
#pragma ESD_IDENTIFIER(RGB565)
#pragma ESD_IDENTIFIER(PALETTED565)
#pragma ESD_IDENTIFIER(PALETTED4444)
#pragma ESD_IDENTIFIER(PALETTED8)
// Specially loaded bitmap formats
#define DXT1 0x81
#define JPEG 0x91
#define PNG 0x92
#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_4x4_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_5x4_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_5x5_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_6x5_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_6x6_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_8x5_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_8x6_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_8x8_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_10x5_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_10x6_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_10x8_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_10x10_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_12x10_KHR)
#pragma ESD_IDENTIFIER(COMPRESSED_RGBA_ASTC_12x12_KHR)
#endif
#pragma ESD_END()

///  Switch bitmap cell number
#pragma ESD_FUNCTION(ESD_BitmapCell_Switched, Type = ESD_BitmapCell, DisplayName = "Switch Bitmap Cell", Category = EsdUtilities, Inline)
#pragma ESD_PARAMETER(BitmapInfo, Type = ESD_BitmapCell, DisplayName = "Bitmap Cell")
#pragma ESD_PARAMETER(Cell, Type = uint16_t, DisplayName = "Cell")
static inline ESD_BitmapCell ESD_BitmapCell_Switched(ESD_BitmapCell bitmapCell, uint16_t cell)
{
	bitmapCell.Cell = cell;
	return bitmapCell;
}

/// A function to make bitmap persistent in memory by reloading the data if necessary, called during the Update cycle of each frame
#pragma ESD_UPDATE(ESD_BitmapCell_Persist, DisplayName = "Persist Bitmap", Category = EsdUtilities)
#pragma ESD_PARAMETER(bitmapCell, Type = ESD_BitmapCell)
ESD_CORE_EXPORT void ESD_BitmapCell_Persist(ESD_BitmapCell bitmapCell);

/// A function to get bitmap information structure from bitmap cell
#pragma ESD_FUNCTION(ESD_BitmapCell_GetInfo, Type = ESD_BitmapInfo *, DisplayName = "ESD BitmapCell GetInfo", Category = EsdUtilities, Inline)
#pragma ESD_PARAMETER(bitmapCell, Type = ESD_BitmapCell)
static inline ESD_BitmapInfo *ESD_BitmapCell_GetInfo(ESD_BitmapCell bitmapCell)
{
	return bitmapCell.Info;
}

/// A function to get the bitmap width from bitmap information. Returns width of 1 pixel for NULL bitmapInfo
#pragma ESD_FUNCTION(ESD_BitmapInfo_GetWidth, Type = int32_t, DisplayName = "ESD BitmapInfo GetWidth", Category = EsdUtilities, Inline)
#pragma ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
static inline int32_t ESD_BitmapInfo_GetWidth(ESD_BitmapInfo *bitmapInfo)
{
	if (!bitmapInfo)
		return 1;
	return bitmapInfo->Width;
}

/// A function to get the bitmap height from bitmap information. Returns height of 1 pixel for NULL bitmapInfo
#pragma ESD_FUNCTION(ESD_BitmapInfo_GetHeight, Type = int32_t, DisplayName = "ESD BitmapInfo GetHeight", Category = EsdUtilities, Inline)
#pragma ESD_PARAMETER(bitmapInfo, Type = ESD_BitmapInfo *)
static inline int32_t ESD_BitmapInfo_GetHeight(ESD_BitmapInfo *bitmapInfo)
{
	if (!bitmapInfo)
		return 1;
	return bitmapInfo->Height;
}

#endif /* #ifndef ESD_BITMAPINFO__H */

/* end of file */
