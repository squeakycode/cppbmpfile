#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include <cppbmpfile/cppbmpfile.hpp>
#include <string>

static const size_t test_file_width = 90;
static const size_t test_file_height = 100;
static const size_t test_file_padding = 2;
TEST_CASE("result type to string", "[cpp_bmp_file]")
{
    CHECK(std::string(operation_result_type_to_string(cppbmpfile::operation_result_type::ok)) == "BMP file operation successful.");
}

TEST_CASE("test invalid arguments", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties props;
    cppbmpfile::operation_result result;

    props.height = test_file_height;
    props.width = test_file_width;
    props.pixel_format = cppbmpfile::pixel_format_type::Mono8;
    props.orientation = cppbmpfile::orientation_type::bottom_up;
    props.line_padding = test_file_padding;
    size_t buffer_size = cppbmpfile::bmp_file::compute_buffer_size(props);
    CHECK(buffer_size == (test_file_width + test_file_padding) * test_file_height);
    std::vector<uint8_t> buffer(buffer_size);

    props.height = 0;
    buffer_size = cppbmpfile::bmp_file::compute_buffer_size(props);
    CHECK(buffer_size == 0);
    result = cppbmpfile::bmp_file::save("dontcare", buffer.data(), buffer.size(), props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::invalid_argument);
    props.height = test_file_height;

    props.width = 0;
    buffer_size = cppbmpfile::bmp_file::compute_buffer_size(props);
    CHECK(buffer_size == 0);
    result = cppbmpfile::bmp_file::save("dontcare", buffer.data(), buffer.size(), props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::invalid_argument);
    props.width = test_file_width;

    props.pixel_format = cppbmpfile::pixel_format_type::invalid;
    buffer_size = cppbmpfile::bmp_file::compute_buffer_size(props);
    CHECK(buffer_size == 0);
    result = cppbmpfile::bmp_file::save("dontcare", buffer.data(), buffer.size(), props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::invalid_argument);
    props.pixel_format = cppbmpfile::pixel_format_type::Mono8;

    props.orientation = cppbmpfile::orientation_type::invalid;
    buffer_size = cppbmpfile::bmp_file::compute_buffer_size(props);
    CHECK(buffer_size == (test_file_width + test_file_padding) * test_file_height);
    result = cppbmpfile::bmp_file::save("dontcare", buffer.data(), buffer.size(), props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::invalid_argument);
    result = cppbmpfile::bmp_file::load("dontcare", buffer.data(), buffer.size(), props, false, true);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::invalid_argument);
    props.orientation = cppbmpfile::orientation_type::bottom_up;

    result = cppbmpfile::bmp_file::save((const char*)nullptr, buffer.data(), buffer.size(), props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::null_argument);
    props.orientation = cppbmpfile::orientation_type::bottom_up;

    result = cppbmpfile::bmp_file::save("dontcare", nullptr, buffer.size(), props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::null_argument);
    props.orientation = cppbmpfile::orientation_type::bottom_up;

    result = cppbmpfile::bmp_file::save("dontcare", buffer.data(), 0, props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::invalid_argument);
    props.orientation = cppbmpfile::orientation_type::bottom_up;

    result = cppbmpfile::bmp_file::save("Mono8_out.bmp", buffer.data(), 10, props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::buffer_too_small);


    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/NotThere.bmp", props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::file_not_found);

    result = cppbmpfile::bmp_file::load((const char*)nullptr, props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::null_argument);


    result = cppbmpfile::bmp_file::load((const char*)nullptr, buffer.data(), buffer.size(), props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::null_argument);

    result = cppbmpfile::bmp_file::load("dontcare", nullptr, buffer.size(), props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::null_argument);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/Mono8.bmp", buffer.data(), 10, props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::buffer_too_small);
}



