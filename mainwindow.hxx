#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <iostream>


#include "itkImage.h"
#include <itkImageFileWriter.h>
#include <itkImportImageFilter.h>
#include <itkImageFileReader.h>
#include <itkCastImageFilter.h>
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

    ImageReaderType::Pointer m_reader;
    ImageWriterType::Pointer m_writer;
    CastFilterType::Pointer m_cast_filter;
    FilterType::Pointer m_filter;

    bool m_input_set;
    bool m_output_set;


};



#endif