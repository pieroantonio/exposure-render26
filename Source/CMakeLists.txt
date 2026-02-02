# Exposure Render: An interactive photo-realistic volume rendering framework
# Copyright (C) 2011 Thomas Kroes

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8)

PROJECT(ExposureRender)

FIND_PACKAGE(CUDA)

# Supported streaming architectures (customize with EXPOSURE_RENDER_CUDA_ARCHITECTURES)
SET(EXPOSURE_RENDER_CUDA_ARCHITECTURES "52;60;70;75;80;86;89;90" CACHE STRING "CUDA architectures to target (e.g. 75;86;90)")

FOREACH(ARCH IN LISTS EXPOSURE_RENDER_CUDA_ARCHITECTURES)
	SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_${ARCH},code=sm_${ARCH};${CUDA_NVCC_FLAGS}")
ENDFOREACH()

# Emit PTX for the newest architecture in the list for forward compatibility.
LIST(LENGTH EXPOSURE_RENDER_CUDA_ARCHITECTURES EXPOSURE_RENDER_CUDA_ARCH_COUNT)
MATH(EXPR EXPOSURE_RENDER_CUDA_ARCH_LAST_INDEX "${EXPOSURE_RENDER_CUDA_ARCH_COUNT} - 1")
LIST(GET EXPOSURE_RENDER_CUDA_ARCHITECTURES ${EXPOSURE_RENDER_CUDA_ARCH_LAST_INDEX} EXPOSURE_RENDER_CUDA_PTX_ARCH)
SET(CUDA_NVCC_FLAGS "-gencode=arch=compute_${EXPOSURE_RENDER_CUDA_PTX_ARCH},code=compute_${EXPOSURE_RENDER_CUDA_PTX_ARCH};${CUDA_NVCC_FLAGS}")

#SET(CUDA_NVCC_FLAGS "-OPT:Olimit=99999;${CUDA_NVCC_FLAGS}")

# Add CUDA includes
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CUDA_SDK_ROOT_DIR}/common/inc
	${CUDA_TOOLKIT_INCLUDE}
)

# Export symbols
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_EXPORTING")

# Make the "Core" source group
SOURCE_GROUP("Core" FILES ${vtkErCoreSources})

# General
SET(General
	device.h
	utilities.h
	geometry.h
	transport.h
	raymarching.h
	shader.h
	sample.h
	rng.h
	montecarlo.h
	ray.h
	volumes.h
	lights.h
	objects.h
	clippingobjects.h
	textures.h
	filter.h
	log.h
	defines.h
	camera.h
	procedural.h
	shape.h
	matrix.h
	vector.h
	buffer1d.h
	buffer2d.h
	buffer3d.h
	boundingbox.h
	transferfunction.h
	rendersettings.h
	timing.h
	exception.h
	enums.h
	color.h
	exposurerender.h
	exposurerender.cpp
	tracer.h
	volume.h
	light.h
	scatterevent.h
	intersection.h
	object.h
	clippingobject.h
	texture.h
	bitmap.h
	buffer.h
	framebuffer.h
	pf.h
	plf.h
	pcf.h
	singlescattering.h
)

# General group
SOURCE_GROUP("General" FILES ${General})

# Shapes
SET(Shapes
	shapes.h
	plane.h
	box.h
	disk.h
	ring.h
	sphere.h
	cylinder.h
)

# Shapes group
SOURCE_GROUP("Shapes" FILES ${Shapes})

# API
SET(Bindable
	erbindable.h
	ertracer.h
	ervolume.h
	erlight.h
	erobject.h
	erclippingobject.h
	ertexture.h
	erbitmap.h
)

# API group
SOURCE_GROUP("Bindable" FILES ${Bindable})

# Filters
SET(Cuda
	singlescattering.cuh
	estimate.cuh
	gradientmagnitude.cuh
	filterrunningestimate.cuh
	filterframeestimate.cuh
	tonemap.cuh
	autofocus.cuh
	list.cuh
	wrapper.cuh
	macros.cuh
	core.cu
)

# Filters group
SOURCE_GROUP("Cuda" FILES ${Cuda})

# Make the library
CUDA_ADD_LIBRARY(ErCore ${General} ${Shapes} ${Bindable} ${Cuda} SHARED)
