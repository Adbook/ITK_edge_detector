#ifndef EDGE_DETECTION_FILTER_HXX
#define EDGE_DETECTION_FILTER_HXX

#include "edge_detection_filter.h"



template<typename TImage>
void EdgeDetectionFilter<TImage>::GenerateData(){

	typename TImage::ConstPointer input = this->GetInput();
	typename TImage::Pointer output = this ->GetOutput();

	/** Image allocations and iterator definitions **/
	//container for the de-noised inage (so we don't modify the input image)
	typename TImage::Pointer denoised_image = TImage::New();
	denoised_image->SetRegions(input->GetRequestedRegion());
	denoised_image->Allocate();

	//image for the edge gradient intensity
	typename TImage::Pointer edge_gradient = TImage::New();
	edge_gradient->SetRegions(input->GetRequestedRegion());
	edge_gradient->Allocate();
	
	//image for the edge direction (rounded to either vertical, horizontal or diagonal)
	typename TImage::Pointer edge_angle = TImage::New();
	edge_angle->SetRegions(input->GetRequestedRegion());
	edge_angle->Allocate();

	typename TImage::Pointer max_edges = TImage::New();
	max_edges->SetRegions(input->GetRequestedRegion());
	max_edges->Allocate();
	
	//output allocation
	this->AllocateOutputs();

	//edge detection

	ReduceNoise(input, denoised_image);
	ComputeGradients(denoised_image, edge_gradient, edge_angle);
	RemoveNonMaximums(edge_gradient, edge_angle, max_edges);
	if(m_automatic_thresholding)
		ComputeThreshold(max_edges);
	RemoveWeakEdges(max_edges, output);

}



template<typename TImage>
void EdgeDetectionFilter<TImage>::GenerateOffsetVector(std::vector< typename itk::NeighborhoodIterator<TImage>::OffsetType > &v){

		v.push_back(typename itk::NeighborhoodIterator<TImage>::OffsetType  {0, -1});
		v.push_back(typename itk::NeighborhoodIterator<TImage>::OffsetType  {1, -1});
		v.push_back(typename itk::NeighborhoodIterator<TImage>::OffsetType  {1, 0});
		v.push_back(typename itk::NeighborhoodIterator<TImage>::OffsetType  {1, 1});
		v.push_back(typename itk::NeighborhoodIterator<TImage>::OffsetType  {0, 1});
		v.push_back(typename itk::NeighborhoodIterator<TImage>::OffsetType  {-1, 1});
		v.push_back(typename itk::NeighborhoodIterator<TImage>::OffsetType  {-1, 0});
		v.push_back(typename itk::NeighborhoodIterator<TImage>::OffsetType  {-1, -1});

}

/** Denoising **/
//we use a gaussian operator to reduce the image noise
template<typename TImage>
void EdgeDetectionFilter<TImage>::ReduceNoise(typename TImage::ConstPointer input, typename TImage::Pointer output){
	
	//Gaussian operator definition (for de-noising)  
	typedef itk::GaussianOperator<float, TImage::ImageDimension> GaussianOperatorType;
	GaussianOperatorType gaussian_operatorH;
	GaussianOperatorType gaussian_operatorV;

	
	gaussian_operatorH.SetDirection(0);
	gaussian_operatorH.SetVariance(m_variance);
	gaussian_operatorH.CreateDirectional();

	gaussian_operatorV.SetDirection(1);
	gaussian_operatorV.SetVariance(m_variance); 
	gaussian_operatorV.CreateDirectional();

	itk::Size<2> radius;
	radius.Fill(gaussian_operatorH.GetRadius()[0]);
	
	//iterators
	ConstNeighborhoodIteratorType input_n_it (radius, input, input->GetRequestedRegion());
	itk::ImageRegionIterator <TImage> output_it(output, input->GetRequestedRegion()); 

	//object needed to compute the inner product between the neighborhood and the gaussian operator
	itk::NeighborhoodInnerProduct <TImage> neighborhoodInnerProduct;

	//iterator initialization
	input_n_it.GoToBegin();
	output_it.GoToBegin();

	//iteration loop
	while(!input_n_it.IsAtEnd()){
		typename TImage::PixelType H = neighborhoodInnerProduct(input_n_it, gaussian_operatorH);
		typename TImage::PixelType V = neighborhoodInnerProduct(input_n_it, gaussian_operatorV);
		output_it.Set( sqrt(H*H + V*V));
		++input_n_it;
		++output_it;
	}
	
}


