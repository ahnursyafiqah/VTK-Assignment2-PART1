
// Modified from a modified version of the file described in the following comment...
//    -Chris

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: Medical1.cxx,v $
  Language:  C++
  Date:      $Date: 2002/11/27 16:06:38 $
  Version:   $Revision: 1.2 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolume16Reader.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkActor.h"
#include "vtkOutlineFilter.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkPolyDataNormals.h"
#include "vtkContourFilter.h"
#include "vtkStructuredPointsReader.h"
#include "vtkMarchingCubes.h"
#include "vtkRecursiveDividingCubes.h"
#include "vtkScalarBarWidget.h"
#include "vtkScalarBarActor.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkCallbackCommand.h"
#include "vtkObject.h"
#include "vtkSmartPointer.h"
//#include "vtkVolumeRayCastCompositeFunction.h"
//#include "vtkVolumeRayCastMapper.h"
//#include "vtkVolumeRayCastMIPFunction.h"'

double rayStepSize = 1.0;

void KeyboardCallback(vtkObject* obj, unsigned long eventId, void* clientData, void* callData)
{
	// Check if the event is a key press event
	if (eventId == vtkCommand::KeyPressEvent)
	{
		vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(obj);
		std::string key = interactor->GetKeySym();

		// Increase or decrease the ray step size based on key press
		if (key == "Up")
			rayStepSize += 0.1;
		else if (key == "Down")
			rayStepSize -= 0.1;

		// Update the ray step size in the volume mapper
		vtkSmartVolumeMapper* volumeMapper = static_cast<vtkSmartVolumeMapper*>(clientData);
		volumeMapper->SetSampleDistance(rayStepSize);

		// Render the scene
		interactor->GetRenderWindow()->Render();
	}
}