TEST_CASE("test image properties", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties props;
    cppbmpfile::operation_result result;

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/TooSmall.bmp", props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::not_a_bmp_file);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/Mono8.bmp", props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::ok);
    CHECK(result);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::Mono8);
    CHECK(props.line_padding == test_file_padding);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/Mono8_flipped.bmp", props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::ok);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up); //test file to be adapted
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::Mono8);
    CHECK(props.line_padding == test_file_padding);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGR8.bmp", props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::ok);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::BGR8);
    CHECK(props.line_padding == test_file_padding);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/256_color.bmp", props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::ok);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::BGR8);
    CHECK(props.line_padding == test_file_padding);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGR8_flipped.bmp", props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::ok);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up); //test file to be adapted
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::BGR8);
    CHECK(props.line_padding == test_file_padding);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGRA8.bmp", props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::ok);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::BGRA8);
    CHECK(props.line_padding == 0);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGRA8_flipped.bmp", props);
    CHECK(result.operator cppbmpfile::operation_result_type() == cppbmpfile::operation_result_type::ok);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);  //test file to be adapted
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::BGRA8);
    CHECK(props.line_padding == 0);
}

TEST_CASE("test load mono8", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties props;
    cppbmpfile::operation_result result;
    std::vector<uint8_t> buffer((test_file_width + test_file_padding) * test_file_height);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/Mono8.bmp", buffer.data(), buffer.size(), props);
    CHECK(result);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::Mono8);
    CHECK(props.line_padding == test_file_padding);

    //check the test pattern
    for (size_t line = 0; line < test_file_height; ++line)
    {
        for (size_t column = 0; column < test_file_width; ++column)
        {
            size_t expected = column + (test_file_height - line); //bottom up
            size_t offset = (test_file_width + test_file_padding) * line + column;
            if (buffer[offset] != expected)
            {
                CHECK(static_cast<size_t>(buffer[offset]) == expected);
                break;
            }
        }
    }
}

TEST_CASE("test load mono8 non linear color table", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties props;
    cppbmpfile::operation_result result;
    std::vector<uint8_t> buffer((test_file_width + test_file_padding) * test_file_height);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/Mono8_non_linear.bmp", buffer.data(), buffer.size(), props);
    CHECK(result);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::Mono8);
    CHECK(props.line_padding == test_file_padding);

    //check the test pattern
    for (size_t line = 0; line < test_file_height; ++line)
    {
        for (size_t column = 0; column < test_file_width; ++column)
        {
            size_t expected = column + (test_file_height - line); //bottom up
            if (expected == 0x13)
            {
                expected = 0x14;
            }
            else if (expected == 0x14)
            {
                expected = 0x13;
            }
            size_t offset = (test_file_width + test_file_padding) * line + column;
            if (buffer[offset] != expected)
            {
                CHECK(static_cast<size_t>(buffer[offset]) == expected);
                break;
            }
        }
    }
}

TEST_CASE("test load bgr8", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties props;
    cppbmpfile::operation_result result;
    std::vector<uint8_t> buffer((test_file_width * 3 + test_file_padding) * test_file_height);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGR8.bmp", buffer.data(), buffer.size(), props);
    CHECK(result);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::BGR8);
    CHECK(props.line_padding == test_file_padding);
    static const size_t byte_per_pixel = 3;

    //check the test pattern
    for (size_t line = 0; line < test_file_height; ++line)
    {
        for (size_t column = 0; column < test_file_width; ++column)
        {
            size_t expected = column + (test_file_height - line); //bottom up
            size_t offset = (test_file_width * byte_per_pixel + test_file_padding) * line + column * byte_per_pixel;

            if (buffer[offset] != expected)
            {
                CHECK(static_cast<size_t>(buffer[offset]) == expected);
                break;
            }
            if (buffer[offset + 1] != expected)
            {
                CHECK(static_cast<size_t>(buffer[offset + 1]) == expected);
                break;
            }
            if (buffer[offset + 2] != expected)
            {
                CHECK(static_cast<size_t>(buffer[offset + 2]) == expected);
                break;
            }
        }
    }
}

TEST_CASE("test load bgra8", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties props;
    cppbmpfile::operation_result result;
    std::vector<uint8_t> buffer((test_file_width * 4) * test_file_height);

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGRA8.bmp", buffer.data(), buffer.size(), props);
    CHECK(result);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::BGRA8);
    CHECK(props.line_padding == 0);
    static const size_t byte_per_pixel = 4;

    //check the test pattern
    for (size_t line = 0; line < test_file_height; ++line)
    {
        for (size_t column = 0; column < test_file_width; ++column)
        {
            size_t expected = column + (test_file_height - line); //bottom up
            size_t offset = (test_file_width * byte_per_pixel) * line + column * byte_per_pixel;

            if (buffer[offset] != expected)
            {
                CHECK(static_cast<size_t>(buffer[offset]) == expected);
                break;
            }
            if (buffer[offset + 1] != expected)
            {
                CHECK(static_cast<size_t>(buffer[offset + 1]) == expected);
                break;
            }
            if (buffer[offset + 2] != expected)
            {
                CHECK(static_cast<size_t>(buffer[offset + 2]) == expected);
                break;
            }
            if (buffer[offset + 3] != 255)
            {
                CHECK(static_cast<size_t>(buffer[offset + 3]) == 255);
                break;
            }
        }
    }
}

