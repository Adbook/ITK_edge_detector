#ifndef EDGE_DETECTION_FILTER_H
#define EDGE_DETECTION_FILTER_H

#include <itkImageToImageFilter.h>
#include <itkImage.h>
#include <itkSmartPointer.h>
#include "itkObjectFactory.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodIterator.h"

//use the existing sobel operator
#include "itkSobelOperator.h"
//use the existing gaussian operator
#include "itkGaussianOperator.h"
//inner product to do the convolution betzeen the image and the operators
#include "itkNeighborhoodInnerProduct.h"

#include "itkImageToListSampleAdaptor.h"
#include "itkSampleToHistogramFilter.h"
#include "itkHistogram.h"

//using namespace itk;

template <typename TImage>
class EdgeDetectionFilter : public itk::ImageToImageFilter <TImage, TImage>{
public:
	//standard type definitions
	typedef EdgeDetectionFilter Self;
	typedef itk::ImageToImageFilter<TImage, TImage> Superclass;
  	typedef itk::SmartPointer<Self> Pointer;
  	typedef itk::SmartPointer<const Self> ConstPointer;

  	//type definitions
	typedef TImage ImageType;
	typedef typename ImageType::PixelType PixelType;

	//this macro adds the New() method for the factory
	itkNewMacro(Self);

	//this macro adds runtime type information
	itkTypeMacro(EdgeDetectionFilter, itk::ImageToImagefilter);

	//accessors
	void SetLowerThreshold(typename TImage::PixelType l) { m_lower_threshold = l; }
	void SetUpperThreshold(typename TImage::PixelType u) { m_upper_threshold = u; }
	void SetVariance(double v) { m_variance = v; }
	void SetAutomaticThresholdingOn () { m_automatic_thresholding = true; }
	void SetAutomaticThresholdingOff () { m_automatic_thresholding = false; }

	typename TImage::PixelType GetLowerThreshold() { return m_lower_threshold; }
	typename TImage::PixelType GetUpperThreshold() { return m_upper_threshold; }
	double GetVariance() { return m_variance; }
	bool GetAutomaticthresholding () { return m_automatic_thresholding;}
protected:
	EdgeDetectionFilter(){}
	~EdgeDetectionFilter(){}


	virtual void GenerateData();
private:
	//copy operations: private and not implemented
	EdgeDetectionFilter(const Self &A);
	void operator=(const Self &A);


	typedef  itk::ConstNeighborhoodIterator<TImage> ConstNeighborhoodIteratorType;
	typedef  itk::NeighborhoodIterator<TImage> NeighborhoodIteratorType;

	void GenerateOffsetVector(std::vector< typename NeighborhoodIteratorType::OffsetType > &v);
	void ReduceNoise(typename TImage::ConstPointer input, typename TImage::Pointer output);
	void ComputeGradients(typename TImage::Pointer input, typename TImage::Pointer edge_gradient, typename TImage::Pointer edge_angle);
	void RemoveNonMaximums(typename TImage::Pointer edge_gradient, typename TImage::Pointer edge_angle, typename TImage::Pointer output);
	void RemoveWeakEdges(typename TImage::Pointer input, typename TImage::Pointer output);
	void ComputeThreshold (typename TImage::Pointer input);

	typename TImage::PixelType m_lower_threshold; //weak pixel threshold
	typename TImage::PixelType m_upper_threshold; //strong pixel threshold
	double m_variance; //gaussian filter desired variance
	bool m_automatic_thresholding; //determines whether the filter should try to detect the optimal threshold values

};

#include "edge_detection_filter.hxx"

#endif