int main(int argc, char** argv)
{

	// Create the callback command
	vtkSmartPointer<vtkCallbackCommand> keyboardCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	keyboardCallback->SetCallback(KeyboardCallback);
	vtkSmartPointer<vtkSmartVolumeMapper> SmartvolumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	keyboardCallback->SetClientData(SmartvolumeMapper.GetPointer());  // Pass the volume mapper as client data

	// Add the callback command to the interactor
	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
	iren->AddObserver(vtkCommand::KeyPressEvent, keyboardCallback);


	// Create the renderer, the render window, and the interactor. The renderer
	// draws into the render window, the interactor enables mouse- and 
	// keyboard-based interaction with the data within the render window.
	vtkRenderer* aRenderer = vtkRenderer::New();
	vtkRenderWindow* renWin = vtkRenderWindow::New();
	renWin->AddRenderer(aRenderer);
	//vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);


	// vtkVolumeReader16 reads in the head CT data set.  
	vtkVolume16Reader* reader = vtkVolume16Reader::New();
	reader->SetDataDimensions(256, 256);
	//reader->SetImageRange (1,93);
	reader->SetImageRange(1, 256);
	//reader->SetImageRange(1, 178);
	reader->SetDataByteOrderToLittleEndian();
	reader->SetFilePrefix("../data/foot/foot");
	//reader->SetFilePrefix("../data/teapot/teapot");
	//reader->SetFilePrefix("../data/headsq/quarter");
	//reader->SetFilePrefix("../data/aneurism/aneurism");
	//reader->SetFilePrefix("../data/teapot/teapot");
	//reader->SetDataSpacing (3.2, 3.2, 1.5);
	//reader->SetDataSpacing(1, 1, 2.2);
	reader->SetDataSpacing(1, 1, 1);


	// This part creates the colorMap function to the volume rendering.
	vtkPiecewiseFunction* opacityTransferFunction = vtkPiecewiseFunction::New();
	opacityTransferFunction->AddPoint(20, 0.0);
	opacityTransferFunction->AddPoint(40, 0.0);
	opacityTransferFunction->AddPoint(60, 0.3);
	opacityTransferFunction->AddPoint(80,0.5);
	opacityTransferFunction->AddPoint(100, 0.9);

	/*opacityTransferFunction->AddPoint(20, 0.0);
	opacityTransferFunction->AddPoint(40, 0.0);
	opacityTransferFunction->AddPoint(50, 0.3);
	opacityTransferFunction->AddPoint(80, 0.5);
	opacityTransferFunction->AddPoint(100, 0.9);*/


	vtkColorTransferFunction* colorTransferFunction = vtkColorTransferFunction::New();
	colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	colorTransferFunction->AddRGBPoint(0, 255, 0, 0);
	colorTransferFunction->AddRGBPoint(700.0, 1.0, 0.6, 0.0);
	colorTransferFunction->AddRGBPoint(800.0, 1.0, 0.0, 0.0);
	colorTransferFunction->AddRGBPoint(1150.0, 0.9, 0.9, 0.9);

	// The property describes how the data will look
	vtkVolumeProperty* volumeProperty = vtkVolumeProperty::New();
	volumeProperty->SetColor(colorTransferFunction);
	volumeProperty->SetScalarOpacity(opacityTransferFunction);
	volumeProperty->ShadeOn();
	volumeProperty->SetInterpolationTypeToLinear();

	// vtkVolumeRay Functions
	  //vtkVolumeRayCastCompositeFunction* compositeFunction = vtkVolumeRayCastCompositeFunction::New();
	  //vtkVolumeRayCastMIPFunction* compositeFunction = vtkVolumeRayCastMIPFunction::New();

	  //vtkVolumeRayCastMapper* volumeMapper = vtkVolumeRayCastMapper::New();
	  //volumeMapper->SetVolumeRayCastFunction(compositeFunction);
	  //volumeMapper->SetInputData(reader->GetOutput());
	  //vtkNew<vtkStructuredPointsReader>reader;
	  //reader->SetFileName();
	  //reader->Update();

	vtkNew<vtkSmartVolumeMapper> volumeMapper;
	volumeMapper->SetInputConnection(reader->GetOutputPort());


	// Set the sampling rate
	volumeMapper->SetSampleDistance(1.0);



	// The volume holds the mapper and the property and
	  // can be used to position/orient the volume
	vtkVolume* volume = vtkVolume::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	// An outline provides context around the data.
	vtkOutlineFilter* outlineData = vtkOutlineFilter::New();
	outlineData->SetInputData((vtkDataSet*)reader->GetOutput());
	vtkPolyDataMapper* mapOutline = vtkPolyDataMapper::New();
	mapOutline->SetInputData(outlineData->GetOutput());
	vtkActor* outline = vtkActor::New();
	outline->SetMapper(mapOutline);
	outline->GetProperty()->SetColor(0, 0, 0);

	// It is convenient to create an initial view of the data. The FocalPoint
	// and Position form a vector direction. Later on (ResetCamera() method)
	// this vector is used to position the camera to look at the data in
	// this direction.
	vtkCamera* aCamera = vtkCamera::New();
	aCamera->SetViewUp(0, 0, -1);
	aCamera->SetPosition(0, 1, 0);
	aCamera->SetFocalPoint(0, 0, 0);
	aCamera->ComputeViewPlaneNormal();

	// Actors are added to the renderer. An initial camera view is created.
	// The Dolly() method moves the camera towards the FocalPoint,
	// thereby enlarging the image.
	aRenderer->AddActor(outline);
	aRenderer->AddVolume(volume);
	aRenderer->SetActiveCamera(aCamera);
	aRenderer->ResetCamera();
	aCamera->Dolly(1.5);

	// Set a background color for the renderer and set the size of the
	// render window (expressed in pixels).
	aRenderer->SetBackground(1, 1, 1);
	renWin->SetSize(800, 600);

	// Note that when camera movement occurs (as it does in the Dolly()
	// method), the clipping planes often need adjusting. Clipping planes
	// consist of two planes: near and far along the view direction. The 
	// near plane clips out objects in front of the plane; the far plane
	// clips out objects behind the plane. This way only what is drawn
	// between the planes is actually rendered.
	aRenderer->ResetCameraClippingRange();


	vtkScalarBarWidget* scalarWidget = vtkScalarBarWidget::New();
	scalarWidget->SetInteractor(iren);
	scalarWidget->GetScalarBarActor()->SetTitle("Transfer Function");
	scalarWidget->GetScalarBarActor()->SetLookupTable(colorTransferFunction);


	// Initialize the event loop and then start it.
	iren->Initialize();
	renWin->SetWindowName("Simple Volume Renderer");
	renWin->Render();
	scalarWidget->EnabledOn();
	iren->Start();

	return 0;
}