/** Edge gradient and angle calculation **/
//we use a sobel operator to get vertical and horizontal gradient intensities (Gx and Gy)
// G = sqrt(Gx^2+Gy^2) and theta = atan2(Gy,Gx)
template<typename TImage>
void EdgeDetectionFilter<TImage>::ComputeGradients(typename TImage::Pointer input, typename TImage::Pointer edge_gradient, typename TImage::Pointer edge_angle){
	
	
	
	typename ConstNeighborhoodIteratorType::RadiusType radius; //neighborhood and operator radius //TODO: try a larger radius ?
	radius.Fill(2);

	//iterators
	ConstNeighborhoodIteratorType denoisedNeighborhoodIt (radius, input, input->GetRequestedRegion());
	itk::ImageRegionIterator <TImage> gradientIt(edge_gradient, input->GetRequestedRegion()); //output regionIterator
	itk::ImageRegionIterator <TImage> angleIt(edge_angle, input->GetRequestedRegion()); //output regionIterator


	//horizontal sobel operator
	itk::SobelOperator<typename TImage::PixelType, TImage::ImageDimension> horizontalSobelOperator;
	horizontalSobelOperator.SetDirection(0);
	horizontalSobelOperator.CreateToRadius(radius);
	
	//vertical sobel operator
	itk::SobelOperator<typename TImage::PixelType, TImage::ImageDimension> verticalSobelOperator;
	verticalSobelOperator.SetDirection(1);
	verticalSobelOperator.CreateToRadius(radius);

	//object needed to compute the inner product between the neighborhood and the gaussian operator
	itk::NeighborhoodInnerProduct <TImage> neighborhoodInnerProduct;

	//initialization
	denoisedNeighborhoodIt.GoToBegin();
	gradientIt.GoToBegin();
	angleIt.GoToBegin();
	
	while(!denoisedNeighborhoodIt.IsAtEnd()){
		double Gx = neighborhoodInnerProduct( denoisedNeighborhoodIt, horizontalSobelOperator );
		double Gy = neighborhoodInnerProduct( denoisedNeighborhoodIt, verticalSobelOperator );
		gradientIt.Set( sqrt(Gx * Gx + Gy * Gy));

		//TODO: clean up
		double angle = fmod((atan2(Gy, Gx) * 180 / M_PI ) + 90, 180); 
		if(angle < 0) angle += 180;
		if(angle >= 180) angle -= 180;

		double rounded_angle =  round( angle / 45 ) * 45; //rounding the directions to horiz, vert or diag. Could also interpolate to be more accurate but unnecessary
		if(rounded_angle == 180) rounded_angle = 0;
		
		angleIt.Set( angle ); 
	
		++angleIt;
		++gradientIt;
		++denoisedNeighborhoodIt;
	}
}