TEST_CASE("test load 256 color", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties props;
    cppbmpfile::operation_result result;
    std::vector<uint8_t> buffer((test_file_width * 3 + test_file_padding) * test_file_height);


    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/256_color.bmp", buffer.data(), buffer.size(), props);
    CHECK(result);
    CHECK(props.height == test_file_height);
    CHECK(props.width == test_file_width);
    CHECK(props.orientation == cppbmpfile::orientation_type::bottom_up);
    CHECK(props.pixel_format == cppbmpfile::pixel_format_type::BGR8);
    CHECK(props.line_padding == test_file_padding);
    static const size_t byte_per_pixel = 3;

    //check the test pattern
    for (size_t line = 0; line < test_file_height; ++line)
    {
        for (size_t column = 0; column < test_file_width; ++column)
        {
            size_t expected = column + (test_file_height - line); //bottom up
            size_t offset = (test_file_width * byte_per_pixel + test_file_padding) * line + column * byte_per_pixel;
            if (expected == 32) //color table modified
            {
                if (buffer[offset] != expected)
                {
                    CHECK(static_cast<size_t>(buffer[offset]) == 32);
                    break;
                }
                if (buffer[offset + 1] != expected)
                {
                    CHECK(static_cast<size_t>(buffer[offset + 1]) == 33);
                    break;
                }
                if (buffer[offset + 2] != expected)
                {
                    CHECK(static_cast<size_t>(buffer[offset + 2]) == 34);
                    break;
                }
            }
            else
            {
                if (buffer[offset] != expected)
                {
                    CHECK(static_cast<size_t>(buffer[offset]) == expected);
                    break;
                }
                if (buffer[offset + 1] != expected)
                {
                    CHECK(static_cast<size_t>(buffer[offset + 1]) == expected);
                    break;
                }
                if (buffer[offset + 2] != expected)
                {
                    CHECK(static_cast<size_t>(buffer[offset + 2]) == expected);
                    break;
                }
            }
        }
    }
}

TEST_CASE("test load save load mono8", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties propsA;
    cppbmpfile::image_properties propsB;
    cppbmpfile::image_properties propsC;
    cppbmpfile::operation_result result;
    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/Mono8.bmp", propsA);
    CHECK(result);

    size_t buffer_size = cppbmpfile::bmp_file::compute_buffer_size(propsA);
    CHECK(buffer_size == (test_file_width + test_file_padding) * test_file_height);
    std::vector<uint8_t> bufferB(buffer_size);
    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/Mono8.bmp", bufferB.data(), bufferB.size(), propsB);
    CHECK(result);
    CHECK(propsA.height == propsB.height);
    CHECK(propsA.width == propsB.width);
    CHECK(propsA.pixel_format == propsB.pixel_format);
    CHECK(propsA.orientation == propsB.orientation);
    CHECK(propsA.line_padding == propsB.line_padding);
#if defined(WIN32)
    result = cppbmpfile::bmp_file::save(L"Mono8_out♥.bmp", bufferB.data(), bufferB.size(), propsB);
#else
    result = cppbmpfile::bmp_file::save("Mono8_out♥.bmp", bufferB.data(), bufferB.size(), propsB);
#endif
    CHECK(result);

    std::vector<uint8_t> bufferC(buffer_size);
#if defined(WIN32)
    result = cppbmpfile::bmp_file::load(L"Mono8_out♥.bmp", bufferC.data(), bufferC.size(), propsC);
#else
    result = cppbmpfile::bmp_file::load("Mono8_out♥.bmp", bufferC.data(), bufferC.size(), propsC);
#endif
    CHECK(result);
    CHECK(propsA.height == propsC.height);
    CHECK(propsA.width == propsC.width);
    CHECK(propsA.pixel_format == propsC.pixel_format);
    CHECK(propsA.orientation == propsC.orientation);
    CHECK(propsA.line_padding == propsC.line_padding);

    CHECK(bufferB == bufferC);

    propsB.orientation = cppbmpfile::orientation_type::top_down;
    result = cppbmpfile::bmp_file::save("Mono8_out_flipped.bmp", bufferB.data(), bufferB.size(), propsB);
    CHECK(result);
    std::vector<uint8_t> bufferD(buffer_size);
    result = cppbmpfile::bmp_file::load("Mono8_out_flipped.bmp", bufferD.data(), bufferD.size(), propsB, false, true);
    CHECK(result);
    CHECK(bufferB == bufferD);
}

