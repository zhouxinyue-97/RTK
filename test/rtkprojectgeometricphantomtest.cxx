#include "rtkTest.h"
#include "rtkMacro.h"
#include "rtkThreeDCircularProjectionGeometryXMLFile.h"
#include "rtkConstantImageSource.h"
#include "rtkGeometricPhantomFileReader.h"
#include "rtkProjectGeometricPhantomImageFilter.h"
#include "rtkSheppLoganPhantomFilter.h"

#include <itkRegularExpressionSeriesFileNames.h>

/**
 * \file rtkprojectgeometricphantomtest.cxx
 *
 * \brief Functional test for the class that projects a geometric phantom
 * specified in a config file.
 *
 * This test generates the projections of a Shepp-Logan phantom which are
 * specified by a configuration file located at the Baseline folder.
 * The generated results are compared to the expected results, which are
 * created through hard-coded geometric parameters.
 *
 * \author Marc Vila
 */

int main(int argc, char*argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  ConfigFile.txt" << std::endl;
    return EXIT_FAILURE;
  }

  const unsigned int Dimension = 3;
  using OutputPixelType = float;
  using OutputImageType = itk::Image< OutputPixelType, Dimension >;
#if FAST_TESTS_NO_CHECKS
  const unsigned int NumberOfProjectionImages = 3;
#else
  const unsigned int NumberOfProjectionImages = 180;
#endif
  using GeometryType = rtk::ThreeDCircularProjectionGeometry;

  // Constant image sources
  using ConstantImageSourceType = rtk::ConstantImageSource< OutputImageType >;
  ConstantImageSourceType::PointType origin;
  ConstantImageSourceType::SizeType size;
  ConstantImageSourceType::SpacingType spacing;

  ConstantImageSourceType::Pointer projectionsSource = ConstantImageSourceType::New();
  origin[0] = -254.;
  origin[1] = -254.;
  origin[2] = -254.;
#if FAST_TESTS_NO_CHECKS
  size[0] = 2;
  size[1] = 2;
  size[2] = NumberOfProjectionImages;
  spacing[0] = 508.;
  spacing[1] = 508.;
  spacing[2] = 508.;
#else
  size[0] = 128;
  size[1] = 128;
  size[2] = NumberOfProjectionImages;
  spacing[0] = 4.;
  spacing[1] = 4.;
  spacing[2] = 4.;
#endif
  projectionsSource->SetOrigin( origin );
  projectionsSource->SetSpacing( spacing );
  projectionsSource->SetSize( size );
  projectionsSource->SetConstant( 0. );

  // Geometry object
  using GeometryType = rtk::ThreeDCircularProjectionGeometry;
  GeometryType::Pointer geometry = GeometryType::New();
  for(unsigned int noProj=0; noProj<NumberOfProjectionImages; noProj++)
    geometry->AddProjection(600., 1200., noProj*360./NumberOfProjectionImages);

  // Shepp Logan projections filter
  using SLPType = rtk::SheppLoganPhantomFilter<OutputImageType, OutputImageType>;
  SLPType::Pointer slp=SLPType::New();
  slp->SetInput( projectionsSource->GetOutput() );
  slp->SetGeometry(geometry);
  TRY_AND_EXIT_ON_ITK_EXCEPTION( slp->Update() );

  // Shepp Logan projections filter from Configuration File
  using PGPType = rtk::ProjectGeometricPhantomImageFilter<OutputImageType, OutputImageType>;
  PGPType::Pointer pgp=PGPType::New();
  pgp->SetInput( projectionsSource->GetOutput() );
  pgp->SetGeometry(geometry);
  pgp->SetConfigFile(argv[1]);
  TRY_AND_EXIT_ON_ITK_EXCEPTION( pgp->Update() );

  CheckImageQuality<OutputImageType>(slp->GetOutput(), pgp->GetOutput(), 0.00055, 88, 255.0);
  std::cout << "Test PASSED! " << std::endl;

  return EXIT_SUCCESS;
}
