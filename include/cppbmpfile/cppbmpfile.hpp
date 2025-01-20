// cppbmpfile - A lean C++ implementation for loading and saving BMP files.
// Link: https://github.com/squeakycode/cppbmpfile
// Version: 0.9.0
// Minimum required C++ Standard: C++11
// License: BSD 3-Clause License
// 
// Copyright (c) 2025, Andreas Gau
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
\file
\brief Contains a lean C++ implementation for loading and saving BMP files.
\mainpage
C++ BMP File {#pageTitle}
================

##Purpose##
Load and save BMP files for image processing.


##Design Goals##
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

##When to Use##
- If you want to load and save BMP files for image processing.
*/
#pragma once
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <stdint.h>
#include <vector>

namespace cppbmpfile
{
    /// Defines what pixels are used by an image.
    enum class pixel_format
    {
        Mono8,  //!< 8 Bit pixel data as uint8_t luminance values.
        BGR8,   //!< 24 Bit pixel data in the order uint8_t blue, uint8_t green, uint8_t red. Sometimes confusingly referred to as RGB.
        BGRA8,  //!< 32 Bit pixel data in the order uint8_t blue, uint8_t green, uint8_t red, uint8_t alpha.Sometimes confusingly referred to as RGBA.
        invalid //!< Invalid value used for initialization purposes.
    };

    /// Defines at which line an image starts.
    enum class orientation_type
    {
        top_down, //!< The image starts at line zero.
        bottom_up, //!< The image starts at line (height - 1).
        invalid //!< Invalid value used for initialization purposes.
    };

    /// Holds the properties of an image.
    struct image_properties
    {
        uint32_t width = 0; //!< Width of the image in pixel.
        uint32_t height = 0; //!< Height of the image in lines.
        size_t line_padding = 0; //!< Additional bytes added to a line to align the beginning of a line to e.g. 4 bytes.
        pixel_format pixel_format = pixel_format::invalid; //!< Defines what pixels are used by the image.
        orientation_type orientation = orientation_type::bottom_up; //!< Defines at which line the image starts.
    };

    /// Defines at which line an image starts.
    enum class operation_result_type
    {
        ok, //!< Operation successful
        file_not_found, //!< The file could not be opened.
        file_open_for_writing_error, //<! Failed to open file for writing.
        file_read_error, //!< Reading the file failed.
        file_write_error, //!< Writing the file failed.
        buffer_too_small, //!< The provided buffer for reading or writing is too small.
        not_a_bmp_file, //!< The file that is loaded is not a BMP file.
        unsupported_compression, //!< Compression is currently not supported.
        unsupported_bit_per_pixel, //!< Some bit depth are currently not supported.
        unsupported_use_of_color_table, //!< Color table used for 24 or 32 bit formats.
        too_large_color_table, //!< Color table size implausible.
        corrupt, //!< The file that is loaded seems to be a BMP file, but checking the header data and the size of the file failed.
        null_argument, //!< null pointer passed as argument.
        invalid_argument, //!< argument given is not valid.
        invalid //!< Invalid value used for initialization purposes.
    };

    inline const char* operation_result_type_to_string(operation_result_type result_type)
    {
        switch (result_type)
        {
        case operation_result_type::ok:
            return "BMP file operation successful.";
        case operation_result_type::file_not_found:
            return "BMP file not found.";
        case operation_result_type::file_open_for_writing_error:
            return "Failed to open BMP file for writing.";
        case operation_result_type::file_read_error:
            return "BMP file read error.";
        case operation_result_type::file_write_error:
            return "BMP file write error.";
        case operation_result_type::buffer_too_small:
            return "Buffer too small for BMP file operation.";
        case operation_result_type::not_a_bmp_file:
            return "BMP file read error. Not a BMP.file.";
        case operation_result_type::unsupported_compression:
            return "BMP file read error. Compression type not supported.";
        case operation_result_type::unsupported_bit_per_pixel:
            return "BMP file read error. Bit per pixel not supported.";
        case operation_result_type::unsupported_use_of_color_table:
            return "BMP file read error. Color table variant not supported.";
        case operation_result_type::too_large_color_table:
            return "BMP file read error. Color table too large.";
        case operation_result_type::corrupt:
            return "BMP file read error. File has been corrupted.";
        case operation_result_type::null_argument:
            return "Argument must not be null.";
        case operation_result_type::invalid_argument:
            return "An argument passed is invalid.";
        case operation_result_type::invalid:
            return "Invalid operation type. No operation executed.";
        default:
            return "Unsupported operation result type.";
        }
    }

    /// Used for evaluating the result of an operation.
    class operation_result
    {
    public:
        /// Creates an invalid operation result.
        operation_result() = default;

        /// Creates and initializes an operation result.
        operation_result(operation_result_type result_type)
            : m_result_type(result_type)
        {
        }

        /// Returns true if the operation result is OK, false otherwise.
        operator bool() const
        {
            return m_result_type == operation_result_type::ok;
        }

        /// Returns the operation result.
        operator operation_result_type() const
        {
            return m_result_type;
        }

    private:
        operation_result_type m_result_type = operation_result_type::invalid;
    };

    /// Used for loading and saving data from and to a BMP file.
    class bmp_file
    {
    public:
        /**
            \brief Computes the buffer size needed to hold the image data.
            \param[out] the_image_properties  The properties of the image.
            \return Returns the buffer size or zero on invlaid arguments
        */
        static size_t compute_buffer_size(image_properties& the_image_properties)
        {
            size_t buffer_size = 0;
            if (!(
                  the_image_properties.height == 0
                || the_image_properties.width == 0
                || the_image_properties.pixel_format == pixel_format::invalid
                ))
            {
                buffer_size = determine_stride(the_image_properties) * the_image_properties.height;
            }
            return buffer_size;
        }

        /**
            \brief Loads the image properties. This can be used, e.g., to determine the needed buffer size.
            \param[in] filename  The name of the file.
            \param[out] the_image_properties  The properties of the image stored in the file.
            \return Returns information about the result of the operation.
        */
        template <typename char_type>
        static operation_result load(const char_type* filename, image_properties& the_image_properties)
        {
            operation_result result;
            bmp_header header = {};
            std::vector<color_table_entry> color_table;
            if (filename == nullptr)
            {
                result = operation_result_type::null_argument;
            }
            else
            {
                std::ifstream file(filename, std::ios::binary);
                result = load_image_properties(file, header, color_table, the_image_properties);
                file.close();
            }
            return result;
        }

        /**
            \brief Loads the image and its properties.
            \param[in] filename  The name of the file.
            \param[out] buffer  The buffer to store the image data in.
            \param[in] buffersize  The size of buffer.
            \param[inout] the_image_properties  The properties of the image stored in the file. Used as input parameter if the force flags are set to true.
            \param[in] force_line_padding  Use the line padding set in the_image_properties instead.
            \param[in] force_orientation  Use the orientation set in the_image_properties instead.
            \return Returns information about the result of the operation.
        */
        template <typename char_type>
        static operation_result load(const char_type* filename, void* buffer, size_t buffersize, image_properties& the_image_properties, bool force_line_padding = false, bool force_orientation = false)
        {
            operation_result result(operation_result_type::ok);
            bmp_header header = {};
            std::vector<color_table_entry> color_table;

            if (buffer == nullptr || filename == nullptr)
            {
                result = operation_result_type::null_argument;
            }
            else if (force_orientation && the_image_properties.orientation == orientation_type::invalid)
            {
                result = operation_result_type::invalid_argument;
            }
            else
            {
                std::ifstream file(filename, std::ios::binary);
                orientation_type suggested_orientation = the_image_properties.orientation;
                size_t suggested_line_padding = the_image_properties.line_padding;
                result = load_image_properties(file, header, color_table, the_image_properties);
                orientation_type orientation_in_file = the_image_properties.orientation;

                if (result)
                {
                    if (force_line_padding)
                    {
                        the_image_properties.line_padding = suggested_line_padding;
                    }
                    if (force_orientation)
                    {
                        the_image_properties.orientation = suggested_orientation;
                    }

                    const size_t stride_in_file = determine_stride(header.bits_per_pixel, header.width);
                    const size_t stride_in_buffer = determine_stride(the_image_properties);
                    const size_t line_padding_in_file = determine_line_padding(header.bits_per_pixel, header.width);
                    const size_t image_size_in_buffer = stride_in_buffer * abs(header.height);

                    if (image_size_in_buffer > buffersize)
                    {
                        result = operation_result_type::buffer_too_small;
                    }
                    else
                    {
                        file.seekg(header.offset);
                        if (!file)
                        {
                            result = operation_result_type::file_read_error;
                        }
                        else
                        {
                            if (header.bits_per_pixel > 8 && (the_image_properties.pixel_format == pixel_format::BGR8 || the_image_properties.pixel_format == pixel_format::BGRA8))
                            {
                                for (uint32_t line = 0; line < the_image_properties.height; ++line)
                                {
                                    uint8_t* p_target = reinterpret_cast<uint8_t*>(buffer);
                                    if (the_image_properties.orientation == orientation_in_file)
                                    {
                                        p_target += line * stride_in_buffer;
                                    }
                                    else
                                    {
                                        p_target += (the_image_properties.height - line - 1) * stride_in_buffer;
                                    }
                                    file.read(reinterpret_cast<char*>(p_target), stride_in_file - line_padding_in_file);
                                    if (!file)
                                    {
                                        result = operation_result_type::file_read_error;
                                        break;
                                    }
                                    if (line_padding_in_file)
                                    {
                                        file.seekg(line_padding_in_file, std::ios_base::cur);
                                        if (!file)
                                        {
                                            result = operation_result_type::file_write_error;
                                            break;
                                        }
                                    }
                                }
                            }
                            else if (the_image_properties.pixel_format == pixel_format::Mono8)
                            {
                                bool linear_lut = check_is_linear_mono8(color_table);
                                // read line by line, b == g == r in color table
                                for (int32_t line = 0; line < abs(header.height); ++line)
                                {
                                    uint8_t* p_target = reinterpret_cast<uint8_t*>(buffer);
                                    if (the_image_properties.orientation == orientation_in_file)
                                    {
                                        p_target += line * stride_in_buffer;
                                    }
                                    else
                                    {
                                        p_target += (the_image_properties.height - line - 1) * stride_in_buffer;
                                    }
                                    file.read(reinterpret_cast<char*>(p_target), stride_in_file - line_padding_in_file);
                                    if (!file)
                                    {
                                        result = operation_result_type::file_read_error;
                                        break;
                                    }
                                    if (!linear_lut)
                                    {
                                        for (int32_t column = 0; column < abs(header.width); ++column, ++p_target)
                                        {
                                            *p_target = color_table[*p_target].b;
                                        }
                                    }
                                    if (line_padding_in_file)
                                    {
                                        file.seekg(line_padding_in_file, std::ios_base::cur);
                                        if (!file)
                                        {
                                            result = operation_result_type::file_write_error;
                                            break;
                                        }
                                    }
                                }
                            }
                            else if (the_image_properties.pixel_format == pixel_format::BGR8 && header.bits_per_pixel == 8)
                            {
                                // read line by line, b != g != r somewhere in color table
                                std::vector<uint8_t> stride_buffer(stride_in_file);
                                for (int32_t line = 0; line < abs(header.height); ++line)
                                {
                                    file.read(reinterpret_cast<char*>(stride_buffer.data()), stride_buffer.size());
                                    if (file.gcount() != static_cast<std::streamsize>(stride_buffer.size()))
                                    {
                                        result = operation_result_type::file_read_error;
                                        break;
                                    }
                                    uint8_t* p_target = reinterpret_cast<uint8_t*>(buffer);
                                    if (the_image_properties.orientation == orientation_in_file)
                                    {
                                        p_target += line * stride_in_buffer;
                                    }
                                    else
                                    {
                                        p_target += (the_image_properties.height - line - 1) * stride_in_buffer;
                                    }
                                    for (int32_t column = 0; column < abs(header.width); ++column, ++p_target)
                                    {
                                        const color_table_entry& entry = color_table[stride_buffer[column]];
                                        *p_target = entry.b;
                                        ++p_target;
                                        *p_target = entry.g;
                                        ++p_target;
                                        *p_target = entry.r;
                                    }
                                }
                            }
                            else
                            {
                                result = operation_result_type::unsupported_bit_per_pixel;
                            }
                        }
                    }
                }
                file.close();
            }
            return result;
        }

        /**
            \brief Save the image and its properties.
            \param[in] filename  The name of the file.
            \param[in] buffer  The buffer holding the image data.
            \param[in] buffersize  The size of buffer.
            \param[in] the_image_properties  The properties of the image.
            \param[in] force_bottom_up  Force bottom up when saving to disk for best compatibility.
            \return Returns information about the result of the operation.
        */
        template <typename char_type>
        static operation_result save(const char_type* filename, const void* buffer, size_t buffersize, const image_properties& the_image_properties, bool force_bottom_up = true)
        {
            operation_result result(operation_result_type::ok);
            bmp_header header = {};
            std::vector<color_table_entry> color_table;

            if (buffer == nullptr || filename == nullptr)
            {
                result = operation_result_type::null_argument;
            }
            else if (
                the_image_properties.height == 0
                || the_image_properties.width == 0
                || the_image_properties.pixel_format == pixel_format::invalid
                || the_image_properties.orientation == orientation_type::invalid
                || buffersize == 0
                || determine_stride(the_image_properties) == 0
                )
            {
                result = operation_result_type::invalid_argument;
            }
            else if (determine_stride(the_image_properties) * the_image_properties.height > buffersize)
            {
                result = operation_result_type::buffer_too_small;
            }
            else
            {
                std::ofstream file(filename, std::ios::binary);
                if (!file.is_open())
                {
                    result = operation_result_type::file_open_for_writing_error;
                }
                else
                {
                    const size_t stride_in_buffer = determine_stride(the_image_properties);
                    orientation_type orientation_in_file = orientation_type::bottom_up;

                    // fill in intial values
                    header.type = 0x4D42;
                    header.size = sizeof(bmp_header);
                    header.reserved1 = 0;
                    header.reserved2 = 0;
                    header.offset = sizeof(bmp_header);
                    header.bitmap_info_header_size = sizeof(bmp_header) - 14;
                    header.width = static_cast<int32_t>(the_image_properties.width);
                    header.height = static_cast<int32_t>(the_image_properties.height);
                    if (!force_bottom_up && the_image_properties.orientation == orientation_type::top_down)
                    {
                        header.height = -header.height;
                        orientation_in_file = orientation_type::top_down;
                    }
                    header.num_planes = 1;
                    if (the_image_properties.pixel_format == pixel_format::Mono8)
                    {
                        header.bits_per_pixel = 8;
                    }
                    else if (the_image_properties.pixel_format == pixel_format::BGR8)
                    {
                        header.bits_per_pixel = 24;
                    }
                    else if (the_image_properties.pixel_format == pixel_format::BGRA8)
                    {
                        header.bits_per_pixel = 32;
                    }
                    else
                    {
                        assert(false); //invalid path
                    }
                    header.compression = 0; // BI_RGB, no compression
                    header.image_size_bytes = 0; //keeping it 0
                    header.x_resolution = 0;
                    header.y_resolution = 0;
                    header.num_colors = 0;
                    header.important_colors = 0;

                    if (the_image_properties.pixel_format == pixel_format::Mono8)
                    {
                        header.size += 256 * sizeof(color_table_entry);
                        header.offset += 256 * sizeof(color_table_entry);
                        header.num_colors = 256;
                        header.important_colors = 256;
                    }

                    const size_t stride_in_file = determine_stride(header.bits_per_pixel, header.width);
                    const size_t image_size_in_file = stride_in_file * abs(header.height);
                    header.size += static_cast<uint32_t>(image_size_in_file);

                    // write header
                    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
                    if (!file)
                    {
                        result = operation_result_type::file_write_error;
                    }
                    // write color table if needed
                    if (result && the_image_properties.pixel_format == pixel_format::Mono8)
                    {
                        color_table_entry entry;
                        entry.reserved = 255;
                        for (size_t i = 0; i < 256; ++i)
                        {
                            entry.b = entry.g = entry.r = static_cast<uint8_t>(i);
                            file.write(reinterpret_cast<const char*>(&entry), sizeof(entry));
                            if (!file)
                            {
                                result = operation_result_type::file_write_error;
                                break;
                            }
                        }
                    }
                    // write image data, linewise
                    const size_t line_padding_in_file = determine_line_padding(header.bits_per_pixel, header.width);
                    for (uint32_t line = 0; line < the_image_properties.height; ++line)
                    {
                        const uint8_t* p_source = reinterpret_cast<const uint8_t*>(buffer);
                        if (the_image_properties.orientation == orientation_in_file)
                        {
                            p_source += line * stride_in_buffer;
                        }
                        else
                        {
                            p_source += (the_image_properties.height - line - 1) * stride_in_buffer;
                        }
                        file.write(reinterpret_cast<const char*>(p_source), stride_in_buffer - the_image_properties.line_padding);
                        if (!file)
                        {
                            result = operation_result_type::file_write_error;
                            break;
                        }
                        if (line_padding_in_file && line_padding_in_file <= 4)
                        {
                            const uint32_t padding = 0;
                            file.write(reinterpret_cast<const char*>(&padding), line_padding_in_file);
                            if (!file)
                            {
                                result = operation_result_type::file_write_error;
                                break;
                            }
                        }
                        else if (line_padding_in_file == 0)
                        {
                            // nothing to do
                        }
                        else
                        {
                            assert(false); // invalid path
                        }
                    }
                }
            }

            return result;
        }
    private:

#pragma pack(push)
#pragma pack(1)
        struct bmp_header
        {
            uint16_t  type;             //!< Header magic value.
            uint32_t  size;             //!< Image file size in bytes.
            uint16_t  reserved1;        //!< Not used
            uint16_t  reserved2;        //!< Not used
            uint32_t  offset;           //!< Offset in bytes to image data in bytes from beginning of file.
            uint32_t  bitmap_info_header_size;  //!< Bitmap info header size in bytes (40 bytes).
            int32_t   width;           //!< Width of the image.
            int32_t   height   ;        //!< Height of image.
            uint16_t  num_planes;       //!< Number of color planes.
            uint16_t  bits_per_pixel;   //!< Bits per pixel.
            uint32_t  compression;      //!< Compression type.
            uint32_t  image_size_bytes; //!< Image size in bytes.
            int32_t   x_resolution;     //!< X resolution in pixels per meter.
            int32_t   y_resolution;     //!< Y resolution in pixels per meter.
            uint32_t  num_colors;       //!< Number of colors.
            uint32_t  important_colors; //!< Important colors.
        };

        struct color_table_entry
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t reserved;
        };
#pragma pack(pop)

        static size_t byte_per_pixel_in_file(uint16_t  bits_per_pixel)
        {
            if (bits_per_pixel == 8)
            {
                return 1;
            }
            if (bits_per_pixel == 24)
            {
                return 3;
            }
            if (bits_per_pixel == 32)
            {
                return 4;
            }
            assert(false); // bits_per_pixel is expected to be checked before calling this helper method
            return 0;
        }

        static size_t determine_stride(uint16_t bits_per_pixel, int32_t width)
        {
            size_t stride = byte_per_pixel_in_file(bits_per_pixel) * abs(width);
            bool dividableByFour = stride % 4 == 0;
            if (!dividableByFour)
            {
                stride += 4 - stride % 4;
            }
            return stride;
        }

        static size_t determine_line_padding(uint16_t bits_per_pixel, int32_t width)
        {
            size_t line_padding = 0;
            size_t stride = byte_per_pixel_in_file(bits_per_pixel) * abs(width);
            bool dividableByFour = stride % 4 == 0;
            if (!dividableByFour)
            {
                line_padding = 4 - stride % 4;
            }
            return line_padding;
        }

        static size_t determine_stride(const image_properties& the_image_properties)
        {
            size_t byte_per_pixel = 0;
            if (the_image_properties.pixel_format == pixel_format::Mono8)
            {
                byte_per_pixel = 1;
            }
            else if (the_image_properties.pixel_format == pixel_format::BGR8)
            {
                byte_per_pixel = 3;
            }
            else if (the_image_properties.pixel_format == pixel_format::BGRA8)
            {
                byte_per_pixel = 4;
            }
            else
            {
                assert(false); // invalid path
            }

            size_t stride = the_image_properties.width * byte_per_pixel + the_image_properties.line_padding;
            return stride;
        }

        static operation_result load_and_check_header(std::istream& stream, bmp_header& header_out)
        {
            operation_result result(operation_result_type::ok);
            const bmp_header& header = header_out;

            stream.read(reinterpret_cast<char*>(&header_out), sizeof(header_out));
            if (!stream)
            {
                result = operation_result_type::not_a_bmp_file;
            }
            else
            {
                // header.type; -> expected to be BM
                // header.size; -> ignored, because it is unreliable
                // header.reserved1; -> ignored
                // header.reserved2; -> ignored
                // header.offset; -> checked
                // header.dib_header_size; -> checked
                // header.widthx; -> checked
                // header.height; -> checked
                // header.num_planes; -> ignored, not used for bmp
                // header.bits_per_pixel; -> checked
                // header.compression; -> checked
                // header.image_size_bytes; -> checked
                // header.x_resolution; -> ignored, not used
                // header.y_resolution; -> ignored, not used
                // header.num_colors; -> checked
                // header.important_colors; -> checked
            
                //check the header
                if (header.type != 0x4D42)
                {
                    result = operation_result_type::not_a_bmp_file;
                }
                else if (header.bitmap_info_header_size < 40)
                {
                    result = operation_result_type::corrupt;
                }
                else if (header.offset < sizeof(header))
                {
                    result = operation_result_type::corrupt;
                }
                else if (header.height == 0 || header.width <= 0)
                {
                    result = operation_result_type::corrupt;
                }
                else if (!(header.bits_per_pixel == 1 || header.bits_per_pixel == 4 || header.bits_per_pixel == 8 || header.bits_per_pixel == 16 || header.bits_per_pixel == 24 || header.bits_per_pixel == 32))
                {
                    result = operation_result_type::corrupt;
                }
                else if (header.compression != 0 /* BI_RGB, no compression */)
                {
                    result = operation_result_type::unsupported_compression;
                }
                else if (!(header.bits_per_pixel == 8 || header.bits_per_pixel == 24 || header.bits_per_pixel == 32))
                {
                    result = operation_result_type::unsupported_bit_per_pixel;
                }
                else if ((header.bits_per_pixel == 24 || header.bits_per_pixel == 32) && (header.num_colors != 0 || header.important_colors != 0))
                {
                    result = operation_result_type::unsupported_use_of_color_table;
                }
                else if (header.bits_per_pixel == 8 && (header.num_colors > 256 || header.important_colors > 256))
                {
                    result = operation_result_type::too_large_color_table;
                }
                else
                {
                    size_t image_data_size = determine_stride(header.bits_per_pixel, header.width) * abs(header.height);

                    if (!(header.image_size_bytes == 0 || header.image_size_bytes == image_data_size))
                    {
                        result = operation_result_type::corrupt;
                    }
                }
            }

            return result;
        }

        static operation_result load_color_table(std::istream& stream, const bmp_header& header, std::vector<color_table_entry>& color_table_out)
        {
            operation_result result(operation_result_type::ok);
            stream.seekg(header.bitmap_info_header_size + 14 /* file header size */);
            if (!stream)
            {
                result = operation_result_type::file_read_error;
            }
            else
            {
                if (header.num_colors)
                {
                    color_table_out.resize(header.num_colors);
                }
                else if (header.num_colors == 0 && header.bits_per_pixel == 1)
                {
                    color_table_out.resize(1);
                }
                else if (header.num_colors == 0 && header.bits_per_pixel == 4)
                {
                    color_table_out.resize(16);
                }
                else if (header.num_colors == 0 && header.bits_per_pixel == 8)
                {
                    color_table_out.resize(256);
                }

                if (color_table_out.size() != 0)
                {
                    const size_t size_of_color_table_bytes = color_table_out.size() * sizeof(color_table_entry);
                    stream.read(reinterpret_cast<char*>(color_table_out.data()), size_of_color_table_bytes);
                    if (!stream)
                    {
                        result = operation_result_type::file_read_error;
                    }
                }
                else
                {
                    result = operation_result_type::unsupported_use_of_color_table;
                }
            }
            return result;
        }

        static bool check_is_linear_mono8(const std::vector<color_table_entry>& color_table)
        {
            uint8_t i = 0;
            for (const color_table_entry& entry : color_table)
            {
                if (!(entry.b == i && entry.g == i && entry.r == i))
                {
                    return false;
                }
                ++i;
            }
            return true;
        }

        static bool check_is_mono8(const std::vector<color_table_entry>& color_table)
        {
            for (const color_table_entry& entry : color_table)
            {
                // not all values are equal
                if (!(entry.r == entry.g && entry.r == entry.b))
                {
                    return false;
                }
            }
            return true;
        }

        static void determine_image_properties(const bmp_header& header, std::vector<color_table_entry>& color_table_out, image_properties& the_image_properties)
        {
            
            the_image_properties.height = abs(header.height);
            the_image_properties.width = abs(header.width);
            if (header.bits_per_pixel <= 8)
            {
                if (check_is_mono8(color_table_out))
                {
                    the_image_properties.pixel_format = pixel_format::Mono8;
                }
                else
                {
                    the_image_properties.pixel_format = pixel_format::BGR8;
                }
            }
            else if (header.bits_per_pixel == 24)
            {
                the_image_properties.pixel_format = pixel_format::BGR8;
            }
            else if (header.bits_per_pixel == 32)
            {
                the_image_properties.pixel_format = pixel_format::BGRA8;
            }
            else
            {
                assert(false); // not supported yet
            }
            
            the_image_properties.orientation = header.height < 0 ? orientation_type::top_down : orientation_type::bottom_up;
            the_image_properties.line_padding = determine_line_padding(header.bits_per_pixel, header.width);
        }

        static operation_result load_image_properties(std::ifstream& stream, bmp_header& header, std::vector<color_table_entry>& color_table_out, image_properties& the_image_properties)
        {
            operation_result result;

            if (!stream.is_open())
            {
                result = operation_result_type::file_not_found;
            }
            else
            {
                result = load_and_check_header(stream, header);
            }

            if (result && header.bits_per_pixel == 8)
            {
                result = load_color_table(stream, header, color_table_out);
            }

            if (result)
            {
                determine_image_properties(header, color_table_out, the_image_properties);
            }
            else
            {
                the_image_properties = image_properties(); // clear
            }

            return result;
        }
    };
}