/** Step 3: non-maximum suppression **/
//modifies edge_gradient
//for each pixel, if one of its neighbors in the edge_angle direction is greater than the pixel itself, we set it to zero
template<typename TImage>
void EdgeDetectionFilter<TImage>::RemoveNonMaximums(typename TImage::Pointer edge_gradient, typename TImage::Pointer edge_angle, typename TImage::Pointer output){

	//iterators	
	typename NeighborhoodIteratorType::RadiusType radius;
	radius.Fill(1);
	NeighborhoodIteratorType gradient_n_it (radius, edge_gradient, edge_gradient->GetRequestedRegion());
	itk::ImageRegionIterator <TImage> output_it (output, edge_gradient->GetRequestedRegion());
	itk::ImageRegionIterator <TImage> angle_it (edge_angle, edge_gradient->GetRequestedRegion()); //output regionIterator

	//iterator initialization
	gradient_n_it.GoToBegin();
	angle_it .GoToBegin();
	output_it.GoToBegin();

	//vector used to map angle values to neighborhood offsets
	std::vector<typename NeighborhoodIteratorType::OffsetType> ordered_offset_list;
	GenerateOffsetVector(ordered_offset_list);

	//offset to get the middle pixel of the neighborhood
	typename NeighborhoodIteratorType::OffsetType origin_offset = {{0, 0}};
	
	while(!angle_it .IsAtEnd())
	{
		double angle = angle_it .Value(); //getting the angle value

		double rounded_angle =  round( angle / 45 ) * 45;
		if(rounded_angle == 360) rounded_angle = 0;


/*
		float t = (fmod(angle, 45) / 45); //TODO: approximate interpolation (should not be linear)

		
		int index1, index2;
		if(angle < rounded_angle){
			index2 = rounded_angle / 45;
			index1 = index2 - 1;
			if(index1 == -1) index1 = 7;
		}else if(angle > rounded_angle){
			index1 = rounded_angle / 45;
			index2 = index1 + 1;
			if(index2 == 8) index2 = 0;

		}else{ //equality
			index1 = rounded_angle / 45;
			index2 = index1;
			t = 0;
		}
		
		auto offset1 = ordered_offset_list[index1];
		auto offset2 = ordered_offset_list[index2];

		typename TImage::PixelType local_pixel_value = gradient_n_it.GetPixel(origin_offset);
		output_it.Set(local_pixel_value  );


		for(int i = 0; i < 2; ++i){
			double pixel_value1 = gradient_n_it.GetPixel(offset1);
			double pixel_value2 = gradient_n_it.GetPixel(offset2);
		
			double real_value = t * pixel_value1 + (1-t) * pixel_value2;
			 

			if(local_pixel_value  < real_value){
				output_it.Set(0);
			}
			
			offset1[0] *= -1;
			offset1[1] *= -1;
			offset2[0] *= -1;
			offset2[1] *= -1;
		}*/
		

		auto offset = ordered_offset_list[  (rounded_angle / 45)]; //getting the offset from the angle todo: interpolation instead of rounding the angle ?
		
		//getting the opposite offset value
		auto opposite_offset = offset; 
		opposite_offset[0] *= -1; 
		opposite_offset[1] *= -1;

		//getting the pixel value
		typename TImage::PixelType local_pixel_value = gradient_n_it.GetPixel(origin_offset);
		output_it.Set(local_pixel_value );
	
		//pixel value comparisons
		bool isInBounds = true;
		double pixel_value = gradient_n_it.GetPixel(offset, isInBounds);
	
		if(isInBounds && pixel_value > local_pixel_value)
			output_it.Set(0);

		//same comparison, with the opposite edge
		pixel_value = gradient_n_it.GetPixel(opposite_offset, isInBounds);
		if(isInBounds && pixel_value > local_pixel_value)
			output_it.Set( 0);
	

		++angle_it ;
		++gradient_n_it;
		++output_it;
	}
}

template<typename TImage>
void EdgeDetectionFilter<TImage>::RemoveWeakEdges(typename TImage::Pointer input, typename TImage::Pointer output){

	//offset definitions for the neighborhood iterator
	std::vector<typename NeighborhoodIteratorType::OffsetType> ordered_offset_list;
	GenerateOffsetVector(ordered_offset_list);
	typename NeighborhoodIteratorType::OffsetType origin_offset = {{0, 0}};

	//radius creation for the neighborhood
	typename NeighborhoodIteratorType::RadiusType radius;
	radius.Fill(1);

	//iterator creation
	NeighborhoodIteratorType input_n_it(radius, input, input->GetRequestedRegion());
	itk::ImageRegionIterator <TImage> output_it(output, input->GetRequestedRegion()); 
	
	//initialization
	input_n_it.GoToBegin();
	output_it.GoToBegin();

	//iterations
	while(!input_n_it.IsAtEnd()){
		output_it.Set(0);
		//strong edges ( > upper thresh ) are automatically valid
		if(input_n_it.GetPixel(origin_offset) >= m_upper_threshold)
			output_it.Set(255);

		//weak edges are valid if a strong edge is in their neighborhood
		else if(input_n_it.GetPixel(origin_offset) > m_lower_threshold){
			for(int off = 0; off < 8; off++){
				if(input_n_it.GetPixel(ordered_offset_list[off]) > m_upper_threshold){
					output_it.Set(255);
					break;
				}
			}
		}

		++input_n_it;
		++output_it;
	}
}

