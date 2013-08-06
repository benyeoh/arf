//==============================================================================
//
//		Defines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/21/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

const static uint SCENE_OBJECT_CHUNK_H	= 0x08;

const static uint COMP_RENDER_CHUNK_H	= _MAKE_ID('c', 'r', 'e', 'n');

// Components
const static int MAX_COMPONENT_TYPES	= 128;

// Component Type
enum
{
	COMP_START = -1,

	#include "BasicCompDefines.h"

	COMP_APPHELPERS_NUM,

	COMP_NUM_RESERVED = 32,
};

// Component Events
enum
{
	EVENT_COMP_START = -1,
	EVENT_COMP_REMOVED,
	EVENT_COMP_ADDED,

	#include "BasicCompEventDefines.h"

	EVENT_COMP_APPHELPERS_NUM,
};

// Component update phases
enum
{
	UPDATE_PHASE_GROUP_0	= 0x0,

	UPDATE_PHASE_GATHER_0	= UPDATE_PHASE_GROUP_0,
	UPDATE_PHASE_STORE_0,
	//UPDATE_PHASE_POST_0,

	UPDATE_PHASE_GROUP_1,

	UPDATE_PHASE_GATHER_1	= UPDATE_PHASE_GROUP_1,				// Default
	UPDATE_PHASE_STORE_1,										// AI, Gameplay, Animation update, Animation-To-Physics, Physics Mod
	//UPDATE_PHASE_POST_1,
													
	UPDATE_PHASE_GROUP_2,										// Physics

	UPDATE_PHASE_GATHER_2	= UPDATE_PHASE_GROUP_2,
	UPDATE_PHASE_STORE_2,										// Physics-To-Spatial
	//UPDATE_PHASE_POST_2,

	UPDATE_PHASE_GROUP_3,										

	UPDATE_PHASE_GATHER_3	= UPDATE_PHASE_GROUP_3,
	UPDATE_PHASE_STORE_3,										// Camera SetPos
	//UPDATE_PHASE_POST_3,

	MAX_NUM_UPDATE_PHASES,

	UPDATE_PHASE_TEMP_COMP = 0xFFFFFFFF
};

_NAMESPACE_END