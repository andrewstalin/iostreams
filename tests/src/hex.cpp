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
#include "transform_test.h"
#include "iostreams/transform/string_transform/hex.h"
#include <algorithm>

using namespace iostreams;

static const std::vector<uint8_t> HEX_TEST_DATA{ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
static const std::string HEX{ "0a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9fa0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff00010203040506070809" };

static constexpr uint8_t REPEAT_COUNT{ 1 };

template<typename byte_type>
void FromHexTest(const std::string& data)
{
	auto i = REPEAT_COUNT;
	std::vector<byte_type> expected;
	expected.assign(HEX_TEST_DATA.begin(), HEX_TEST_DATA.end());

	while (i-- > 0)
	{
		tests::FromStringTransformTest<FromHexTransform<byte_type>>(data, expected);
	}
}

template<typename byte_type>
void ToHexTest()
{
	auto i = REPEAT_COUNT;
	std::vector<byte_type> data;
	data.assign(HEX_TEST_DATA.begin(), HEX_TEST_DATA.end());

	while (i-- > 0)
	{
		tests::ToStringTransformTest<ToHexTransform<byte_type>>(data, HEX);
	}
}

TEST(hex_case, to_hex_from_char_array_test)
{
	ToHexTest<char>();
}

TEST(hex_case, to_hex_from_uint8_array_test)
{
	ToHexTest<uint8_t>();
}

TEST(hex_case, from_lowercase_hex_to_char_array_test)
{
	FromHexTest<char>(HEX);
}

TEST(hex_case, from_lowercase_hex_to_uint8_array_test)
{
	FromHexTest<uint8_t>(HEX);
}

TEST(hex_case, from_uppercase_hex_to_uint8_array_test)
{
	std::string uppercase_hex;
	uppercase_hex.resize(HEX.size());
	std::transform(HEX.begin(), HEX.end(), uppercase_hex.begin(), ::toupper);
	FromHexTest<uint8_t>(uppercase_hex);
}

TEST(hex_case, from_uppercase_hex_to_char_array_test)
{
	std::string uppercase_hex;
	uppercase_hex.resize(HEX.size());
	std::transform(HEX.begin(), HEX.end(), uppercase_hex.begin(), ::toupper);
	FromHexTest<char>(uppercase_hex);
}

TEST(hex_case, from_split_space_hex_test)
{
	std::vector<char> buffer;
	buffer.reserve(HEX.size() * 2);
	size_t i{ 0 };

	while(i < HEX.size() - 2)
	{
		buffer.push_back(HEX[i++]);
		buffer.push_back(HEX[i++]);
		buffer.push_back(' ');
	}

	buffer.push_back(HEX[i++]);
	buffer.push_back(HEX[i++]);

	FromHexTest<char>(std::string(buffer.begin(), buffer.end()));
}