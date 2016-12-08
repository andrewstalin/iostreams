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
#include "iostreams/transform/string_transform/base64.h"
#include "iostreams/stream.h"
#include "iostreams/memory.h"

using namespace iostreams;

static const std::vector<uint8_t> BASE64_TEST_DATA = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
static const std::string BASE64 = "CgsMDQ4PEBESExQVFhcYGRobHB0eHyAhIiMkJSYnKCkqKywtLi8wMTIzNDU2Nzg5Ojs8PT4/QEFCQ0RFRkdISUpLTE1OT1BRUlNUVVZXWFlaW1xdXl9gYWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXp7fH1+f4CBgoOEhYaHiImKi4yNjo+QkZKTlJWWl5iZmpucnZ6foKGio6SlpqeoqaqrrK2ur7CxsrO0tba3uLm6u7y9vr/AwcLDxMXGx8jJysvMzc7P0NHS09TV1tfY2drb3N3e3+Dh4uPk5ebn6Onq6+zt7u/w8fLz9PX29/j5+vv8/f7/AAECAwQFBgcICQ==";
static const std::string BASE64_WITH_LINES = "CgsMDQ4PEBESExQVFhcYGRobHB0eHyAhIiMkJSYnKCkqKywtLi8wMTIzNDU2Nzg5Ojs8PT4/QEFC\r\nQ0RFRkdISUpLTE1OT1BRUlNUVVZXWFlaW1xdXl9gYWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXp7\r\nfH1+f4CBgoOEhYaHiImKi4yNjo+QkZKTlJWWl5iZmpucnZ6foKGio6SlpqeoqaqrrK2ur7CxsrO0\r\ntba3uLm6u7y9vr/AwcLDxMXGx8jJysvMzc7P0NHS09TV1tfY2drb3N3e3+Dh4uPk5ebn6Onq6+zt\r\n7u/w8fLz9PX29/j5+vv8/f7/AAECAwQFBgcICQ==";

static constexpr uint8_t REPEAT_COUNT{ 1 };

template<typename byte_type>
void FromBase64Test(const std::string& data)
{
	auto i = REPEAT_COUNT;
	std::vector<byte_type> expected;
	expected.assign(BASE64_TEST_DATA.begin(), BASE64_TEST_DATA.end());

	while (i-- > 0)
	{
		tests::FromStringTransformTest<FromBase64Transform<byte_type>>(data, expected);
	}
}

template<typename byte_type>
void ToBase64Test()
{
	auto i = REPEAT_COUNT;
	std::vector<byte_type> data;
	data.assign(BASE64_TEST_DATA.begin(), BASE64_TEST_DATA.end());

	while (i-- > 0)
	{
		tests::ToStringTransformTest<ToBase64Transform<byte_type>>(data, BASE64);
	}
}

TEST(base64_case, to_base64_from_char_array_test)
{
	ToBase64Test<char>();
}

TEST(base64_case, to_base64_from_uint8_array_test)
{
	ToBase64Test<uint8_t>();
}

TEST(base64_case, from_base64_to_uint8_array_test)
{
	FromBase64Test<uint8_t>(BASE64);
}

TEST(base64_case, from_base64_to_char_array_test)
{
	FromBase64Test<char>(BASE64);
}

TEST(base64_case, from_base64_with_split_lines_to_char_array_test)
{
	FromBase64Test<char>(BASE64_WITH_LINES);
}

TEST(base64_case, from_base64_with_split_lines_to_uint8_array_test)
{
	FromBase64Test<uint8_t>(BASE64_WITH_LINES);
}

TEST(base64_case, from_base64_transform_test)
{
	MemoryStream<char> source_stream(200);
	source_stream.write(BASE64.data(), BASE64.size());
	source_stream.seek(0);

	MemoryStream<char> destination_stream(200);
	FromBase64Transform<char> from_base64_transform;
	transform<char>(&source_stream, &destination_stream, from_base64_transform);

	destination_stream.seek(0);

	auto result = destination_stream.read_all<std::vector<char>>();
	std::vector<uint8_t> actual;
	actual.assign(result.begin(), result.end());

	EXPECT_EQ(BASE64_TEST_DATA.size(), actual.size());
	EXPECT_EQ(BASE64_TEST_DATA, actual);
}

TEST(base64_case, bug_length_test)
{
	std::vector<std::pair<std::string, std::string>> expected = { {std::string(""), std::string("")}, { std::string("a"), std::string("YQ==") }, { std::string("ab"), std::string("YWI=") }, { std::string("abc"), std::string("YWJj") }, { std::string("abcd"), std::string("YWJjZA==") } };

	for (auto i = 0u; i < expected.size(); ++i)
	{
		auto expected_text = expected.at(i).first;
		auto expected_base64 = expected.at(i).second;

		MemoryStream<char> text_stream;
		MemoryStream<char> base64_stream;

		text_stream.write(expected_text.data(), expected_text.size());
		text_stream.seek(0);

		ToBase64Transform<char> to_base64_transform;
		transform<char>(&text_stream, &base64_stream, to_base64_transform);

		auto actual_base64 = base64_stream.read_all<std::string>();
		EXPECT_EQ(expected_base64, actual_base64);

		text_stream.resize(0);
		base64_stream.seek(0);

		FromBase64Transform<char> from_base64_transform;
		transform<char>(&base64_stream, &text_stream, from_base64_transform);

		auto actual_text = text_stream.read_all<std::string>();
		EXPECT_EQ(expected_text, actual_text);
	}
}