template < typename TImage >
void EdgeDetectionFilter<TImage>::ComputeThreshold (typename TImage::Pointer input){
	//converting the image to a list of samples
	typedef itk::Statistics::ImageToListSampleAdaptor < TImage > AdaptorType;
	typename AdaptorType::Pointer adaptor = AdaptorType::New();
	adaptor->SetImage(input);
	adaptor->Update();
	//const typename AdaptorType::MeasurementVectorType& measurementVector = adaptor->GetMeasurementVector();
	typename AdaptorType::Iterator iter = adaptor->Begin();
	double maxVal = 0;
	while(iter != adaptor->End()){ //todo: very slow
		double val = iter.GetMeasurementVector()[0];
		if(val > maxVal)
			maxVal = val;
		++iter;
	}
	maxVal = ceil(maxVal);

	typedef typename TImage::PixelType HistogramMeasurementType;
	typedef itk::Statistics::Histogram < HistogramMeasurementType > HistogramType;
	typedef itk::Statistics::SampleToHistogramFilter< AdaptorType, HistogramType > HistogramFilterType;

	//filter initialization
	typename HistogramFilterType::Pointer filter = HistogramFilterType::New();
	const unsigned int numberOfComponents = 1;
	typename HistogramType::SizeType size (numberOfComponents);
	size.Fill(maxVal); //TODO: too big

	filter->SetInput(adaptor);
	filter->SetHistogramSize(size);
	filter->SetMarginalScale(10);

	typename HistogramType::MeasurementVectorType min ( numberOfComponents);
	typename HistogramType::MeasurementVectorType max ( numberOfComponents);
	min.Fill(0);
	max.Fill(maxVal);

	filter->SetHistogramBinMinimum( min );
	filter->SetHistogramBinMaximum( max );

	//histogram calculation
	filter->Update();
	typename HistogramType::ConstPointer histogram = filter -> GetOutput();
	const unsigned int histSize = histogram->Size();

	//Otsu's method for calculating the threshold (maximize inter-class variance)
	double max_variance = 0;
	int max_variance_thresh = 0;

	for(unsigned int t = 0; t < maxVal; ++t){

		unsigned int bg_class_prob = 0, fg_class_prob = 0;
		double fg_class_mean = 0, bg_class_mean = 0;

		//calculating class probabilities
		for(unsigned int bin = 0; bin < histSize; ++bin){
			if(bin <= t) 
				bg_class_prob += histogram->GetFrequency(bin, 0);
			else 
				fg_class_prob += histogram->GetFrequency(bin, 0);
		}

		//calculating class means
		for(unsigned int bin = 0; bin < histSize; ++bin){
			if(bin <= t)
				bg_class_mean += bin * ( (double)histogram->GetFrequency(bin, 0) / static_cast<double> (bg_class_prob) );
			else
				fg_class_mean += bin * ( (double)histogram->GetFrequency(bin, 0) / static_cast<double> (fg_class_prob) );
		}

		//calculating the inter-class-variance
		double inter_class_variance = bg_class_prob * fg_class_prob * (bg_class_mean - fg_class_mean) * (bg_class_mean - fg_class_mean);
		
		//maximization of the variance
		if(inter_class_variance > max_variance){
			max_variance = inter_class_variance;
			max_variance_thresh = t;
		}


	}

	m_upper_threshold = max_variance_thresh;
	m_lower_threshold = max_variance_thresh / 2;
	
}

#endif