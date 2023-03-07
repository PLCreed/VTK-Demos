#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // Create an image
    vtkNew<vtkImageMandelbrotSource> source;
    source->Update();

    vtkNew<vtkImageCast> castFilter;
    castFilter->SetInputConnection(source->GetOutputPort());
    castFilter->SetOutputScalarTypeToUnsignedChar();
    castFilter->Update();

    // Create an actor
    vtkNew<vtkImageActor> actor1a;
    actor1a->GetMapper()->SetInputConnection(castFilter->GetOutputPort());
    vtkNew<vtkImageActor> actor1b;
    actor1b->GetMapper()->SetInputConnection(castFilter->GetOutputPort());

    vtkNew<vtkImageActor> actor2a;
    actor2a->GetMapper()->SetInputConnection(castFilter->GetOutputPort());

    vtkNew<vtkImageActor> actor2b;
    actor2b->GetMapper()->SetInputConnection(castFilter->GetOutputPort());

    // Create the transformation
    vtkNew<vtkTransform> transform1a;
    transform1a->PostMultiply();
    transform1a->Translate(10.0, 0.0, 0.0);
    actor1a->SetUserTransform(transform1a.Get());

    vtkNew<vtkTransform> transform1b;
    transform1b->PostMultiply();
    transform1b->Translate(10.0, 0.0, 0.0);
    transform1b->RotateZ(40.0);
    actor1b->SetUserTransform(transform1b.Get());

    vtkNew<vtkTransform> transform2a;
    transform2a->PostMultiply();
    transform2a->RotateZ(40.0);
    actor2a->SetUserTransform(transform2a.Get());

    vtkNew<vtkTransform> transform2b;
    transform2b->PostMultiply();
    transform2b->RotateZ(40.0);
    transform2b->Translate(10.0, 0.0, 0.0);
    actor2b->SetUserTransform(transform2b.Get());

    // Define viewport ranges
    // (xmin, ymin, xmax, ymax)
    double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
    double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

    // Cetup render window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(640, 640);
    renderWindow->SetWindowName("TransformOrderDemo");

    // Setup both renderers
    vtkNew<vtkRenderer> leftRenderer;
    renderWindow->AddRenderer(leftRenderer.Get());
    leftRenderer->SetViewport(leftViewport);
    leftRenderer->SetBackground(colors->GetColor3d("BurlyWood").GetData());

    vtkNew<vtkRenderer> rightRenderer;
    renderWindow->AddRenderer(rightRenderer.Get());
    rightRenderer->SetViewport(rightViewport);
    rightRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

    // an interactor
    vtkNew<vtkRenderWindowInteractor> interactor;

    interactor->SetRenderWindow(renderWindow.Get());

    vtkNew<vtkAxesActor> axes;
    axes->AxisLabelsOff();
    axes->SetTotalLength(5, 5, 5);

    vtkCamera *camera;
    camera = leftRenderer->GetActiveCamera();
    leftRenderer->AddActor(axes.Get());
    leftRenderer->AddActor(actor1a.Get());
    leftRenderer->AddActor(actor1b.Get());

    rightRenderer->AddActor(axes.Get());
    rightRenderer->AddActor(actor2a.Get());
    rightRenderer->AddActor(actor2b.Get());
    rightRenderer->SetActiveCamera(camera);

    leftRenderer->ResetCamera();
    // rightRenderer->ResetCamera();
    camera->Zoom(1.3);

    renderWindow->Render();

    interactor->Start();

    return EXIT_SUCCESS;
}
