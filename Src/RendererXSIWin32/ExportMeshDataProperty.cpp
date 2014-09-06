//==============================================================================
//
//		Properties.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/17/2007
//
//==============================================================================

#include "stdafx.h"

XSI::CStatus
RegisterExportMeshDataProperty(XSI::PluginRegistrar& in_reg)
{
	in_reg.RegisterProperty(_W("RPropExportMeshData"));
//	in_reg.RegisterProperty(_W("RPropExportEffect"));

	return XSI::CStatus::OK;
}

XSI::CStatus
ExportMeshes()
{
	InitializeFileSystem();
	
	// Export all valid meshes in the scene
	XSI::Application app;
	XSI::Model root = app.GetActiveSceneRoot();

	// Get all meshes
	XSI::CRefArray meshesToExport;
	GetAllObjectsWithPolygonMeshes(meshesToExport, root);

	XSI::CustomProperty prop = root.GetProperties().GetItem(_W("RPropExportMeshData"));

	// Set vertex desc
	uint curIndex = 3;
	RVertexDesc desc[256] =
	{
		{ VDU_POSITION, 0 },
		{ VDU_NORMAL, 0 },
		{ VDU_TEXCOORDF2, 0 },
	};

	XSI::CValue requireTangents = prop.GetParameterValue(_W("RParamRequireTangents"));
	if(requireTangents)
	{
		desc[curIndex].usage = VDU_TANGENT;
		desc[curIndex++].usageIndex = 0;

		desc[curIndex].usage = VDU_BINORMAL;
		desc[curIndex++].usageIndex = 0;
	}

	desc[curIndex].usage = VDU_END;
	desc[curIndex++].usageIndex = 0;

	XSI::CString meshOutDir = prop.GetParameterValue(_W("RParamMeshOutDir"));
	meshOutDir = GetRelativePath(meshOutDir, GetApplicationRoot());

	long numMeshes = (long) (meshesToExport.GetCount());
	_LOOPi(numMeshes)
	{
		XSI::X3DObject curObj = meshesToExport[i];

		XSI::CValue retVal;
		XSI::CValueArray args(2);
		//args[0] = meshOutDir;
		//args[1] = curObj.GetName();
		args[0] = (XSI::CValue::siPtrType)(desc);
		args[1] = curObj;

		XSI::CStatus status = app.ExecuteCommand(_W("RComExportMeshData"), args, retVal);
		status.AssertSucceeded(curObj.GetName() + _W(" --- export failed!"));

		if(status == XSI::CStatus::OK)
		{
			XSI::CValueArray meshData = retVal;
			IByteBufferPtr pVBGroupData = (IByteBuffer*) ((XSI::CValue::siPtrType) meshData[0]);
			IByteBufferPtr pIBData = (IByteBuffer*) ((XSI::CValue::siPtrType) meshData[1]);

			wstring fileName = meshOutDir.GetWideString();
			fileName += _W("\\");
			fileName += curObj.GetName().GetWideString();
			wstring vbGroupStr = fileName;
			vbGroupStr += _W(".vbg");
			wstring ibStr = fileName;
			ibStr += _W(".ib");

			IFFilePtr pVBGroupFile = g_pFileSystem->GetFile(vbGroupStr.c_str());
			pVBGroupFile->Open(FO_WRITE_ONLY);
			pVBGroupFile->Write(pVBGroupData->GetData(), pVBGroupData->GetDataLength());
			pVBGroupFile->Close();

			IFFilePtr pIBFile = g_pFileSystem->GetFile(ibStr.c_str());
			pIBFile->Open(FO_WRITE_ONLY);
			pIBFile->Write(pIBData->GetData(), pIBData->GetDataLength());
			pIBFile->Close();
		}		
	}
	//
	//

	//// A 3d object with a mesh geometry must be selected
	//XSI::X3DObject xobj(app.GetSelection()[0]);
	//if (!xobj.IsValid()) 
	//	return XSI::CStatus::Fail;

	//// Get a geometry accessor from the selected object	
	//XSI::Primitive prim = xobj.GetActivePrimitive();
	//XSI::PolygonMesh mesh = prim.GetGeometry();
	//if (!mesh.IsValid()) 
	//	return XSI::CStatus::Fail;

	//args[3] = xobj;


	return XSI::CStatus::OK;
}


