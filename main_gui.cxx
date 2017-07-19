#include "itkImage.h"
#include <itkImageFileWriter.h>
#include <itkImportImageFilter.h>
#include <itkImageFileReader.h>
#include "itkCastImageFilter.h"
#include "edge_detection_filter.h"

#include <iostream>
#include <QApplication>
#include "mainwindow.hxx"



int main(int argc, char **argv)
{



    QApplication app(argc, argv);
    app.setApplicationName("Edge Detection Filter GUI");

    MainWindow win;
    win.show();

    return app.exec();



    //constants definitions
    const int dimension = 2;

    //type definitions
    //typedef float PixelType;
    typedef float InputPixelType; 
    typedef unsigned char OutputPixelType;

    typedef itk::Image<InputPixelType , dimension> InputImageType;
    typedef itk::Image<OutputPixelType , dimension> OutputImageType;
    typedef itk::ImageFileReader<InputImageType> ImageReaderType;
    typedef itk::ImageFileWriter<OutputImageType> ImageWriterType;
    
    typedef EdgeDetectionFilter<InputImageType> FilterType;
    typedef itk::CastImageFilter<   itk::Image<InputPixelType,dimension>,
                                    itk::Image<OutputPixelType,dimension> >
                                    CastFilterType;

    //allocations
    ImageReaderType::Pointer reader = ImageReaderType::New();
    FilterType::Pointer filter = FilterType::New();
    CastFilterType::Pointer cast_filter = CastFilterType::New();
    ImageWriterType::Pointer writer = ImageWriterType::New();

    //reader configuration
    reader->SetFileName("test");

    //filter configuration (output values and thresholds)
    filter->SetInput(reader->GetOutput());
    filter->SetAutomaticThresholdingOn();
    filter->SetVariance(1.5);
    //convert to uchar for the writer
    cast_filter->SetInput(filter->GetOutput());

    //writer configuration
    writer->SetFileName("test");
    writer->SetInput(cast_filter->GetOutput());

    //starting the process and writing the output to the disk
    try {
        writer->Update();
    }catch(itk::ExceptionObject & err)
    {
        std::cerr << "Exception Caught while writing\n";
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}
