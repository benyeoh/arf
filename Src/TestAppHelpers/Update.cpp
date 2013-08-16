//==============================================================================
//
//		Update.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/11/2009
//
//==============================================================================

#include "stdafx.h"

void SetScreenMode(UINT screenWidth, UINT screenHeight, BOOL fullScreen)
{
	RDisplayInfo info;

	info.backBufferHeight = screenHeight;
	info.backBufferWidth = screenWidth;
	info.fullScreen = fullScreen;
	info.hWnd = g_hWnd;

	g_pRenderer->SetDisplayInfo(info);

	gmtl::setPerspective(g_Proj, gmtl::Math::PI / 3.0f, ((float)screenWidth) / screenHeight, NEAR_PLANE, FAR_PLANE);
}

void ProcessInput()
{
	g_pInput->Update();

	// Process mouse input
	IIMouse& mouse = g_pInput->GetMouse();
	uint numActiveButtons = mouse.GetNumOfActiveButtons();
	_LOOPi(numActiveButtons)
	{
		const eIMouseCode* pActiveButtons = mouse.GetActiveButtons();
		if(pActiveButtons[i] == IMC_RIGHTBUTTON)
		{
			// Rotate camera

			// Get direction vector of mouse movement in screen space
			gmtl::VecA3f mouseVec;//gmtl::Vec3f(1.0f, 1.0f, 0.0f);
			_CAST_VEC3(mouseVec) = mouse.GetPosDelta();
			mouseVec[1] = -mouseVec[1];
			float length = gmtl::normalize(_CAST_VEC3(mouseVec));
			if(length > gmtl::GMTL_EPSILON)
			{			
				// In reference space
				gmtl::VecA3f axisOfRot;
				gmtl::cross(_CAST_VEC3(axisOfRot), _CAST_VEC3(mouseVec), gmtl::Vec3f(0.0f, 0.0f, 1.0f));

				// Convert rotation to eye space
				gmtl::MatrixA44f rotToWorldSpaceMat;
				gmtl::VecA3f xAxis, yAxis, zAxis;
				_CAST_VEC3(zAxis) = -_CAST_VEC3(g_EyeDir);
				yAxis = g_EyeUpVec;
				gmtl::cross(_CAST_VEC3(xAxis), _CAST_VEC3(yAxis), _CAST_VEC3(zAxis));
				NormalizeVec(xAxis);
				gmtl::cross(_CAST_VEC3(yAxis), _CAST_VEC3(zAxis), _CAST_VEC3(xAxis));
				NormalizeVec(yAxis);
				rotToWorldSpaceMat.set(xAxis[0], xAxis[1], xAxis[2],
					yAxis[0], yAxis[1], yAxis[2], 						
					zAxis[0], zAxis[1], zAxis[2]);					
				gmtl::invert(_CAST_MAT44(rotToWorldSpaceMat));
				gmtl::VecA3f axisOfRotInWorldSpace;
				TransformVec(&rotToWorldSpaceMat, &axisOfRot, &axisOfRotInWorldSpace);

				gmtl::AxisAnglef rotAxis;
				rotAxis.setAxis(_CAST_VEC3(axisOfRotInWorldSpace));
				rotAxis.setAngle(length * 0.005f);

				// Transform eye direction
				gmtl::MatrixA44f cameraRotMat;
				gmtl::setRot(cameraRotMat, rotAxis);

				gmtl::VecA3f oldEyeDir = g_EyeDir;
				TransformVec(&cameraRotMat, &oldEyeDir, &oldEyeDir);
				g_EyeDir = oldEyeDir;

				//gmtl::xform(g_EyeDir, cameraRotMat, oldEyeDir);
			}			
		}
	}

	// Process keyboard input
	IIKeyboard& keyboard = g_pInput->GetKeyboard();
	uint numActiveKeys = keyboard.GetNumOfActiveKeys();
	const eIKeyCode* keyCodes = keyboard.GetActiveKeys();
	_LOOPi(numActiveKeys)
	{
        const static float SPEED = 100.0f;

		switch(keyCodes[i])
		{
		case IKC_W:
			{
				_CAST_VEC3(g_EyePos) = _CAST_VEC3(g_EyePos) + g_TimeDT * _CAST_VEC3(g_EyeDir) * SPEED;
				break;
			}

		case IKC_A:
			{
				gmtl::Vec3f right;
				gmtl::Vec3f zAxis = -_CAST_VEC3(g_EyeDir);
				gmtl::cross(right, _CAST_VEC3(g_EyeUpVec), zAxis);
				gmtl::normalize(right);
				_CAST_VEC3(g_EyePos) = _CAST_VEC3(g_EyePos) - g_TimeDT * _CAST_VEC3(right) * SPEED;
				break;
			}

		case IKC_D:
			{
				gmtl::Vec3f right;
				gmtl::Vec3f zAxis = -_CAST_VEC3(g_EyeDir);
				gmtl::cross(right, _CAST_VEC3(g_EyeUpVec), zAxis);
				gmtl::normalize(right);
				_CAST_VEC3(g_EyePos) = _CAST_VEC3(g_EyePos) + g_TimeDT * _CAST_VEC3(right) * SPEED;
				break;
			}

		case IKC_S:
			{
				_CAST_VEC3(g_EyePos) = _CAST_VEC3(g_EyePos) - g_TimeDT * _CAST_VEC3(g_EyeDir) * SPEED;
				break;
			}
		}

		uint numJustPressedKeys = keyboard.GetNumOfJustPressedKeys();
		const eIKeyCode* justPressedCodes = keyboard.GetJustPressedKeys();
		_LOOPi(numJustPressedKeys)
		{
			switch(justPressedCodes[i])
			{
			case IKC_ESC:
				{
					if(g_IsFullScreen)
					{
						SetWindowPos(g_hWnd, NULL, 0, 0, 1024, 768, SWP_NOREPOSITION);	

						RECT clRect;
						GetClientRect(g_hWnd, &clRect);
						SetScreenMode(clRect.right - clRect.left, clRect.bottom - clRect.top, FALSE);		
						g_IsFullScreen = FALSE;	
					}
					break;
				}

			case IKC_1:
				{
					g_IsDebugRenderLights = !g_IsDebugRenderLights;
					break;
				}

			case IKC_2:
				{
					g_IsDebugRenderSceneDB = !g_IsDebugRenderSceneDB;
					break;
				}

			case IKC_4:
				{
					g_IsWireFrameMode = !g_IsWireFrameMode;
					//g_pBaseFX->GetDebug().SetWireFrame(g_IsWireFrameMode);
					break;
				}


			}
		}

	}

	// Set view
	gmtl::Vec3f at;
	at = _CAST_VEC3(g_EyePos) + _CAST_VEC3(g_EyeDir);
	gmtl::setView(_CAST_MAT44(g_View), 
		_CAST_VEC3(g_EyePos), 
		_CAST_VEC3(at), 
		_CAST_VEC3(g_EyeUpVec));

	//g_pShadowDirLight->Update(g_CascadeFrustum, g_DirLight.dir, _CAST_VEC3(g_EyePos) - g_DirLight.dir * 100.0f, _CAST_MAT44(g_View));

    // Disable all AI
#ifndef _USE_PRT_VERSION
    AABox box;
    box.min[0] = g_EyePos[0] - FAR_PLANE;
    box.min[1] = g_EyePos[1] - FAR_PLANE;
    box.min[2] = g_EyePos[2] - FAR_PLANE;

    box.max[0] = g_EyePos[0] + FAR_PLANE;
    box.max[1] = g_EyePos[1] + FAR_PLANE;
    box.max[2] = g_EyePos[2] + FAR_PLANE;

    Sphere enableSphere;
    enableSphere.center[0] = g_EyePos[0];
    enableSphere.center[1] = g_EyePos[1];
    enableSphere.center[2] = g_EyePos[2];
    enableSphere.center[3] = FAR_PLANE;

    AHComponent* pAI[64000];
    uint numEntities = g_pSceneEntityFlockContainer->QuerySphere((void**) pAI, 64000, box, enableSphere, SDB_FILTER_FLOCKING_COMP);
    _LOOPi(numEntities)
    {
        FlockingAIComponent* pFlock = (FlockingAIComponent*) pAI[i];
        pFlock->EnableOneFrame();
    }

#endif
	double renderStart = g_pPlatform->GetTimer().GetTime();
	g_pEntityMgr->Update( UPDATE_PHASE_GROUP_1 );
	g_UpdateTimes[g_TimeCounter] = g_pPlatform->GetTimer().GetTime() - renderStart;

//	g_pSkyDomeSpatial->SetWorldPos(g_EyePos);
//	g_pEntityMgr->UpdateComponentsTemp();

	float amount = (float)g_pPlatform->GetTimer().GetTime() * 0.5f;
	//g_PointLight.pos = _CAST_VEC3(g_PointLightPos) + amount * gmtl::Vec3f(5.0f, 0.0f, 0.0f);
	//gmtl::setTrans(_CAST_MAT44(g_PointLightWorld), g_PointLight.pos);
	//
	//g_pShadowPointLight->Update(g_PointLight.pos);
	//g_pShadowPointLight->SetRange(1.0f / g_PointLight.invRange);

	//float piInc = gmtl::Math::TWO_PI / NUM_POINT_LIGHTS;
	//_LOOPi(NUM_POINT_LIGHTS)
	//{
	//	float x = cos(piInc * i + amount) * 2.0f;
	//	float z = sin(piInc * i + amount) * 2.0f;

	//	g_PointLightPos[i]	= gmtl::VecA3f(x, 4.0f, z);
	//	g_PointLight[i].pos = _CAST_VEC3(g_PointLightPos[i]);
	//	gmtl::setTrans(_CAST_MAT44(g_PointLightWorld[i]), g_PointLight[i].pos);

	//	g_pShadowPointLight[i]->Update(g_PointLight[i].pos);

	//}
}