XSIPLUGINCALLBACK XSI::CStatus
RPropExportMeshData_Define(XSI::CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);

	XSI::CustomProperty prop = ctxt.GetSource();
	XSI::Parameter param;

	prop.AddParameter(_W("RParamMeshOutDir"), 
		XSI::CValue::siString, 
		XSI::siPersistable,
		_W("Export Directory"),
		_W("The normalized output directory path of the mesh data"),
		GetApplicationRoot(),
		param);

	prop.AddParameter(_W("RParamRequireTangents"), 
		XSI::CValue::siBool, 
		XSI::siPersistable,
		_W("Require tangent and binormals?"),
		_W("Select to export with tangent the binormal data for each vertex"),
		true,
		param);

	return XSI::CStatus::OK;	
}

XSIPLUGINCALLBACK XSI::CStatus
RPropExportMeshData_UpdateLayout(XSI::CRef& in_ctxt)
{
	XSI::Context ctxt(in_ctxt);

	XSI::PPGLayout layout = ctxt.GetSource();
	layout.Clear() ;

	XSI::PPGItem item;

	layout.AddGroup(_W("Parameters"));
	item = layout.AddItem(_W("RParamMeshOutDir"), _W("Export Directory"), XSI::siControlFolder);
	item.PutAttribute(XSI::siUIInitialDir, GetApplicationRoot());
	//item.PutAttribute(XSI::siUISubFolder, _W("data\\meshes"));

	//item.PutAttribute(XSI::siUICX, 365L);

	item = layout.AddItem(_W("RParamRequireTangents"), _W("Require tangent and binormals?"), XSI::siControlCheck);
	//item.PutAttribute(XSI::siUICX, 365L);

	layout.EndGroup();
	layout.AddSpacer(1, 2);
	item = layout.AddButton(_W("RButtonExportMeshData"), _W("Export"));
	item.PutAttribute(XSI::siUICX, 365L);

	return XSI::CStatus::OK;
}

XSIPLUGINCALLBACK XSI::CStatus
RPropExportMeshData_DefineLayout(XSI::CRef& in_ctxt)
{
	return RPropExportMeshData_UpdateLayout(in_ctxt);
}

XSIPLUGINCALLBACK XSI::CStatus
RPropExportMeshData_PPGEvent(XSI::CRef& in_ctxt)
{
	XSI::PPGEventContext ctxt(in_ctxt);
	XSI::PPGEventContext::PPGEvent eventID = ctxt.GetEventID();

	XSI::Application app;
	//XSI::UIToolkit kit = app.GetUIToolkit();
	XSI::CStatus status = XSI::CStatus::OK;

	if(eventID == XSI::PPGEventContext::siOnInit)
	{
		// This event meant that the UI was just created.
		// It gives us a chance to set some parameter values.
		// We could even change the layout completely at this point.

		// For this event Source() of the event is the CustomProperty object

		XSI::CustomProperty prop = ctxt.GetSource() ;

		app.LogMessage( L"OnInit called for " + prop.GetFullName() ) ;

		// If you regenerate the layout then call this:
		//ctxt.PutAttribute(L"Refresh", true);

	}
	else 
	if(eventID == XSI::PPGEventContext::siButtonClicked)
	{
		// If there are multiple buttons 
		// we can use this attribute to figure out which one was clicked.
		XSI::CValue buttonPressed = ctxt.GetAttribute( L"Button" ) ;
		app.LogMessage( L"Button pressed: " + buttonPressed.GetAsText() ) ;

		if(buttonPressed.GetAsText() == _W("RButtonExportMeshData"))
		{
			status = ExportMeshes();
		}
	}
	else 
	if(eventID == XSI::PPGEventContext::siTabChange)
	{
		// We will be called when the PPG is first opened
		// and every time the tab changes

		// Retrieve the label of the tab that is now active
		XSI::CValue tabLabel = ctxt.GetAttribute( L"Tab" ) ;

		app.LogMessage( L"Tab changed to: " + tabLabel .GetAsText() ) ;
	}
	else 
	if(eventID == XSI::PPGEventContext::siParameterChange)
	{
		// For this event the Source of the event is the parameter
		// itself
		XSI::Parameter changed = ctxt.GetSource() ;
		XSI::CustomProperty prop = changed.GetParent() ;
		XSI::CString   paramName = changed.GetScriptName() ; 

		app.LogMessage( L"Parameter Changed: " + paramName ) ;
	}

	return status;
}
