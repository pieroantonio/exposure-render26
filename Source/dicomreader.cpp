/*
    Exposure Render: An interactive photo-realistic volume rendering framework
    Copyright (C) 2011 Thomas Kroes

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "dicomreader.h"

#include <cstring>
#include <vector>

#ifdef EXPOSURE_RENDER_USE_GDCM
#include <gdcmDirectory.h>
#include <gdcmImage.h>
#include <gdcmImageReader.h>
#include <gdcmIPPSorter.h>
#include <gdcmPixelFormat.h>
#endif

namespace ExposureRender
{

static void SetErrorMessage(std::string* pErrorMessage, const char* pMessage)
{
	if (pErrorMessage)
		*pErrorMessage = pMessage;
}

#ifdef EXPOSURE_RENDER_USE_GDCM
static bool ReadSlice(const std::string& Filename,
	const Vec3i& Resolution,
	unsigned short* pDestination,
	std::string* pErrorMessage)
{
	gdcm::ImageReader reader;
	reader.SetFileName(Filename.c_str());

	if (!reader.Read())
	{
		SetErrorMessage(pErrorMessage, "Failed to read DICOM slice.");
		return false;
	}

	const gdcm::Image& image = reader.GetImage();
	unsigned int dims[3] = { 0, 0, 0 };
	image.GetDimensions(dims);

	if ((int)dims[0] != Resolution[0] || (int)dims[1] != Resolution[1])
	{
		SetErrorMessage(pErrorMessage, "Slice dimensions do not match series.");
		return false;
	}

	const gdcm::PixelFormat pixelFormat = image.GetPixelFormat();
	const unsigned short bitsAllocated = pixelFormat.GetBitsAllocated();
	const bool isSigned = pixelFormat.GetPixelRepresentation() != 0;

	if (bitsAllocated != 8 && bitsAllocated != 16)
	{
		SetErrorMessage(pErrorMessage, "Unsupported DICOM bit depth (only 8/16-bit supported).");
		return false;
	}

	const size_t bufferLength = image.GetBufferLength();
	std::vector<char> buffer(bufferLength);

	if (!image.GetBuffer(&buffer[0]))
	{
		SetErrorMessage(pErrorMessage, "Failed to extract DICOM pixel buffer.");
		return false;
	}

	const size_t pixels = static_cast<size_t>(Resolution[0]) * static_cast<size_t>(Resolution[1]);

	if (bitsAllocated == 16)
	{
		if (isSigned)
		{
			const short* pSource = reinterpret_cast<const short*>(&buffer[0]);
			for (size_t i = 0; i < pixels; ++i)
				pDestination[i] = static_cast<unsigned short>(pSource[i]);
		}
		else
		{
			const unsigned short* pSource = reinterpret_cast<const unsigned short*>(&buffer[0]);
			memcpy(pDestination, pSource, pixels * sizeof(unsigned short));
		}
	}
	else
	{
		const unsigned char* pSource = reinterpret_cast<const unsigned char*>(&buffer[0]);
		for (size_t i = 0; i < pixels; ++i)
			pDestination[i] = static_cast<unsigned short>(pSource[i]);
	}

	return true;
}
#endif

bool DicomSeriesReader::LoadSeries(const std::string& Directory,
	Buffer3D<unsigned short>& Voxels,
	Vec3f& Spacing,
	std::string* pErrorMessage)
{
#ifndef EXPOSURE_RENDER_USE_GDCM
	SetErrorMessage(pErrorMessage, "GDCM support is disabled. Rebuild with EXPOSURE_RENDER_USE_GDCM.");
	return false;
#else
	gdcm::Directory directory;
	directory.Load(Directory.c_str(), true);
	const std::vector<std::string>& filenames = directory.GetFilenames();

	if (filenames.empty())
	{
		SetErrorMessage(pErrorMessage, "No DICOM files found in directory.");
		return false;
	}

	gdcm::IPPSorter sorter;
	sorter.SetComputeZSpacing(true);

	if (!sorter.Sort(filenames))
	{
		SetErrorMessage(pErrorMessage, "Failed to sort DICOM series.");
		return false;
	}

	const std::vector<std::string>& sorted = sorter.GetFilenames();

	if (sorted.empty())
	{
		SetErrorMessage(pErrorMessage, "No DICOM slices after sorting.");
		return false;
	}

	gdcm::ImageReader reader;
	reader.SetFileName(sorted[0].c_str());

	if (!reader.Read())
	{
		SetErrorMessage(pErrorMessage, "Failed to read first DICOM slice.");
		return false;
	}

	const gdcm::Image& image = reader.GetImage();
	unsigned int dims[3] = { 0, 0, 0 };
	image.GetDimensions(dims);

	double spacing[3] = { 1.0, 1.0, 1.0 };
	image.GetSpacing(spacing);
	Spacing = Vec3f((float)spacing[0], (float)spacing[1], (float)spacing[2]);

	const Vec3i resolution((int)dims[0], (int)dims[1], (int)sorted.size());
	Voxels.Resize(resolution);

	const size_t slicePixels = static_cast<size_t>(resolution[0]) * static_cast<size_t>(resolution[1]);

	for (size_t slice = 0; slice < sorted.size(); ++slice)
	{
		unsigned short* pSliceDestination = Voxels.Data + (slice * slicePixels);

		if (!ReadSlice(sorted[slice], resolution, pSliceDestination, pErrorMessage))
			return false;
	}

	return true;
#endif
}

}
