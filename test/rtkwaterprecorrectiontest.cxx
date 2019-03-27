#include "rtkTest.h"
#include "rtkConstantImageSource.h"

#include "rtkWaterPrecorrectionImageFilter.h"

/**
 * \file rtkwaterprecorrectiontest.cxx
 *
 * \brief Functional test for the water precorrection class
 *
 * \author S. Brousmiche
 */

int main(int , char** )
{
  const unsigned int Dimension = 2;
  using OutputPixelType = float;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;

  // Constant image sources
  using ConstantImageSourceType = rtk::ConstantImageSource< OutputImageType >;
  ConstantImageSourceType::PointType origin;
  ConstantImageSourceType::SizeType size;
  ConstantImageSourceType::SpacingType spacing;

  // Create constant image of value 2 and reference image.
  ConstantImageSourceType::Pointer imgIn  = ConstantImageSourceType::New();
  ConstantImageSourceType::Pointer imgRef = ConstantImageSourceType::New();

  origin[0] = -126;
  origin[1] = -126;
  size[0] = 16;
  size[1] = 16;
  spacing[0] = 16.;
  spacing[1] = 16.;

  imgIn->SetOrigin( origin );
  imgIn->SetSpacing( spacing );
  imgIn->SetSize( size );
  imgIn->SetConstant( 1.5 );
  //imgIn->UpdateOutputInformation();

  imgRef->SetOrigin( origin );
  imgRef->SetSpacing( spacing );
  imgRef->SetSize( size );
  imgRef->SetConstant( 5.0 );
  imgRef->Update();

  OutputImageType::Pointer output = imgIn->GetOutput();

  std::cout << "\n\n****** Case 1: order 2 ******" << std::endl;

  using WPCType = rtk::WaterPrecorrectionImageFilter<OutputImageType, OutputImageType>;
  WPCType::Pointer model2 = WPCType::New();

  // Update median filter
  WPCType::VectorType c1;
  c1.push_back(2.0);
  c1.push_back(2.0);
  model2->SetInput(output);
  model2->SetCoefficients(c1);
  model2->Update();

  CheckImageQuality<OutputImageType>(model2->GetOutput(), imgRef->GetOutput(), 1.8, 51, 1011.0);
  std::cout << "\n\nTest PASSED! " << std::endl;

  std::cout << "\n\n****** Case 2: order 3 ******" << std::endl;

  WPCType::Pointer model3 = WPCType::New();
  WPCType::VectorType c2;
  c2.push_back(0.05);
  c2.push_back(0.3);
  c2.push_back(2.0);
  model3->SetInput(imgIn->GetOutput());
  model3->SetCoefficients(c2);
  model3->Update();

  CheckImageQuality<OutputImageType>(model3->GetOutput(), imgRef->GetOutput(), 1.8, 51, 1011.0);
  std::cout << "\n\nTest PASSED! " << std::endl;

  std::cout << "\n\n****** Case 3: order 5 ******" << std::endl;

  WPCType::Pointer model5 = WPCType::New();

  WPCType::VectorType c3;
  c3.push_back(0.0687);
  c3.push_back(2.5);
  c3.push_back(0.6);
  c3.push_back(-0.2);
  c3.push_back(0.1);
  model5->SetInput(imgIn->GetOutput());
  model5->SetCoefficients(c3);
  model5->Update();

  CheckImageQuality<OutputImageType>(model5->GetOutput(), imgRef->GetOutput(), 1.8, 51, 1011.0);
  std::cout << "\n\nTest PASSED! " << std::endl;

  return EXIT_SUCCESS;
}
