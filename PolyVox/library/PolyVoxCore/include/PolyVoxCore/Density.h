/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#ifndef __PolyVox_Density_H__
#define __PolyVox_Density_H__

#include "PolyVoxCore/Voxel.h"

#include "PolyVoxImpl/TypeDef.h"

#include <cassert>
#include <limits>

#undef min
#undef max

namespace PolyVox
{
	///This class represents a voxel storing only a density.
	////////////////////////////////////////////////////////////////////////////////
	/// In order to perform a surface extraction on a LargeVolume, PolyVox needs the underlying
	/// voxel type to provide both getDensity() and getMaterial() functions. The getDensity()
	/// function is used to determine if a voxel is 'solid', and if it is then the getMaterial()
	/// funtion is used to determine what material should be assigned to the resulting mesh.
	///
	/// This class meets these requirements, although it only actually stores a density value.
	/// For the getMaterial() function it just returns a constant value of '1'.
	///
	/// \sa Material, MaterialDensityPair
	////////////////////////////////////////////////////////////////////////////////

	// int32_t template parameter is a dummy, required as the compiler expects to be able to declare an
	// instance of VoxelType::MaterialType without knowing that VoxelType doesn't actually have a material.
	template <typename Type>
	class Density
	{
	public:
		//We expose DensityType and MaterialType in this way so that, when code is
		//templatised on voxel type, it can determine the underlying storage type
		//using code such as 'VoxelType::DensityType value = voxel.getDensity()'
		//or 'VoxelType::MaterialType value = voxel.getMaterial()'.
		typedef Type DensityType;
		typedef int32_t MaterialType; //Shouldn't define this one...

		Density() : m_uDensity(0) {}
		Density(DensityType uDensity) : m_uDensity(uDensity) {}

		bool operator==(const Density& rhs) const throw()
		{
			return (m_uDensity == rhs.m_uDensity);
		};

		bool operator!=(const Density& rhs) const throw()
		{
			return !(*this == rhs);
		}

		DensityType getDensity() const throw() { return m_uDensity; }
		//MaterialType getMaterial() const throw() { return 1; }

		void setDensity(DensityType uDensity) { m_uDensity = uDensity; }
		//void setMaterial(MaterialType /*uMaterial*/) { assert(false); } //Cannot set material on voxel of type Density

		//static DensityType getmaxDensity()() throw() { return (std::numeric_limits<DensityType>::max)(); } 
		//static DensityType getminDensity()() throw() { return (std::numeric_limits<DensityType>::min)(); }
		static DensityType getThreshold() throw() { return (std::numeric_limits<DensityType>::max)() / 2; }

	private:
		DensityType m_uDensity;
	};

	// These are the predefined density types. The 8-bit types are sufficient for many purposes (including
	// most games) but 16-bit and float types do have uses particularly in medical/scientific visualisation.
	typedef Density<uint8_t> Density8;
	
	template<>
	class VoxelTypeTraits< Density8 >	
	{
	public:
		typedef uint8_t DensityType;
		typedef uint8_t MaterialType;
		static Density8::DensityType minDensity() { return std::numeric_limits<Density8::DensityType>::min(); }
		static Density8::DensityType maxDensity() { return std::numeric_limits<Density8::DensityType>::max(); }
	};

	template<>
	typename VoxelTypeTraits<Density8>::DensityType convertToDensity(Density8 voxel);
}

#endif //__PolyVox_Density_H__