TEST_CASE("test load save load bgr8", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties propsA;
    cppbmpfile::image_properties propsB;
    cppbmpfile::image_properties propsC;
    cppbmpfile::operation_result result;
    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGR8.bmp", propsA);
    CHECK(result);

    size_t buffer_size = cppbmpfile::bmp_file::compute_buffer_size(propsA);
    CHECK(buffer_size == (test_file_width * 3 + test_file_padding) * test_file_height);
    std::vector<uint8_t> bufferB(buffer_size);
    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGR8.bmp", bufferB.data(), bufferB.size(), propsB);
    CHECK(result);
    CHECK(propsA.height == propsB.height);
    CHECK(propsA.width == propsB.width);
    CHECK(propsA.pixel_format == propsB.pixel_format);
    CHECK(propsA.orientation == propsB.orientation);
    CHECK(propsA.line_padding == propsB.line_padding);

    result = cppbmpfile::bmp_file::save("BGR8_out.bmp", bufferB.data(), bufferB.size(), propsB);
    CHECK(result);

    std::vector<uint8_t> bufferC(buffer_size);
    result = cppbmpfile::bmp_file::load("BGR8_out.bmp", bufferC.data(), bufferC.size(), propsC);
    CHECK(result);
    CHECK(propsA.height == propsC.height);
    CHECK(propsA.width == propsC.width);
    CHECK(propsA.pixel_format == propsC.pixel_format);
    CHECK(propsA.orientation == propsC.orientation);
    CHECK(propsA.line_padding == propsC.line_padding);

    CHECK(bufferB == bufferC);
}

TEST_CASE("test load save load bgra8", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties propsA;
    cppbmpfile::image_properties propsB;
    cppbmpfile::image_properties propsC;
    cppbmpfile::operation_result result;
    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGRA8.bmp", propsA);
    CHECK(result);

    size_t buffer_size = cppbmpfile::bmp_file::compute_buffer_size(propsA);
    CHECK(buffer_size == (test_file_width * 4) * test_file_height);
    std::vector<uint8_t> bufferB(buffer_size);
    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGRA8.bmp", bufferB.data(), bufferB.size(), propsB);
    CHECK(result);
    CHECK(propsA.height == propsB.height);
    CHECK(propsA.width == propsB.width);
    CHECK(propsA.pixel_format == propsB.pixel_format);
    CHECK(propsA.orientation == propsB.orientation);
    CHECK(propsA.line_padding == propsB.line_padding);

    result = cppbmpfile::bmp_file::save("BGRA8_out.bmp", bufferB.data(), bufferB.size(), propsB);
    CHECK(result);

    std::vector<uint8_t> bufferC(buffer_size);
    result = cppbmpfile::bmp_file::load("BGRA8_out.bmp", bufferC.data(), bufferC.size(), propsC);
    CHECK(result);
    CHECK(propsA.height == propsC.height);
    CHECK(propsA.width == propsC.width);
    CHECK(propsA.pixel_format == propsC.pixel_format);
    CHECK(propsA.orientation == propsC.orientation);
    CHECK(propsA.line_padding == propsC.line_padding);

    CHECK(bufferB == bufferC);

    propsB.orientation = cppbmpfile::orientation_type::top_down;
    result = cppbmpfile::bmp_file::save("BGRA8_out_flipped.bmp", bufferB.data(), bufferB.size(), propsB);
    CHECK(result);
    std::vector<uint8_t> bufferD(buffer_size);
    result = cppbmpfile::bmp_file::load("BGRA8_out_flipped.bmp", bufferD.data(), bufferD.size(), propsB, false, true);
    CHECK(result);
    CHECK(bufferB == bufferD);
}

