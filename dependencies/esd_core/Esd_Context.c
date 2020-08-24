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

#include "Esd_Context.h"
#include "Esd_Utility.h"

#include "Esd_GpuAlloc.h"
#include "Esd_Scissor.h"
#include "Esd_BitmapHandle.h"
#include "Esd_TouchTag.h"
#include "Esd_CoWidget.h"

//
// Globals
//
ESD_CORE_EXPORT Esd_Context *Esd_CurrentContext = NULL;
ESD_CORE_EXPORT EVE_HalContext *Esd_Host = NULL; // Pointer to current s_Host
ESD_CORE_EXPORT Esd_GpuAlloc *Esd_GAlloc = NULL; // Pointer to current s_GAlloc

//
// External definitions
//
#if _DEBUG
void Esd_CheckTypeSizes();
#endif

// When not in the simulation, use the Esd_Main__Start etc symbols
// as exported by the single Application logic document included
#ifndef ESD_SIMULATION
#if defined(EVE_MULTI_TARGET)
#define Esd_IsRunning__ESD() (phost->Host == EVE_HOST_BT8XXEMU ? BT8XXEMU_isRunning(phost->Emulator) : true)
#elif defined(BT8XXEMU_PLATFORM)
#define Esd_IsRunning__ESD() BT8XXEMU_isRunning(Esd_Host->Emulator)
#else
#define Esd_IsRunning__ESD() true
#endif
#else
int Esd_IsRunning__ESD();
#endif

void Esd_Scissor_DlStart();

// extern void Esd_Widget_ProcessFree(); // TODO: Bind from widgets

// extern void Esd_Timer_CancelGlobal(); // TODO: Bind from widgets
// extern void Esd_Timer_UpdateGlobal(); // TODO: Bind from widgets

//
// Constants
//
#define FT_WELCOME_MESSAGE "Copyright (C) Bridgetek Pte Ltd\n"

ESD_CORE_EXPORT void Esd_SetCurrent(Esd_Context *ec)
{
	Esd_CurrentContext = ec;
	Esd_Host = &ec->HalContext;
	Esd_GAlloc = &ec->GpuAlloc;
}

ESD_CORE_EXPORT void Esd_Defaults(Esd_Parameters *ep)
{
	memset(ep, 0, sizeof(Esd_Parameters));
}

bool cbCmdWait(struct EVE_HalContext *phost)
{
	Esd_Context *ec = (Esd_Context *)phost->UserContext;

	/* Loop an idle task instead of doing nothing */
	if (ec->Idle)
		ec->Idle(ec->UserContext);
	/* EVE_Hal_idle(&ec->HalContext); */ /* Already called by EVE HAL */
	ec->SwapIdled = true;

	return Esd_IsRunning__ESD() && !ec->RequestStop;
}

ESD_CORE_EXPORT void Esd_Initialize()
{
	EVE_Hal_initialize();

#ifdef ESD_SIMULATION
	eve_printf_debug("\f"); // Shows horizontal line in ESD output window
#endif
	eve_printf_debug(FT_WELCOME_MESSAGE);

#if _DEBUG
	Esd_CheckTypeSizes();
#endif
}

#ifdef ESD_SIMULATION

void Esd_HookOpen__ESD(Esd_Context *ec);
void Esd_HookClose__ESD(Esd_Context *ec);

Esd_Callback Esd_HookStart__ESD(Esd_Context *ec, Esd_Callback start);
Esd_Callback Esd_HookUpdate__ESD(Esd_Context *ec, Esd_Callback update);
Esd_Callback Esd_HookRender__ESD(Esd_Context *ec, Esd_Callback render);
Esd_Callback Esd_HookIdle__ESD(Esd_Context *ec, Esd_Callback idle);
Esd_Callback Esd_HookEnd__ESD(Esd_Context *ec, Esd_Callback end);

#endif

