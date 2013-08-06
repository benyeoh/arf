//==============================================================================
//
//		BaseFXMenu.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/10/2008
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
RegisterBaseFXMenu(XSI::PluginRegistrar& in_reg)
{
	XSI::PluginItem menu = in_reg.RegisterMenu(XSI::siMenuTbGetMaterialID, _W("BaseFX"), false, false);
	menu = in_reg.RegisterMenu(XSI::siMenuMainFileExportID, _W("ExportBFXUMeshList"), false, false);

	return XSI::CStatus::OK;
	//XSI::PluginItem menu2 = in_reg.RegisterMenu(XSI::siMenuMainTopLevelID, _THE_PROJECT_LABEL, false, false);
	//
	//if(menu == menu2)
	//{
	//	XSI::Application app;
	//	app.LogMessage(_W("Cheebye!!!"), XSI::siFatalMsg);
	//}
}

XSIPLUGINCALLBACK 
XSI::CStatus BaseFX_Init( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );

	XSI::Menu oMenu;
	oMenu = ctxt.GetSource();		

	XSI::MenuItem menuItem;
	oMenu.AddCallbackItem(_W("BaseFX Material Group"), _W("OnBaseFXMaterialClicked"), menuItem);

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK
XSI::CStatus OnBaseFXMaterialClicked(XSI::CRef& in_ref)
{
	XSI::Application app;
	XSI::Model root = app.GetActiveSceneRoot();
	//XSI::CustomProperty prop = root.GetProperties().GetItem(_W("Create_RenderFX_Material"));
	XSI::Scene scene = app.GetActiveProject().GetActiveScene();
	XSI::MaterialLibrary matLib = scene.GetActiveMaterialLibrary();
	XSI::CRefArray materials = matLib.GetItems();
	XSI::Material curMat;

	//_LOOPi( materials.GetCount() )
	//{
	//	XSI::Material mat( materials[i] );

	//	mat.PutSelected(false); //== _W("RenderFX_Effect_Group"))
	//	//{
	//	//	curMat = mat;
	//	//	break;
	//	//}
	//}
	
	//if(!curMat.IsValid())
	//{
	curMat = matLib.CreateMaterial(_W("DXDraw"), _W(""));
	curMat.PutName(_W("BaseFX_Material_Group"));
	
	XSI::CustomProperty prop = curMat.AddProperty(_W("BFXPropCreateBaseFXMaterial"));
	prop.PutName(_W("BaseFX_Material_Group_Property"));
	ShowProperty(prop, _W("BaseFX_Material_Group Property"));
	
	XSI::CRef prevSrc, newSrc;
	XSI::Shader dxDraw = curMat.GetShaders().GetItem(_W("DXDraw"));
	XSI::Parameter prevParam = dxDraw.GetParameter(_W("previous"));
	
	prevParam.ConnectFromPreset(_W("DXShade"), XSI::siRealTimeShaderFamily, prevSrc, newSrc);

	// connect texture node
	XSI::Shader dxShade = newSrc;
	prevParam = dxShade.GetParameter(_W("previous"));
	prevParam.ConnectFromPreset(_W("DXTexture"), XSI::siRealTimeShaderFamily, prevSrc, newSrc);//DXTextureFile

	// set parameters
	dxShade.PutParameterValue(_W("blending")           , true);
	dxShade.PutParameterValue(_W("srcblendingfunction"), 4L);
	dxShade.PutParameterValue(_W("dstblendingfunction"), 5L);

	dxShade.PutParameterValue(_W("Enable_Specular"), false);
	dxShade.PutColorParameterValue(_W("Ambient"), 1.0f, 1.0f, 1.0f);
	dxShade.PutColorParameterValue(_W("Diffuse"), 1.0f, 1.0f, 1.0f);
	
	// Assign to object
	XSI::Selection selected = app.GetSelection();
	_LOOPi( selected.GetCount() )
	{
		XSI::X3DObject obj = selected[i];
		if (obj.IsValid())
		{
			obj.SetMaterial(curMat);
		}
	}
	//else
	//{
	//	XSI::CustomProperty prop;
	//	XSI::CRefArray properties = curMat.GetProperties();
	//	_LOOPi( properties.GetCount() )
	//	{
	//		prop = properties[i];
	//		if(prop.GetName() == _W("RenderFX_Effect_Group_Property"))
	//			break;
	//	}
	//	 
	//	ShowProperty(prop, _W("RenderFX Effect Group Property"), XSI::siFollow);
	//}
	//
	//curMat.PutSelected( true );
	
	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK 
XSI::CStatus ExportBFXUMeshList_Init( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );

	XSI::Menu oMenu;
	oMenu = ctxt.GetSource();		

	XSI::MenuItem menuItem;
	oMenu.AddCallbackItem(_W("ARF BaseFX Mesh List"), _W("OnExportBFXUMeshListClicked"), menuItem);

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK
XSI::CStatus OnExportBFXUMeshListClicked(XSI::CRef& in_ref)
{
	XSI::Application app;
	XSI::Model root = app.GetActiveSceneRoot();
	XSI::CustomProperty prop = root.GetProperties().GetItem(_W("BFXPropExportBFXUMeshList"));
	
	if(!prop.IsValid())
	{
		prop = root.AddProperty(_W("BFXPropExportBFXUMeshList"));
	}

	ShowProperty(prop, _W("Export BaseFX Mesh List"));

	return XSI::CStatus::OK;
}