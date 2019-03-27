#include "rtkTest.h"
#include "rtkMacro.h"
#include "rtkCudaFDKWeightProjectionFilter.h"
#include "rtkConstantImageSource.h"
#include "rtkSheppLoganPhantomFilter.h"
#include "rtkThreeDCircularProjectionGeometry.h"

#include <itkStreamingImageFilter.h>
#include <itkImageRegionSplitterDirection.h>

/**
 * \file rtkfdkprojweightcompcudatest.cxx
 *
 * \brief Test rtk::CudaFDKWeightProjectionFilter vs rtk::FDKWeightProjectionFilter
 *
 * This test compares CUDA and CPU implementations of the projection weighting
 * for FDK reconstruction.
 *
 * \author Simon Rit
 */

int main(int, char** )
{
  const unsigned int Dimension = 3;
  using OutputPixelType = float;
  using OutputImageType = itk::CudaImage< OutputPixelType, Dimension >;


  // Constant image sources
  using ConstantImageSourceType = rtk::ConstantImageSource< OutputImageType >;
  ConstantImageSourceType::PointType origin;
  ConstantImageSourceType::SizeType size;
  ConstantImageSourceType::SpacingType spacing;

  ConstantImageSourceType::Pointer projSource  = ConstantImageSourceType::New();
  origin[0] = -127.;
  origin[1] = -3.;
  origin[2] = 0.;
  size[0] = 128;
  size[1] = 4;
  size[2] = 4;
  spacing[0] = 2.;
  spacing[1] = 2.;
  spacing[2] = 2.;

  projSource->SetOrigin( origin );
  projSource->SetSpacing( spacing );
  projSource->SetSize( size );

  // Geometry
  using GeometryType = rtk::ThreeDCircularProjectionGeometry;
  GeometryType::Pointer geometry = GeometryType::New();
  geometry->AddProjection(600., 700., 0.  , 84., 35, 23, 15, 21, 26);
  geometry->AddProjection(500., 800., 45. , 21., 12, 16, 546, 14, 41);
  geometry->AddProjection(700., 900., 90. , 68., 68, 54, 38, 35, 56);
  geometry->AddProjection(900., 1000., 135., 48., 35, 84, 10, 84, 59);

  // Projections
  using SLPType = rtk::SheppLoganPhantomFilter<OutputImageType, OutputImageType>;
  SLPType::Pointer slp=SLPType::New();
  slp->SetInput( projSource->GetOutput() );
  slp->SetGeometry(geometry);
  slp->SetPhantomScale(116);
  TRY_AND_EXIT_ON_ITK_EXCEPTION( slp->Update() );

  for(int inPlace=0; inPlace<2; inPlace++)
    {
    std::cout << "\n\n****** Case " << inPlace*2 << ": no streaming, ";
    if(!inPlace)
      std::cout << "not";
    std::cout << " in place ******" << std::endl;

    using CUDAFDKWFType = rtk::CudaFDKWeightProjectionFilter;
    CUDAFDKWFType::Pointer cudafdkwf = CUDAFDKWFType::New();
    cudafdkwf->SetInput( slp->GetOutput() );
    cudafdkwf->SetGeometry(geometry);
    cudafdkwf->InPlaceOff();
    TRY_AND_EXIT_ON_ITK_EXCEPTION( cudafdkwf->Update() );

    using CPUFDKWFType = rtk::FDKWeightProjectionFilter<OutputImageType>;
    CPUFDKWFType::Pointer cpufdkwf = CPUFDKWFType::New();
    cpufdkwf->SetInput( slp->GetOutput() );
    cpufdkwf->SetGeometry(geometry);
    cpufdkwf->InPlaceOff();
    TRY_AND_EXIT_ON_ITK_EXCEPTION( cpufdkwf->Update() );

    CheckImageQuality< OutputImageType >(cudafdkwf->GetOutput(), cpufdkwf->GetOutput(), 1.e-5, 95, 1.);

    std::cout << "\n\n****** Case " << inPlace*2+1 << ": with streaming, ";
    if(!inPlace)
      std::cout << "not";
    std::cout << " in place ******" << std::endl;

    // Idem with streaming
    cudafdkwf = CUDAFDKWFType::New();
    cudafdkwf->SetInput( slp->GetOutput() );
    cudafdkwf->SetGeometry(geometry);
    cudafdkwf->InPlaceOff();

    using StreamingType = itk::StreamingImageFilter<OutputImageType, OutputImageType>;
    StreamingType::Pointer streamingCUDA = StreamingType::New();
    streamingCUDA->SetInput( cudafdkwf->GetOutput() );
    streamingCUDA->SetNumberOfStreamDivisions(4);
    itk::ImageRegionSplitterDirection::Pointer splitter = itk::ImageRegionSplitterDirection::New();
    splitter->SetDirection(2); // Splitting along direction 1, NOT 2
    streamingCUDA->SetRegionSplitter(splitter);
    TRY_AND_EXIT_ON_ITK_EXCEPTION( streamingCUDA->Update() );

    cpufdkwf = CPUFDKWFType::New();
    cpufdkwf->SetInput( slp->GetOutput() );
    cpufdkwf->SetGeometry(geometry);
    cpufdkwf->InPlaceOff();

    StreamingType::Pointer streamingCPU = StreamingType::New();
    streamingCPU->SetInput( cpufdkwf->GetOutput() );
    streamingCPU->SetNumberOfStreamDivisions(2);
    TRY_AND_EXIT_ON_ITK_EXCEPTION( streamingCPU->Update() );

    CheckImageQuality< OutputImageType >(streamingCUDA->GetOutput(), streamingCPU->GetOutput(), 1.e-5, 95, 1.);
    }

  // If all succeed
  std::cout << "\n\nTest PASSED! " << std::endl;
  return EXIT_SUCCESS;
}
