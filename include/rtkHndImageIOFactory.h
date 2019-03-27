/*=========================================================================
 *
 *  Copyright RTK Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

//#ifndef ITKHNDIMAGEIOFACTORY_H
//#define ITKHNDIMAGEIOFACTORY_H

#ifndef rtkHndImageIOFactory_h
#define rtkHndImageIOFactory_h

#include "RTKExport.h"
#include "rtkHndImageIO.h"

// itk include
#include <itkImageIOBase.h>
#include <itkObjectFactoryBase.h>
#include <itkVersion.h>

namespace rtk
{

/** \class HndImageIOFactory
 * \brief ITK factory for Hnd file I/O.
 *
 * \author Simon Rit
 *
 * \ingroup RTK
 */
class RTK_EXPORT HndImageIOFactory : public itk::ObjectFactoryBase
{
public:
  /** Standard class type alias. */
  using Self = HndImageIOFactory;
  using Superclass = itk::ObjectFactoryBase;
  using Pointer = itk::SmartPointer<Self>;
  using ConstPointer = itk::SmartPointer<const Self>;

  /** Class methods used to interface with the registered factories. */
  const char* GetITKSourceVersion(void) const ITK_OVERRIDE {
    return ITK_SOURCE_VERSION;
  }

  const char* GetDescription(void) const ITK_OVERRIDE {
    return "His ImageIO Factory, allows the loading of His images into insight";
  }

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(HndImageIOFactory, itk::ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory(void) {
    ObjectFactoryBase::RegisterFactory( Self::New() );
  }

protected:
  HndImageIOFactory();
  virtual ~HndImageIOFactory() ITK_OVERRIDE {}
  using myProductType = HndImageIOFactory;
  const myProductType* m_MyProduct;

private:
  HndImageIOFactory(const Self&); //purposely not implemented
  void operator=(const Self&);    //purposely not implemented

};

} // end namespace

#endif // rtkHndImageIOFactory_h
