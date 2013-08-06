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

void				
SetScreenMode(UINT screenWidth, UINT screenHeight, BOOL fullScreen)
{
	RDisplayInfo info;

	info.backBufferHeight = screenHeight;
	info.backBufferWidth = screenWidth;
	info.fullScreen = fullScreen;
	info.hWnd = g_hWnd;

	g_pRenderer->SetDisplayInfo(info);

	gmtl::setPerspective(g_Proj, gmtl::Math::PI / 3.0f, ((float)screenWidth) / screenHeight, 0.01f, 500.0f);
}

void
UpdateSphereTree(SEFXUSphereTree* pSphereTree, const gmtl::VecA4f& translate, uint index)
{
	SEFXUSphereTreeLevel& level = pSphereTree->GetLevel(index);
	
	_LOOPi(level.numValidSpheres)
	{
		Sphere& sphere = level.pSpheres[i];
		VecVecAdd(&sphere.center, &sphere.center, &translate);
	}
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
				rotAxis.setAngle(length * g_TimeDT * 2.0f);

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
		switch(keyCodes[i])
		{
		case IKC_W:
			{
				_CAST_VEC3(g_EyePos) = _CAST_VEC3(g_EyePos) + g_TimeDT * _CAST_VEC3(g_EyeDir) * 20.0f;
				break;
			}

		case IKC_A:
			{
				gmtl::Vec3f right;
				gmtl::Vec3f zAxis = -_CAST_VEC3(g_EyeDir);
				gmtl::cross(right, _CAST_VEC3(g_EyeUpVec), zAxis);
				gmtl::normalize(right);
				_CAST_VEC3(g_EyePos) = _CAST_VEC3(g_EyePos) - g_TimeDT * _CAST_VEC3(right) * 20.0f;
				break;
			}

		case IKC_D:
			{
				gmtl::Vec3f right;
				gmtl::Vec3f zAxis = -_CAST_VEC3(g_EyeDir);
				gmtl::cross(right, _CAST_VEC3(g_EyeUpVec), zAxis);
				gmtl::normalize(right);
				_CAST_VEC3(g_EyePos) = _CAST_VEC3(g_EyePos) + g_TimeDT * _CAST_VEC3(right) * 20.0f;
				break;
			}

		case IKC_S:
			{
				_CAST_VEC3(g_EyePos) = _CAST_VEC3(g_EyePos) - g_TimeDT * _CAST_VEC3(g_EyeDir) * 20.0f;
				break;
			}

		case IKC_I:
			{
				gmtl::VecA4f trans;
				trans[3] = 0.0f;
				gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
				gmtl::VecA4f diff;
				diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
				diff[1] += g_TimeDT * 3.1f;
				trans[1] += g_TimeDT * 3.1f;
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
				//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
				break;
			}

		case IKC_J:
			{
				gmtl::VecA4f trans;
				trans[3] = 0.0f;
				gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
				gmtl::VecA4f diff;
				diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
				diff[0] -= g_TimeDT * 10.1f;
				trans[0] -= g_TimeDT * 10.1f;
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
				//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
				break;
			}

		case IKC_L:
			{
				gmtl::VecA4f trans;
				trans[3] = 0.0f;
				gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
				gmtl::VecA4f diff;
				diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
				diff[0] += g_TimeDT * 10.1f;
				trans[0] += g_TimeDT * 10.1f;
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
				//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
				break;
			}

		case IKC_K:
			{
				gmtl::VecA4f trans;
				trans[3] = 0.0f;
				gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
				gmtl::VecA4f diff;
				diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
				diff[1] -= g_TimeDT * 3.1f;
				trans[1] -= g_TimeDT * 3.1f;
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
				//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
				break;
			}

		case IKC_M:
			{
				gmtl::VecA4f trans;
				trans[3] = 0.0f;
				gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
				gmtl::VecA4f diff;
				diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
				diff[2] += g_TimeDT * 10.1f;
				trans[2] += g_TimeDT * 10.1f;
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
				//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
				break;
			}

		case IKC_N:
			{
				gmtl::VecA4f trans;
				trans[3] = 0.0f;
				gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
				gmtl::VecA4f diff;
				diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
				diff[2] -= g_TimeDT * 10.1f;
				trans[2] -= g_TimeDT * 10.1f;
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
				//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
				break;
			}

		case IKC_O:
			{
				gmtl::AxisAnglef rotAxis;
				rotAxis.setAxis( gmtl::Vec3f(0.0f, 1.0f, 0.0f) );
				rotAxis.setAngle( 1.0f * g_TimeDT );
				gmtl::MatrixA44f rot;
				gmtl::setRot(rot, rotAxis);

				gmtl::Vec3f trans(g_MeshesWorld[g_CurObjectIndex][0][3], g_MeshesWorld[g_CurObjectIndex][1][3], g_MeshesWorld[g_CurObjectIndex][2][3]);
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
				MatMatMult(&g_MeshesWorld[g_CurObjectIndex], &rot, &g_MeshesWorld[g_CurObjectIndex]);
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), trans);
				break;
			}

		case IKC_P:
			{
				gmtl::AxisAnglef rotAxis;
				rotAxis.setAxis( gmtl::Vec3f(0.0f, 1.0f, 0.0f) );
				rotAxis.setAngle( -1.0f * g_TimeDT );
				gmtl::MatrixA44f rot;
				gmtl::setRot(rot, rotAxis);

				gmtl::Vec3f trans(g_MeshesWorld[g_CurObjectIndex][0][3], g_MeshesWorld[g_CurObjectIndex][1][3], g_MeshesWorld[g_CurObjectIndex][2][3]);
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
				MatMatMult(&g_MeshesWorld[g_CurObjectIndex], &rot, &g_MeshesWorld[g_CurObjectIndex]);
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), trans);
				break;
			}

		case IKC_Y:
			{
				gmtl::AxisAnglef rotAxis;
				rotAxis.setAxis( gmtl::Vec3f(1.0f, 0.0f, 0.0f) );
				rotAxis.setAngle( 1.0f * g_TimeDT );
				gmtl::MatrixA44f rot;
				gmtl::setRot(rot, rotAxis);

				gmtl::Vec3f trans(g_MeshesWorld[g_CurObjectIndex][0][3], g_MeshesWorld[g_CurObjectIndex][1][3], g_MeshesWorld[g_CurObjectIndex][2][3]);
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
				MatMatMult(&g_MeshesWorld[g_CurObjectIndex], &rot, &g_MeshesWorld[g_CurObjectIndex]);
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), trans);
				break;
			}

		case IKC_U:
			{
				gmtl::AxisAnglef rotAxis;
				rotAxis.setAxis( gmtl::Vec3f(1.0f, 0.0f, 0.0f) );
				rotAxis.setAngle( -1.0f * g_TimeDT );
				gmtl::MatrixA44f rot;
				gmtl::setRot(rot, rotAxis);

				gmtl::Vec3f trans(g_MeshesWorld[g_CurObjectIndex][0][3], g_MeshesWorld[g_CurObjectIndex][1][3], g_MeshesWorld[g_CurObjectIndex][2][3]);
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
				MatMatMult(&g_MeshesWorld[g_CurObjectIndex], &rot, &g_MeshesWorld[g_CurObjectIndex]);
				gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), trans);
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
					g_IsDebugRenderObjects = !g_IsDebugRenderObjects;
					break;
				}

			case IKC_2:
				{
					_LOOPi(NUM_MESHES)
					{
						// TODO: Only 1 mat for now
						IBFXMaterialGroupPtr pTemp = g_pBackupNoTexMat[i];
						g_pBackupNoTexMat[i] = g_pMeshes[i]->meshes[0].pMatGroup;
						g_pMeshes[i]->meshes[0].pMatGroup = pTemp;
					}
					break;
				}

			case IKC_4:
				{
					g_IsWireFrameMode = !g_IsWireFrameMode;
					g_pBaseFX->GetDebug().SetWireFrame(g_IsWireFrameMode);
					break;
				}
			case IKC_0:
				{
					g_CurObjectIndex = (g_CurObjectIndex + 1) % NUM_MESHES;
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

	// Update the vis tree
	gmtl::MatrixA44f viewProj;
	MatMatMult(&viewProj, &g_Proj, &g_View);

	float zNear = g_Proj[2][3] / g_Proj[2][2];
	float zFar = g_Proj[2][3] / (1.0f + g_Proj[2][2]);	

	_LOOPi(NUM_MESHES)
	{
		g_pVisTree[i]->UpdateCoverage(g_MeshesWorld[i]);
		g_pVisTree[i]->UpdateSphereLevel(g_MeshesWorld[i], 1);
		g_pVisTree[i]->UpdateScreenCoverage(viewProj, zNear, zFar);
	}
}