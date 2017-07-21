#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QTimer>
#include <iostream>


#include "itkImage.h"
#include <itkImageFileWriter.h>
#include <itkImportImageFilter.h>
#include <itkImageFileReader.h>
#include <itkCastImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkVTKImageIO.h>
//#include <Q4VTKWidgetPlugin.h>
#include <vtkPNGReader.h>
#include <vtkImageViewer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkCamera.h>
#include <vtkImageFlip.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include "edge_detection_filter.h"

//#include 
namespace Ui{
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget * parent = NULL);
	~MainWindow();

protected slots:
	void choose_input_file();
	void choose_output_file();
	void save_output();
	void update_variance();
	void toggle_auto_threshold();
	void update_upper_thresh();
	void update_lower_thresh();
	void timeout();
	//todo: quit
private:
	Ui::MainWindow *ui;

	//todo: controller class ?
	//itk type definitions
	typedef float InputPixelType; 
    typedef unsigned char OutputPixelType;
    typedef itk::Image<InputPixelType , 2> InputImageType;
    typedef itk::Image<OutputPixelType , 2> OutputImageType;
    typedef itk::ImageFileReader<InputImageType> ImageReaderType;
    typedef itk::ImageFileWriter<OutputImageType> ImageWriterType;
    typedef EdgeDetectionFilter<InputImageType> FilterType;
    typedef itk::CastImageFilter<   itk::Image<InputPixelType,2>,
                                    itk::Image<OutputPixelType,2> >
                                    CastFilterType;
    typedef itk::ImageToVTKImageFilter <InputImageType> ITKToVTKFilterType;                                 

    ImageReaderType::Pointer m_reader;
    ImageWriterType::Pointer m_writer;
    CastFilterType::Pointer m_cast_filter;
	ITKToVTKFilterType::Pointer m_itk_to_vtk_filter;
    FilterType::Pointer m_filter;

    vtkImageFlip *imageFlip;
 //   QVTKWidget * m_VTKwidget;

    QTimer *m_timer;

    bool m_input_set;
    bool m_output_set;
    bool m_timer_timed_out;

    void updateVTKWidget();

};



#endif