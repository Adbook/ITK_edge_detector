/** 
 *  This test file is quite similar to the main_cli executable
 * 	but instead of writing to the specified output file, it reads that file
 *  and compares it to the generated output
 *	returns EXIT_SUCCESS if the files are similar
 *  and EXIT_FAILURE otherwise
 *  SEM is used to handle the parameters
 **/

#include "itkImage.h"
#include <iostream>
#include <itkImportImageFilter.h>
#include <itkImageFileReader.h>
#include <itkTestingComparisonImageFilter.h>

#include "../edge_detection_filter.h"


#include "test_execCLP.h"

int main(int argc, char** argv)
{
 PARSE_ARGS; 
    //constants definitions
    const int dimension = 2;
    const double diffTolerance = 0.1; //arbitrary
    const unsigned int pixelTolerance = 10; //same


    //type definitions
    typedef float InputPixelType; 
    typedef itk::Image<InputPixelType , dimension> InputImageType;
    typedef itk::ImageFileReader<InputImageType> ImageReaderType;
    typedef itk::Testing::ComparisonImageFilter<InputImageType, InputImageType> CompType;
    typedef EdgeDetectionFilter<InputImageType> FilterType;

    //allocations
    ImageReaderType::Pointer inputReader = ImageReaderType::New();
    ImageReaderType::Pointer resultReader = ImageReaderType::New();
    FilterType::Pointer filter = FilterType::New();
    CompType::Pointer comparisonFilter = CompType::New();

    //reader configuration and tests
    inputReader->SetFileName(inputPicture.c_str());
    //read test
    try{
    	inputReader->UpdateLargestPossibleRegion();
    }catch (itk::ExceptionObject & e){
    	std::cerr << "Exception while reading: " << inputPicture.c_str() << std::endl;
    	return EXIT_FAILURE;
    }



    resultReader->SetFileName(resultPicture.c_str());
    //read test
        try{
    	resultReader->UpdateLargestPossibleRegion();
    }catch (itk::ExceptionObject & e){
    	std::cerr << "Exception while reading: " << resultPicture.c_str() << std::endl;
    	return EXIT_FAILURE;
    }


    //filter configuration (output values and thresholds)
    filter->SetInput(inputReader->GetOutput());

    if(upperThresh != -1 && lowerThresh != -1){ //both threshold parameters have to be manually entered, or thresholding will be automatic
        if(lowerThresh > upperThresh){
            std::cerr << "Error: lower threshold can't be superior to the upper threshold\n" << std::endl;
            return EXIT_FAILURE;
        }
        filter->SetAutomaticThresholdingOff();
        filter->SetLowerThreshold(lowerThresh);
        filter->SetUpperThreshold(upperThresh);
    } else filter->SetAutomaticThresholdingOn();

    filter->SetVariance(variance);

    //filter test
    try{
    	filter->Update();
    }catch(itk::ExceptionObject & e){
    	std::cerr << "Exception while using the filter" << std::endl;
    	return EXIT_FAILURE;
    }

    //result comparison
    comparisonFilter->SetValidInput(resultReader->GetOutput());
    comparisonFilter->SetTestInput(filter->GetOutput());

    comparisonFilter->UpdateLargestPossibleRegion();
  	bool differenceFailed = false;

  	double diff = comparisonFilter->GetTotalDifference();
  	unsigned int pixels = comparisonFilter->GetNumberOfPixelsWithDifferences();

  	if(diff > diffTolerance && pixels > pixelTolerance)
  		return EXIT_FAILURE;

	return EXIT_SUCCESS;


}