ESD_CORE_EXPORT void Esd_Open(Esd_Context *ec, Esd_Parameters *ep)
{
	EVE_HalContext *phost;
	EVE_CHIPID_T chipId;
	size_t deviceIdx;

	memset(ec, 0, sizeof(Esd_Context));
	ec->ClearColor = 0x212121;
	ec->Start = ep->Start;
	ec->Update = ep->Update;
	ec->Render = ep->Render;
	ec->Idle = ep->Idle;
	ec->End = ep->End;
	ec->UserContext = ep->UserContext;
	Esd_SetCurrent(ec);

#ifdef ESD_SIMULATION
	Esd_HookOpen__ESD(ec);
	ec->Start = Esd_HookStart__ESD(ec, ec->Start);
	ec->Update = Esd_HookStart__ESD(ec, ec->Update);
	ec->Render = Esd_HookStart__ESD(ec, ec->Render);
	ec->Idle = Esd_HookStart__ESD(ec, ec->Idle);
	ec->End = Esd_HookStart__ESD(ec, ec->End);
#endif

#ifdef ESD_SIMULATION
	chipId = EVE_SUPPORT_CHIPID;
	deviceIdx = -1;
#else
	/* Interactive device selection */
	EVE_Util_selectDeviceInteractive(&chipId, &deviceIdx);
#endif

	{
		EVE_HalParameters params;
#if defined(BT8XXEMU_PLATFORM) || defined(EVE_MULTI_TARGET)
		BT8XXEMU_EmulatorParameters emulatorParams;
		BT8XXEMU_FlashParameters flashParams;
#endif

		EVE_Hal_defaultsEx(&params, deviceIdx);
		params.UserContext = ec;
		params.CbCmdWait = cbCmdWait;

#if defined(BT8XXEMU_PLATFORM) || defined(EVE_MULTI_TARGET)
		if (params.Host == EVE_HOST_BT8XXEMU)
		{
			BT8XXEMU_defaults(BT8XXEMU_VERSION_API, &emulatorParams, chipId);
			emulatorParams.Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
			// TODO: emulatorParams.Log
			params.EmulatorParameters = &emulatorParams;
			/*
			if (flashFile)
			{
				BT8XXEMU_Flash_defaults(BT8XXEMU_VERSION_API, &flashParams);
				wcscpy_s(flashParams.DataFilePath, _countof(flashParams.DataFilePath), flashPath[0] ? flashPath : flashFile);
				flashParams.SizeBytes = 2 * 1024 * 1024;
				while (flashParams.SizeBytes < flashSize)
					flashParams.SizeBytes *= 2;
				// TODO: flashParams.Log
				params.EmulatorFlashParameters = &flashParams;
			}
			*/
		}
#endif

		eve_assert_do(EVE_Hal_open(&ec->HalContext, &params));
	}

	phost = &ec->HalContext;
#if ESD_SIMULATION
	eve_assert_do(EVE_Util_bootupConfig(phost));
#else
	eve_assert_do(EVE_Util_bootupConfigInteractive(phost, EVE_DISPLAY_DEFAULT));
#endif

#ifndef ESD_SIMULATION
	// TODO: Store calibration somewhere
	if (!Esd_Calibrate())
	{
		eve_printf_debug("Calibrate failed\n");
	}
#endif

	ec->GpuAlloc.RamGSize = RAM_G_SIZE;
	Esd_GpuAlloc_Reset(&ec->GpuAlloc);

	Esd_BitmapHandle_Initialize();
}

ESD_CORE_EXPORT void Esd_Close(Esd_Context *ec)
{
	// Esd_Widget_ProcessFree(); // TODO: Link this back up (add Esd_Framework_Initialize and Esd_Framework_Release)
	EVE_Hal_close(&ec->HalContext);
	memset(ec, 0, sizeof(Esd_Context));

#ifdef ESD_SIMULATION
	Esd_HookClose__ESD(ec);
#endif

	Esd_CurrentContext = NULL;
	Esd_Host = NULL;
	Esd_GAlloc = NULL;
}

ESD_CORE_EXPORT void Esd_Release()
{
	EVE_Hal_release();
}

ESD_CORE_EXPORT void Esd_Loop(Esd_Context *ec)
{
	EVE_HalContext *phost = &ec->HalContext;
	Esd_SetCurrent(ec);
	(void)phost;

	if (!Esd_IsRunning__ESD() || ec->RequestStop)
		return;

	Esd_Start(ec);

	while (Esd_IsRunning__ESD() && !ec->RequestStop)
	{
		Esd_Update(ec);
		Esd_Render(ec);
		Esd_WaitSwap(ec);
	}

	Esd_Stop(ec);
}

ESD_CORE_EXPORT void Esd_Start(Esd_Context *ec)
{
	Esd_SetCurrent(ec);

	// Initialize framework
	Esd_Scissor_DlStart();
	ec->Frame = 0;
	ec->Millis = EVE_millis();
	// Esd_Timer_CancelGlobal(); // TODO (in Esd_Framework_Initialize)

	// Initialize storage
	EVE_Util_loadSdCard(&ec->HalContext);
#if defined(EVE_FLASH_AVAILABLE)
	Esd_AttachFlashFast();
#endif

	// Initialize application
	if (ec->Start)
		ec->Start(ec->UserContext);
}

