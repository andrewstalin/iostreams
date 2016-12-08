// Licensed under the MIT License <http://opensource.org/licenses/MIT>
// Author: Andrew Stalin <andrew.stalin@gmail.com>
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "tests.h"
#include "utils.h"
#include "stream_test.h"
#include "iostreams/memory.h"
#include "liberror/exception.h"

using namespace iostreams;

TEST(memory_stream_case, hex_create_test)
{
	FromHexTransform<uint8_t> transformer;
	auto stream = MemoryStream<uint8_t>::create("0102030405060708090a0b0c0d", transformer);
	std::vector<uint8_t> actual(static_cast<size_t>(stream.size()));
	stream.read(actual.data(), actual.size());
	EXPECT_EQ(std::vector<uint8_t>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }), actual);
}

TEST(memory_stream_case, hex_with_space_create_test)
{
	FromHexTransform<uint8_t> transformer;
	auto stream = MemoryStream<uint8_t>::create("01 02 03 04 05 06 07 08 09 0a 0b 0c 0d", transformer);
	std::vector<uint8_t> actual(static_cast<size_t>(stream.size()));
	stream.read(actual.data(), actual.size());
	EXPECT_EQ(std::vector<uint8_t>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }), actual);
}

TEST(memory_stream_case, base64_create_test)
{
	FromBase64Transform<uint8_t> transformer;
	auto stream = MemoryStream<uint8_t>::create("AQIDBAUGBwgJCgsMDQ==", transformer);
	std::vector<uint8_t> actual(static_cast<size_t>(stream.size()));
	stream.read(actual.data(), actual.size());
	EXPECT_EQ(std::vector<uint8_t>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }), actual);
}

TEST(memory_stream_case, base64_with_lines_create_test)
{
	FromBase64Transform<uint8_t> transformer;
	auto stream = MemoryStream<uint8_t>::create("CgsMDQ4PEBESExQVFhcYGRobHB0eHyAhIiMkJSYnKCkqKywtLi8wMTIzNDU2Nzg5Ojs8PT4/QEFC\r\nQ0RFRkdISUpLTE1OT1BRUlNUVVZXWFlaW1xdXl9gYWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXp7\r\nfH1+f4CBgoOEhYaHiImKi4yNjo+QkZKTlJWWl5iZmpucnZ6foKGio6SlpqeoqaqrrK2ur7CxsrO0\r\ntba3uLm6u7y9vr/AwcLDxMXGx8jJysvMzc7P0NHS09TV1tfY2drb3N3e3+Dh4uPk5ebn6Onq6+zt\r\n7u/w8fLz9PX29/j5+vv8/f7/AAECAwQFBgcICQ==", transformer);
	std::vector<uint8_t> actual(static_cast<size_t>(stream.size()));
	stream.read(actual.data(), actual.size());
	EXPECT_EQ(std::vector<uint8_t>({ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }), actual);
}

TEST(memory_stream_case, to_string_test)
{
	MemoryStream<uint8_t> stream(3);
	tests::ToStringTest(stream);
}

TEST(memory_stream_case, seek_test)
{
	MemoryStream<uint8_t> stream(3);
	tests::SeekTest(stream);
}

TEST(memory_stream_case, seek_out_of_range_test)
{
	MemoryStream<uint8_t> stream(1);
	tests::SeekOutOffRangeTest(stream);
}

TEST(memory_stream_case, capacity_test)
{
	MemoryStream<uint8_t> stream(3);
	
	stream.reserve(10);
	EXPECT_EQ(0u, stream.size());
	EXPECT_EQ(12u, stream.capacity());

	stream.reserve(9);
	EXPECT_EQ(0u, stream.size());
	EXPECT_EQ(12u, stream.capacity());

	stream.reserve(13);
	EXPECT_EQ(0u, stream.size());
	EXPECT_EQ(15u, stream.capacity());
}

TEST(memory_stream_case, read_write_test)
{
	MemoryStream<uint8_t> stream(3);
	tests::ReadWriteTest(stream);
}

TEST(memory_stream_case, read_test)
{
	MemoryStream<uint8_t> stream(3);
	tests::ReadTest(stream);
}

TEST(memory_stream_case, resize_test)
{
	MemoryStream<uint8_t> stream(3);
	tests::ResizeTest(stream);
}

TEST(memory_stream_case, read_all_to_string_test)
{
	MemoryStream<uint8_t> stream(3);
	tests::ReadAllToStringTest(stream);
}

TEST(memory_stream_case, read_all_to_vector_test)
{
	MemoryStream<uint8_t> stream(3);
	tests::ReadAllToVectorTest(stream);
}