TEST_CASE("test load save load bgr8 flipped", "[cpp_bmp_file]")
{
    cppbmpfile::image_properties propsA;
    cppbmpfile::image_properties propsB;
    cppbmpfile::image_properties propsC;
    cppbmpfile::operation_result result;
    static const size_t byte_per_pixel = 3;

    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGR8.bmp", propsA);
    CHECK(result);

    size_t buffer_size = cppbmpfile::bmp_file::compute_buffer_size(propsA);
    CHECK(buffer_size == (test_file_width * 3 + test_file_padding) * test_file_height);
    std::vector<uint8_t> bufferB(buffer_size);
    result = cppbmpfile::bmp_file::load(TEST_DATA_ROOT_PATH "/testimages/BGR8.bmp", bufferB.data(), bufferB.size(), propsB);
    CHECK(result);
    CHECK(propsA.height == propsB.height);
    CHECK(propsA.width == propsB.width);
    CHECK(propsA.pixel_format == propsB.pixel_format);
    CHECK(propsA.orientation == propsB.orientation);
    CHECK(propsA.line_padding == propsB.line_padding);

    propsB.orientation = cppbmpfile::orientation_type::top_down;
    result = cppbmpfile::bmp_file::save("BGR8_out_flipped.bmp", bufferB.data(), bufferB.size(), propsB);
    CHECK(result);

    std::vector<uint8_t> bufferC(buffer_size);
    result = cppbmpfile::bmp_file::load("BGR8_out_flipped.bmp", bufferC.data(), bufferC.size(), propsC);
    CHECK(result);
    CHECK(propsA.height == propsC.height);
    CHECK(propsA.width == propsC.width);
    CHECK(propsA.pixel_format == propsC.pixel_format);
    CHECK(propsC.orientation == cppbmpfile::orientation_type::bottom_up); // always save as bottom_up
    CHECK(propsA.line_padding == propsC.line_padding);

    CHECK(bufferB != bufferC); // flipped

    for (size_t line = 0; line < test_file_height; ++line)
    {
        for (size_t column = 0; column < test_file_width; ++column)
        {
            size_t offsetB = (test_file_width * byte_per_pixel + test_file_padding) * line + column * byte_per_pixel;
            size_t offsetC = (test_file_width * byte_per_pixel + test_file_padding) * (test_file_height - line - 1) + column * byte_per_pixel;
            if (bufferB[offsetB] != bufferC[offsetC])
            {
                CHECK(bufferB[offsetB] == bufferC[offsetC]);
            }
            if (bufferB[offsetB + 1] != bufferC[offsetC + 1])
            {
                CHECK(bufferB[offsetB + 1] == bufferC[offsetC + 1]);
            }
            if (bufferB[offsetB + 2] != bufferC[offsetC + 2])
            {
                CHECK(bufferB[offsetB + 2] == bufferC[offsetC + 2]);
            }
        }
    }

    std::vector<uint8_t> bufferD(buffer_size);
    propsC.orientation = cppbmpfile::orientation_type::top_down;
    result = cppbmpfile::bmp_file::load("BGR8_out_flipped.bmp", bufferD.data(), bufferD.size(), propsC, false, true);
    CHECK(bufferB == bufferD); // flipped back
}

inline void test_variants(const char* filename, const char* new_name, size_t target_padding, cppbmpfile::orientation_type target_orientation)
{
    cppbmpfile::image_properties props;
    cppbmpfile::operation_result result;
    result = cppbmpfile::bmp_file::load(filename, props);
    CHECK(result);
    props.orientation = target_orientation;
    props.line_padding = target_padding;
    size_t buffer_size = cppbmpfile::bmp_file::compute_buffer_size(props);
    std::vector<uint8_t> buffer(buffer_size);
    result = cppbmpfile::bmp_file::load(filename, buffer.data(), buffer.size(), props, true /* force line padding */, true /* force orientation */);
    CHECK(result);
    CHECK(props.line_padding == target_padding);
    CHECK(props.orientation == target_orientation);
    result = cppbmpfile::bmp_file::save(new_name, buffer.data(), buffer.size(), props, false /* do not force bottom up in file*/);
    CHECK(result);
}

