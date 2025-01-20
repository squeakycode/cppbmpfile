# C++ BMP File

## Purpose
Load and save BMP files for image processing.

## Design Goals
- Header-only design
- Follows the KISS principle (Keep It Simple, Stupid)
- User-friendly and well-documented
- Cross-platform support
- Generates no build warnings on common compilers
- High test coverage
- Capable of loading BMP data into a buffer
- Supports saving data from a buffer to a BMP file
- Handles 8-bit, 24-bit, and 32-bit formats without compression
- Intended to be encapsulated within a lightweight wrapper that can be easily adapted to specific use cases

## When to Use
- If you want to load and save BMP files using buffers for image processing.

## Sample Code
```
//-----------------------------------------------------------------------------
// cppbmpfile
//-----------------------------------------------------------------------------

#include <cppbmpfile/cppbmpfile.hpp>
#include <iostream>
#include <vector>

int main()
{
    // prepare writing
    cppbmpfile::image_properties imagePropertiesA;
    imagePropertiesA.height = 480;
    imagePropertiesA.width = 640;
    imagePropertiesA.pixel_format = cppbmpfile::pixel_format::Mono8;
    imagePropertiesA.orientation = cppbmpfile::orientation_type::bottom_up;
    imagePropertiesA.line_padding = 0;

    size_t bufferSizeA = cppbmpfile::bmp_file::compute_buffer_size(imagePropertiesA);
    std::vector<uint8_t> bufferA(bufferSizeA);

    // create a test image
    uint8_t* p = bufferA.data();
    for (uint32_t line = 0; line < imagePropertiesA.height; ++line)
    {
        for (uint32_t column = 0; column < imagePropertiesA.width; ++column, ++p)
        {
            *p = static_cast<uint8_t>(line + column);
        }
    }

    // writing
    cppbmpfile::operation_result result = cppbmpfile::bmp_file::save("TestImage.bmp", bufferA.data(), bufferA.size(), imagePropertiesA);
    std::cout << cppbmpfile::operation_result_type_to_string(result) << std::endl;

    // reading
    cppbmpfile::image_properties imagePropertiesB;
    if (cppbmpfile::bmp_file::load("TestImage.bmp", imagePropertiesB))
    {
        size_t bufferSizeB = cppbmpfile::bmp_file::compute_buffer_size(imagePropertiesB);
        std::vector<uint8_t> bufferB(bufferSizeB);
        result = cppbmpfile::bmp_file::load("TestImage.bmp", bufferB.data(), bufferB.size(), imagePropertiesB);
        std::cout << cppbmpfile::operation_result_type_to_string(result) << std::endl;
    }
}
```
