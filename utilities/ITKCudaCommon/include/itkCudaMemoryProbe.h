/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkCudaMemoryProbe_h
#define itkCudaMemoryProbe_h

#include "itkResourceProbe.h"
#include "itkIntTypes.h"
#include "itkCudaWin32Header.h"

namespace itk
{
/** \class CudaMemoryProbe
 *
 *  \brief Computes the Cuda memory allocated between two points in code.
 *
 *   This class allows the user to trace the Cuda memory charge between the
 *   execution of two pieces of code. It can be started and stopped in order to
 *   evaluate the execution over multiple passes. The values of memory are
 *   taken from cudaMemGetInfo.
 *
 * \ingroup ITKCudaCommon
 */
class ITKCudaCommon_EXPORT CudaMemoryProbe : public ResourceProbe<OffsetValueType, double>
{
public:
  CudaMemoryProbe();
  ~CudaMemoryProbe() override;

  /** Type for measuring memory. */
  using CudaMemoryLoadType = OffsetValueType;

  /** Type for measuring the average memory. */
  using MeanCudaMemoryLoadType = double;

protected:
  CudaMemoryLoadType
  GetInstantValue() const override;
};
} // end namespace itk

#endif // itkCudaMemoryProbe_h
