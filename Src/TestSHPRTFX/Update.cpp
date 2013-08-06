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

	gmtl::setPerspective(g_Proj, gmtl::Math::PI / 3.0f, ((float)screenWidth) / screenHeight, NEAR_PLANE, FAR_PLANE);
}

void ProcessInput()
{
	g_pInput->Update();

	// Process mouse input
	gmtl::MatrixA44f cameraRotMat;
	gmtl::identity(_CAST_MAT44(cameraRotMat));

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

			// Rotate around up axis (yaw)
			gmtl::VecA3f upAxis;// = g_EyeUpVec;
			if(g_EyeUpVec[1] >= 0.0f)
				upAxis = gmtl::VecA3f(0.0f, 1.0f, 0.0f);
			else
				upAxis = gmtl::VecA3f(0.0f, -1.0f, 0.0f);

			gmtl::AxisAnglef upAxisRot;
			upAxisRot.setAxis(_CAST_VEC3(upAxis));
			upAxisRot.setAngle(mouseVec[0] * g_TimeDT * -10.0f);

			gmtl::MatrixA44f upAxisRotMat;
			gmtl::setRot(upAxisRotMat, upAxisRot);

			// Rotate around right axis (pitch)
			gmtl::VecA3f invEyeDir = -g_EyeDir;
			//TransformVec(&upAxisRotMat, &invEyeDir, &invEyeDir);

			gmtl::VecA3f rightVec;
			gmtl::cross(_CAST_VEC3(rightVec), _CAST_VEC3(upAxis), _CAST_VEC3(invEyeDir));
			gmtl::normalize(_CAST_VEC3(rightVec));

			gmtl::AxisAnglef rightAxisRot;
			rightAxisRot.setAxis(_CAST_VEC3(rightVec));
			rightAxisRot.setAngle(mouseVec[1] * g_TimeDT * -10.0f);

			gmtl::MatrixA44f rightAxisRotMat;
			gmtl::setRot(rightAxisRotMat, rightAxisRot);
	
			MatMatMult(&cameraRotMat, &upAxisRotMat, &rightAxisRotMat);
			TransformVec(&cameraRotMat, &g_EyeDir, &g_EyeDir);
			TransformVec(&cameraRotMat, &rightVec, &rightVec);
			gmtl::normalize(_CAST_VEC3(g_EyeDir));
			invEyeDir = -g_EyeDir;
			
			// Use cross product to derive up vector to ensure orthogonality
			gmtl::cross(_CAST_VEC3(g_EyeUpVec), _CAST_VEC3(invEyeDir), _CAST_VEC3(rightVec));
			gmtl::normalize(_CAST_VEC3(g_EyeUpVec));
		
		}
		else if(pActiveButtons[i] == IMC_LEFTBUTTON)
		{
			// Rotate camera

			// Get direction vector of mouse movement in screen space
			gmtl::VecA3f mouseVec;//gmtl::Vec3f(1.0f, 1.0f, 0.0f);
			_CAST_VEC3(mouseVec) = mouse.GetPosDelta();

			// Rotate around up axis (yaw)
			gmtl::VecA3f upAxis = g_AmbientY;
			gmtl::AxisAnglef upAxisRot;
			upAxisRot.setAxis(_CAST_VEC3(upAxis));
			upAxisRot.setAngle(mouseVec[0] * g_TimeDT * -10.0f);

			gmtl::MatrixA44f upAxisRotMat;
			gmtl::setRot(upAxisRotMat, upAxisRot);

			// Rotate around right axis (pitch)
			gmtl::VecA3f invZ = -g_AmbientZ;
			//TransformVec(&upAxisRotMat, &invEyeDir, &invEyeDir);

			gmtl::VecA3f rightVec;
			gmtl::cross(_CAST_VEC3(rightVec), _CAST_VEC3(upAxis), _CAST_VEC3(g_AmbientZ));
			gmtl::normalize(_CAST_VEC3(rightVec));

			gmtl::AxisAnglef rightAxisRot;
			rightAxisRot.setAxis(_CAST_VEC3(rightVec));
			rightAxisRot.setAngle(mouseVec[1] * g_TimeDT * -10.0f);

			gmtl::MatrixA44f rightAxisRotMat;
			gmtl::setRot(rightAxisRotMat, rightAxisRot);

			gmtl::MatrixA44f shRotMat;
			MatMatMult(&shRotMat, &upAxisRotMat, &rightAxisRotMat);
			TransformVec(&shRotMat, &g_AmbientZ, &g_AmbientZ);
			TransformVec(&shRotMat, &rightVec, &rightVec);
			gmtl::normalize(_CAST_VEC3(g_AmbientZ));
		
			// Use cross product to derive up vector to ensure orthogonality
			gmtl::cross(_CAST_VEC3(g_AmbientY), _CAST_VEC3(g_AmbientZ), _CAST_VEC3(rightVec));
			gmtl::normalize(_CAST_VEC3(g_AmbientY));

			//static float s_Rotate = 0.0f;

			//gmtl::AxisAnglef testRotate;
			//testRotate.setAxis(gmtl::Vec3f(0.0f, 0.0f, 1.0f));
			//testRotate.setAngle(s_Rotate);
			//s_Rotate += g_TimeDT;

			//gmtl::MatrixA44f rotMat;
			//gmtl::setRot(rotMat, testRotate);

			// SH D3D Rotation
			//D3DXMATRIX d3dRot;
			//d3dRot._11 = rightVec[0]; d3dRot._12 = rightVec[2]; d3dRot._13 = rightVec[1]; d3dRot._14 = 0.0f; 
			//d3dRot._21 = g_AmbientZ[0]; d3dRot._22 = g_AmbientZ[2]; d3dRot._23 = g_AmbientZ[1]; d3dRot._24 = 0.0f; 
			//d3dRot._31 = g_AmbientY[0]; d3dRot._32 = g_AmbientY[2]; d3dRot._33 = g_AmbientY[1]; d3dRot._34 = 0.0f; 
			//d3dRot._41 = 0.0f; d3dRot._42 = 0.0f; d3dRot._43 = 0.0f; d3dRot._44 = 1.0f; 

			//Matrix99f transfer;
			//float res[9];
			//float sh[9];
			//
			//sh[0] = g_AmbientCoeffs.rTerm1[0];
			//sh[1] = g_AmbientCoeffs.rTerm1[1];
			//sh[2] = g_AmbientCoeffs.rTerm1[2];
			//sh[3] = g_AmbientCoeffs.rTerm1[3];
			//sh[4] = g_AmbientCoeffs.rTerm2[0];
			//sh[5] = g_AmbientCoeffs.rTerm2[1];
			//sh[6] = g_AmbientCoeffs.rTerm2[2];
			//sh[7] = g_AmbientCoeffs.rTerm2[3];
			//sh[8] = g_AmbientCoeffs.rgbTerm[0];

			//D3DXSHRotate( res, 3, &d3dRot, sh );
			//EvaluateSHTripleProductTransfer3(transfer, g_TripleProductTable, res);
			//g_AmbientRed.Set(transfer);

			//sh[0] = g_AmbientCoeffs.gTerm1[0];
			//sh[1] = g_AmbientCoeffs.gTerm1[1];
			//sh[2] = g_AmbientCoeffs.gTerm1[2];
			//sh[3] = g_AmbientCoeffs.gTerm1[3];
			//sh[4] = g_AmbientCoeffs.gTerm2[0];
			//sh[5] = g_AmbientCoeffs.gTerm2[1];
			//sh[6] = g_AmbientCoeffs.gTerm2[2];
			//sh[7] = g_AmbientCoeffs.gTerm2[3];
			//sh[8] = g_AmbientCoeffs.rgbTerm[1];

			//D3DXSHRotate( res, 3, &d3dRot, sh );
			//EvaluateSHTripleProductTransfer3(transfer, g_TripleProductTable, res);
			//g_AmbientGreen.Set(transfer);

			//sh[0] = g_AmbientCoeffs.bTerm1[0];
			//sh[1] = g_AmbientCoeffs.bTerm1[1];
			//sh[2] = g_AmbientCoeffs.bTerm1[2];
			//sh[3] = g_AmbientCoeffs.bTerm1[3];
			//sh[4] = g_AmbientCoeffs.bTerm2[0];
			//sh[5] = g_AmbientCoeffs.bTerm2[1];
			//sh[6] = g_AmbientCoeffs.bTerm2[2];
			//sh[7] = g_AmbientCoeffs.bTerm2[3];
			//sh[8] = g_AmbientCoeffs.rgbTerm[2];

			//D3DXSHRotate( res, 3, &d3dRot, sh );
			//EvaluateSHTripleProductTransfer3(transfer, g_TripleProductTable, res);
			//g_AmbientBlue.Set(transfer);
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

		//case IKC_SPACE:
		//	{
		//		static float s_Rotate = 0.0f;

		//		gmtl::AxisAnglef testRotate;
		//		testRotate.setAxis(gmtl::Vec3f(0.0f, 0.0f, 1.0f));
		//		testRotate.setAngle(s_Rotate);
		//		s_Rotate += g_TimeDT;

		//		gmtl::MatrixA44f rotMat;
		//		gmtl::setRot(rotMat, testRotate);
		//		//rotMat.set(cameraRotMat[0][0], cameraRotMat[0][1], cameraRotMat[0][2], 0.0f,
		//		//	cameraRotMat[1][0], cameraRotMat[1][1], cameraRotMat[1][2], 0.0f,
		//		//	cameraRotMat[2][0], cameraRotMat[2][1], cameraRotMat[2][2], 0.0f,
		//		//	0.0f, 0.0f, 0.0f, 1.0f);

		//		//// Rotate objects if the camera matrix
		//		//_LOOPi(NUM_MESHES)
		//		//{
		//		//	//gmtl::Vec3f trans(g_MeshesWorld[i][0][3], g_MeshesWorld[i][1][3], g_MeshesWorld[i][2][3]);
		//		//	//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));

		//		//	gmtl::MatrixA44f invRotMat = rotMat;
		//		//	gmtl::invert(invRotMat);
		//		//	//TransformVec(&rotMat, &g_EyePos, &g_EyePos);
		//		//
		//		//	MatMatMult(&g_MeshesWorld[i], &rotMat, &g_MeshesWorld[i]);
		//		//	//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), trans);
		//		//}

		//		//gmtl::MatrixA44f invRotMat = rotMat;
		//		//gmtl::invert(invRotMat);
		//		//TransformVec(&rotMat, &g_EyePos, &g_EyePos);

		//		D3DXMATRIX d3dRot;
		//		d3dRot._11 = rotMat[0][0]; d3dRot._12 = rotMat[0][1]; d3dRot._13 = rotMat[0][2]; d3dRot._14 = rotMat[0][3]; 
		//		d3dRot._21 = rotMat[1][0]; d3dRot._22 = rotMat[1][1]; d3dRot._23 = rotMat[1][2]; d3dRot._24 = rotMat[1][3]; 
		//		d3dRot._31 = rotMat[2][0]; d3dRot._32 = rotMat[2][1]; d3dRot._33 = rotMat[2][2]; d3dRot._34 = rotMat[2][3]; 
		//		d3dRot._41 = rotMat[3][0]; d3dRot._42 = rotMat[3][1]; d3dRot._43 = rotMat[3][2]; d3dRot._44 = rotMat[3][3]; 

		//		Matrix99f transfer;
		//		float res[9];
		//		float sh[9];
		//		//g_pAmbientFX->GetSHToolKit().CreateHemiLobeCoeffs(unitSH, 128, 3);
		//		sh[0] = g_AmbientCoeffs.rTerm1[0];
		//		sh[1] = g_AmbientCoeffs.rTerm1[1];
		//		sh[2] = g_AmbientCoeffs.rTerm1[2];
		//		sh[3] = g_AmbientCoeffs.rTerm1[3];
		//		sh[4] = g_AmbientCoeffs.rTerm2[0];
		//		sh[5] = g_AmbientCoeffs.rTerm2[1];
		//		sh[6] = g_AmbientCoeffs.rTerm2[2];
		//		sh[7] = g_AmbientCoeffs.rTerm2[3];
		//		sh[8] = g_AmbientCoeffs.rgbTerm[0];

		//		D3DXSHRotate( res, 3, &d3dRot, sh );
		//		EvaluateSHTripleProductTransfer3(transfer, g_TripleProductTable, res);
		//		g_AmbientRed.Set(transfer);

		//		sh[0] = g_AmbientCoeffs.gTerm1[0];
		//		sh[1] = g_AmbientCoeffs.gTerm1[1];
		//		sh[2] = g_AmbientCoeffs.gTerm1[2];
		//		sh[3] = g_AmbientCoeffs.gTerm1[3];
		//		sh[4] = g_AmbientCoeffs.gTerm2[0];
		//		sh[5] = g_AmbientCoeffs.gTerm2[1];
		//		sh[6] = g_AmbientCoeffs.gTerm2[2];
		//		sh[7] = g_AmbientCoeffs.gTerm2[3];
		//		sh[8] = g_AmbientCoeffs.rgbTerm[1];

		//		D3DXSHRotate( res, 3, &d3dRot, sh );
		//		EvaluateSHTripleProductTransfer3(transfer, g_TripleProductTable, res);
		//		g_AmbientGreen.Set(transfer);

		//		sh[0] = g_AmbientCoeffs.bTerm1[0];
		//		sh[1] = g_AmbientCoeffs.bTerm1[1];
		//		sh[2] = g_AmbientCoeffs.bTerm1[2];
		//		sh[3] = g_AmbientCoeffs.bTerm1[3];
		//		sh[4] = g_AmbientCoeffs.bTerm2[0];
		//		sh[5] = g_AmbientCoeffs.bTerm2[1];
		//		sh[6] = g_AmbientCoeffs.bTerm2[2];
		//		sh[7] = g_AmbientCoeffs.bTerm2[3];
		//		sh[8] = g_AmbientCoeffs.rgbTerm[2];

		//		D3DXSHRotate( res, 3, &d3dRot, sh );
		//		EvaluateSHTripleProductTransfer3(transfer, g_TripleProductTable, res);
		//		g_AmbientBlue.Set(transfer);

		//		break;
		//	}

		//case IKC_I:
		//	{
		//		gmtl::VecA4f trans;
		//		trans[3] = 0.0f;
		//		gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
		//		gmtl::VecA4f diff;
		//		diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
		//		diff[1] += g_TimeDT * 3.1f;
		//		trans[1] += g_TimeDT * 3.1f;
		//		gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
		//		//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
		//		break; 
		//	}

		//case IKC_J:
		//	{
		//		gmtl::VecA4f trans;
		//		trans[3] = 0.0f;
		//		gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
		//		gmtl::VecA4f diff;
		//		diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
		//		diff[0] -= g_TimeDT * 10.1f;
		//		trans[0] -= g_TimeDT * 10.1f;
		//		gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
		//		//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
		//		break;
		//	}

		//case IKC_L:
		//	{
		//		gmtl::VecA4f trans;
		//		trans[3] = 0.0f;
		//		gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
		//		gmtl::VecA4f diff;
		//		diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
		//		diff[0] += g_TimeDT * 10.1f;
		//		trans[0] += g_TimeDT * 10.1f;
		//		gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
		//		//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
		//		break;
		//	}

		//case IKC_K:
		//	{
		//		gmtl::VecA4f trans;
		//		trans[3] = 0.0f;
		//		gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
		//		gmtl::VecA4f diff;
		//		diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
		//		diff[1] -= g_TimeDT * 3.1f;
		//		trans[1] -= g_TimeDT * 3.1f;
		//		gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
		//		//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
		//		break;
		//	}

		//case IKC_M:
		//	{
		//		gmtl::VecA4f trans;
		//		trans[3] = 0.0f;
		//		gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
		//		gmtl::VecA4f diff;
		//		diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
		//		diff[2] += g_TimeDT * 10.1f;
		//		trans[2] += g_TimeDT * 10.1f;
		//		gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
		//		//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
		//		break;
		//	}

		//case IKC_N:
		//	{
		//		gmtl::VecA4f trans;
		//		trans[3] = 0.0f;
		//		gmtl::setTrans(_CAST_VEC3(trans), _CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]));
		//		gmtl::VecA4f diff;
		//		diff[0] = diff[1] = diff[2] = diff[3] = 0.0f;
		//		diff[2] -= g_TimeDT * 10.1f;
		//		trans[2] -= g_TimeDT * 10.1f;
		//		gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[g_CurObjectIndex]), _CAST_VEC3(trans));
		//		//UpdateSphereTree(g_pSphereTrees[0], diff, g_pSphereTrees[0]->GetNumLevels()-1);
		//		break;
		//	}

		case IKC_O:
			{
				gmtl::AxisAnglef rotAxis;
				rotAxis.setAxis( gmtl::Vec3f(0.0f, 1.0f, 0.0f) );
				rotAxis.setAngle( 1.0f * g_TimeDT );
				gmtl::MatrixA44f rot;
				gmtl::setRot(rot, rotAxis);

				_LOOPi(NUM_MESHES)
				{
					//gmtl::Vec3f trans(g_MeshesWorld[i][0][3], g_MeshesWorld[i][1][3], g_MeshesWorld[i][2][3]);
					//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
					MatMatMult(&g_MeshesWorld[i], &rot, &g_MeshesWorld[i]);
					//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), trans);
				}
				break;
			}

		case IKC_P:
			{
				gmtl::AxisAnglef rotAxis;
				rotAxis.setAxis( gmtl::Vec3f(0.0f, 1.0f, 0.0f) );
				rotAxis.setAngle( -1.0f * g_TimeDT );
				gmtl::MatrixA44f rot;
				gmtl::setRot(rot, rotAxis);

				_LOOPi(NUM_MESHES)
				{
					//gmtl::Vec3f trans(g_MeshesWorld[i][0][3], g_MeshesWorld[i][1][3], g_MeshesWorld[i][2][3]);
					//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
					MatMatMult(&g_MeshesWorld[i], &rot, &g_MeshesWorld[i]);
					//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), trans);
				}
				break;
			}

		case IKC_Y:
			{
				gmtl::AxisAnglef rotAxis;
				rotAxis.setAxis( gmtl::Vec3f(1.0f, 0.0f, 0.0f) );
				rotAxis.setAngle( 1.0f * g_TimeDT );
				gmtl::MatrixA44f rot;
				gmtl::setRot(rot, rotAxis);

				_LOOPi(NUM_MESHES)
				{
					//gmtl::Vec3f trans(g_MeshesWorld[i][0][3], g_MeshesWorld[i][1][3], g_MeshesWorld[i][2][3]);
					//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
					MatMatMult(&g_MeshesWorld[i], &rot, &g_MeshesWorld[i]);
					//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), trans);
				}
				break;
			}

		case IKC_U:
			{
				gmtl::AxisAnglef rotAxis;
				rotAxis.setAxis( gmtl::Vec3f(1.0f, 0.0f, 0.0f) );
				rotAxis.setAngle( -1.0f * g_TimeDT );
				gmtl::MatrixA44f rot;
				gmtl::setRot(rot, rotAxis);

				_LOOPi(NUM_MESHES)
				{
					//gmtl::Vec3f trans(g_MeshesWorld[i][0][3], g_MeshesWorld[i][1][3], g_MeshesWorld[i][2][3]);
					//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
					MatMatMult(&g_MeshesWorld[i], &rot, &g_MeshesWorld[i]);
					//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[i]), trans);
				}
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
					break;
				}

			case IKC_4:
				{
					g_IsWireFrameMode = !g_IsWireFrameMode;
					//g_pBaseFX->GetDebug().SetWireFrame(g_IsWireFrameMode);
					break;
				}
			//case IKC_0:
			//	{
			//		g_CurObjectIndex = (g_CurObjectIndex + 1) % NUM_MESHES;
			//		break;
			//	}


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
	
	// Move point light
	float amount = gmtl::Math::sin((float) g_pPlatform->GetTimer().GetTime() * 1.0f);

	g_PointLight.pos = _CAST_VEC3(g_PointLightPos) + amount * gmtl::Vec3f(30.0f, 0.0f, 0.0f);
}