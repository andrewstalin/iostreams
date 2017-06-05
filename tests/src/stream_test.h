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

#ifndef _IOSTREAMS_STREAM_TEST_H_
#define _IOSTREAMS_STREAM_TEST_H_

#include "tests.h"
#include "utils.h"
#include "iostreams/stream.h"
#include "iostreams/transform/string_transform/base64.h"
#include "iostreams/transform/string_transform/hex.h"
#include "iostreams/error.h"

namespace iostreams
{
	namespace tests
	{
		template<typename byte_type>
		void ToStringTest(iostreams::IStream<byte_type>& stream)
		{
			stream.write(TEST_DATA.data(), TEST_DATA.size());
			
			ToHexTransform<uint8_t> to_hex_transform;
			ToBase64Transform<uint8_t> to_base64_transform;

			EXPECT_EQ(std::string("0102030405060708090a0b0c0d"), stream.to_string(to_hex_transform));
			EXPECT_EQ(std::string("AQIDBAUGBwgJCgsMDQ=="), stream.to_string(to_base64_transform));
		}

		template<typename byte_type>
		void SeekTest(iostreams::IStream<byte_type>& stream)
		{
			stream.seek(0);
			EXPECT_EQ(0u, stream.tell());

			stream.write(TEST_DATA.data(), TEST_DATA.size());

			EXPECT_EQ(13u, stream.tell());

			stream.seek(-2, std::ios_base::end);
			EXPECT_EQ(11u, stream.tell());

			stream.seek(4, std::ios_base::beg);
			EXPECT_EQ(4u, stream.tell());

			stream.seek(-1, std::ios_base::cur);
			EXPECT_EQ(3u, stream.tell());
		}

		template<typename byte_type>
		void SeekOutOffRangeTest(iostreams::IStream<byte_type>& stream)
		{
			EXPECT_THROW(stream.seek(50, std::ios_base::cur), IOStreamsException);
			EXPECT_THROW(stream.seek(-50, std::ios_base::cur), IOStreamsException);
			EXPECT_THROW(stream.seek(50, std::ios_base::end), IOStreamsException);
			EXPECT_THROW(stream.seek(-50, std::ios_base::end), IOStreamsException);
			EXPECT_THROW(stream.seek(50, std::ios_base::beg), IOStreamsException);
			EXPECT_THROW(stream.seek(-50, std::ios_base::beg), IOStreamsException);
		}

		template<typename byte_type>
		void ReadWriteTest(iostreams::IStream<byte_type>& stream)
		{
			size_t pos{ 0 };

			for (const auto& chunk : tests::SplitData<std::vector<uint8_t>>(TEST_DATA, 5))
			{
				if (LOG_ENABLED)
				{
					tests::write_to_log(chunk);
				}

				auto written_bytes = stream.write(chunk.data(), chunk.size());
				pos += chunk.size();

				EXPECT_EQ(chunk.size(), written_bytes);
				EXPECT_EQ(pos, stream.tell());
			}

			EXPECT_EQ(TEST_DATA.size(), stream.size());
			stream.seek(0);

			std::vector<uint8_t> actual(TEST_DATA.size());
			auto read_bytes = stream.read(actual.data(), TEST_DATA.size());
			EXPECT_EQ(TEST_DATA.size(), read_bytes);
			EXPECT_EQ(TEST_DATA, actual);
		}

		template<typename byte_type>
		void ReadTest(iostreams::IStream<byte_type>& stream)
		{
			stream.write(TEST_DATA.data(), TEST_DATA.size());
			std::vector<uint8_t> buffer(TEST_DATA.size());

			stream.seek(0);
			auto read_bytes = stream.read(buffer.data(), 3);
			EXPECT_EQ(std::vector<uint8_t>({ 1, 2, 3 }), std::vector<uint8_t>(buffer.data(), buffer.data() + read_bytes));
			EXPECT_EQ(3u, stream.tell());

			stream.seek(-2, std::ios_base::end);
			read_bytes = stream.read(buffer.data(), 10);
			EXPECT_EQ(std::vector<uint8_t>({ 12, 13 }), std::vector<uint8_t>(buffer.data(), buffer.data() + read_bytes));
			EXPECT_EQ(TEST_DATA.size(), stream.tell());
		}

		template<typename byte_type>
		void ResizeTest(iostreams::IStream<byte_type>& stream)
		{
			EXPECT_EQ(0u, stream.size());
			EXPECT_EQ(0u, stream.tell());

			stream.resize(10u);
			EXPECT_EQ(10u, stream.size());
			EXPECT_EQ(0u, stream.tell());

			stream.seek(7u);
			stream.resize(15u);
			EXPECT_EQ(15u, stream.size());
			EXPECT_EQ(7u, stream.tell());

			stream.resize(5u);
			EXPECT_EQ(5u, stream.size());
			EXPECT_EQ(5u, stream.tell());
		}

		template<typename byte_type>
		void ReadAllToVectorTest(iostreams::IStream<byte_type>& stream)
		{
			stream.write(TEST_DATA.data(), TEST_DATA.size());
			stream.seek(0);

			auto actual = stream. template read_all<std::vector<byte_type>>();
			EXPECT_EQ(TEST_DATA.size(), actual.size());
			EXPECT_EQ(TEST_DATA, actual);
		}

		template<typename byte_type>
		void ReadAllToStringTest(iostreams::IStream<byte_type>& stream)
		{
			stream.write(TEST_DATA.data(), TEST_DATA.size());
			stream.seek(0);

			auto actual = stream. template read_all<std::basic_string<byte_type>>();
			EXPECT_EQ(TEST_DATA.size(), actual.size());
			EXPECT_EQ(TEST_DATA, std::vector<uint8_t>(actual.begin(), actual.end()));
		}
	}
}

#endif