ESD_CORE_EXPORT void Esd_Update(Esd_Context *ec)
{
	uint32_t ms;

	EVE_HalContext *phost = &ec->HalContext;
	Esd_SetCurrent(ec);

	// Restore initial frame values
	// EVE_CoCmd_loadIdentity(phost); // ?
	Esd_Scissor_DlStart();
	// Esd_Widget_ProcessFree(); // TODO: Link this back up!!!
	Esd_BitmapHandle_FrameStart(&ec->HandleState);

	if (ec->ShowLogo)
		return;
	if (ec->ShowingLogo)
		ec->Millis = EVE_millis(); // Reset time

	// Verify initialization
	EVE_Util_loadSdCard(phost); // Does this really need phost???
#if defined(EVE_FLASH_AVAILABLE)
	Esd_AttachFlashFast();
#endif

	// Idle at least once every frame
	ec->LoopState = ESD_LOOPSTATE_IDLE;
	if (!ec->SwapIdled)
	{
		if (ec->Idle)
			ec->Idle(ec->UserContext);
		EVE_Hal_idle(phost);
	}

	// Update GUI state before render
	ec->LoopState = ESD_LOOPSTATE_UPDATE;
	ms = EVE_millis(); // Calculate frame time delta
	ec->DeltaMs = ms - ec->Millis;
	ec->Millis = ms;
	Esd_GpuAlloc_Update(Esd_GAlloc); // Run GC
	Esd_TouchTag_Update(NULL); // Update touch
	if (ec->Update)
		ec->Update(ec->UserContext);
	// Esd_Timer_UpdateGlobal(); // TODO

	// Return to idle state inbetween
	ec->LoopState = ESD_LOOPSTATE_IDLE;
}

ESD_CORE_EXPORT void Esd_Render(Esd_Context *ec)
{
	EVE_HalContext *phost = &ec->HalContext;
	Esd_SetCurrent(ec);

	if (ec->ShowLogo)
	{
		ec->ShowLogo = false;
		ec->ShowingLogo = true;
		Esd_BeginLogo();
		return;
	}

	if (ec->ShowingLogo)
	{
		ec->ShowingLogo = false;
		Esd_EndLogo();
	}

	// Process all coprocessor commands
	ec->LoopState = ESD_LOOPSTATE_RENDER;

	EVE_CoCmd_dlStart(phost);
	EVE_CoDl_clearColorRgb_ex(phost, ec->ClearColor); // Set CLEAR_COLOR_RGB from user var
	EVE_CoDl_clearTag(phost, 255); // Always default to 255, so no touch = 0, touch non-tag = 255
	EVE_CoDl_clear(phost, true, true, true);
	if (ec->Render)
		ec->Render(ec->UserContext);

	if (ec->SpinnerPopup)
	{
		// Spinner used for switching longer loading pages with bitmaps etc
		EVE_CoDl_colorRgb_ex(phost, ~(ec->ClearColor));
		Esd_CoWidget_Spinner(Esd_Update, phost->Width >> 1, phost->Height >> 1, 0, 0);
		ec->SpinnerPopup = false;
		ec->SpinnerPopped = true;
	}
	else if (ec->SpinnerPopped)
	{
		Esd_CoWidget_Stop(Esd_Update);
		ec->SpinnerPopped = false;
	}

	EVE_CoDl_display(phost);
	EVE_CoCmd_swap(phost);

	// Replacement for Ft_Gpu_Hal_WaitCmdfifo_empty(phost); with idle function
	ec->LoopState = ESD_LOOPSTATE_IDLE;

	// Advance frame count
	++ec->Frame;
}

ESD_CORE_EXPORT bool Esd_WaitSwap(Esd_Context *ec)
{
	EVE_HalContext *phost = &ec->HalContext;
	Esd_SetCurrent(ec);
	(void)phost;

	ec->SwapIdled = false;
	EVE_Cmd_waitFlush(&ec->HalContext);

	/* Reset the coprocessor in case of fault */
	if (ec->HalContext.CmdFault)
	{
		/* TODO: Create a utility function that resets the coprocessor and all cached state */
		EVE_Util_resetCoprocessor(&ec->HalContext);
		Esd_BitmapHandle_Reset(&ec->HandleState);

#if _DEBUG
		/* Show error for a while */
		EVE_sleep(1000);
#endif

		return false;
	}

	return true;
}

ESD_CORE_EXPORT void Esd_Stop(Esd_Context *ec)
{
	Esd_SetCurrent(ec);

	// Cleanup application (generally unreachable)
	ec->LoopState = ESD_LOOPSTATE_NONE;
	// Esd_Timer_CancelGlobal(); // TODO
	if (ec->End)
		ec->End(ec->UserContext);
}

/* Implements a generic main function, for reference only */
/*
int Esd_Main(int argc, char *argv[])
{
	Esd_Initialize();

	Esd_Parameters ep;
	Esd_Defaults(&ep);
	ep.Start = Main_Start;
	ep.Update = Main_Update;
	ep.Render = Main_Render;
	ep.Idle = Main_Idle;
	ep.End = Main_End;
	Esd_Context ec;

	Esd_Open(&ec, &ep);
	Esd_Loop(&ec);
	Esd_Close(&ec);

	Esd_Release();
	return EXIT_SUCCESS;
}
*/

/* end of file */