inline void test_compare_variants(const char* filename, const char* new_name)
{
    cppbmpfile::image_properties propsA;
    cppbmpfile::image_properties propsB;
    cppbmpfile::operation_result result;
    result = cppbmpfile::bmp_file::load(filename, propsA);
    CHECK(result);
    result = cppbmpfile::bmp_file::load(new_name, propsB);
    CHECK(result);
    CHECK(propsA.height == propsB.height);
    CHECK(propsA.width == propsB.width);
    CHECK(propsA.pixel_format == propsB.pixel_format);
    CHECK(propsA.orientation == propsB.orientation);
    CHECK(propsA.line_padding == propsB.line_padding);
    std::vector<uint8_t> bufferA(cppbmpfile::bmp_file::compute_buffer_size(propsA));
    std::vector<uint8_t> bufferB(cppbmpfile::bmp_file::compute_buffer_size(propsB));
    CHECK(bufferA.size() == bufferB.size());
    result = cppbmpfile::bmp_file::load(filename, bufferA.data(), bufferA.size(), propsA);
    CHECK(result);
    result = cppbmpfile::bmp_file::load(new_name, bufferB.data(), bufferB.size(), propsB);
    CHECK(result);
    CHECK(bufferA == bufferB);
}

TEST_CASE("test load save variants", "[cpp_bmp_file]")
{
    test_variants(TEST_DATA_ROOT_PATH "/testimages/Mono8.bmp", "V1_Mono8.bmp", 30, cppbmpfile::orientation_type::top_down);
    test_variants("V1_Mono8.bmp", "V2_Mono8.bmp", 0, cppbmpfile::orientation_type::bottom_up);
    test_variants("V2_Mono8.bmp", "V3_Mono8.bmp", 0, cppbmpfile::orientation_type::top_down);
    test_variants("V3_Mono8.bmp", "V4_Mono8.bmp", 50, cppbmpfile::orientation_type::bottom_up);
    test_compare_variants(TEST_DATA_ROOT_PATH "/testimages/Mono8.bmp", "V4_Mono8.bmp");

    test_variants(TEST_DATA_ROOT_PATH "/testimages/BGR8.bmp", "V1_BGR8.bmp", 30, cppbmpfile::orientation_type::top_down);
    test_variants("V1_BGR8.bmp", "V2_BGR8.bmp", 0, cppbmpfile::orientation_type::bottom_up);
    test_variants("V2_BGR8.bmp", "V3_BGR8.bmp", 0, cppbmpfile::orientation_type::top_down);
    test_variants("V3_BGR8.bmp", "V4_BGR8.bmp", 50, cppbmpfile::orientation_type::bottom_up);
    test_compare_variants(TEST_DATA_ROOT_PATH "/testimages/BGR8.bmp", "V4_BGR8.bmp");

    test_variants(TEST_DATA_ROOT_PATH "/testimages/BGRA8.bmp", "V1_BGRA8.bmp", 30, cppbmpfile::orientation_type::top_down);
    test_variants("V1_BGRA8.bmp", "V2_BGRA8.bmp", 0, cppbmpfile::orientation_type::bottom_up);
    test_variants("V2_BGRA8.bmp", "V3_BGRA8.bmp", 0, cppbmpfile::orientation_type::top_down);
    test_variants("V3_BGRA8.bmp", "V4_BGRA8.bmp", 50, cppbmpfile::orientation_type::bottom_up);
    test_compare_variants(TEST_DATA_ROOT_PATH "/testimages/BGRA8.bmp", "V4_BGRA8.bmp");

    test_variants(TEST_DATA_ROOT_PATH "/testimages/256_color.bmp", "V1_256_color.bmp", 30, cppbmpfile::orientation_type::top_down);
    test_variants("V1_256_color.bmp", "V2_256_color.bmp", 0, cppbmpfile::orientation_type::bottom_up);
    test_variants("V2_256_color.bmp", "V3_256_color.bmp", 0, cppbmpfile::orientation_type::top_down);
    test_variants("V3_256_color.bmp", "V4_256_color.bmp", 50, cppbmpfile::orientation_type::bottom_up);
    test_compare_variants(TEST_DATA_ROOT_PATH "/testimages/256_color.bmp", "V4_256_color.bmp");
}