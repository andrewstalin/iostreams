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

#ifndef _IOSTREAMS_TESTS_TRANSFORM_H_
#define _IOSTREAMS_TESTS_TRANSFORM_H_

#include "tests.h"
#include "utils.h"
#include "iostreams/transform/transform.h"

namespace iostreams
{
	namespace tests
	{
		template<typename transformer_type, typename data_type = typename transformer_type::value_type>
		void FromStringTransformTest(const std::string& data, const std::vector<data_type>& expected)
		{
			transformer_type transformer;

			std::vector<data_type> actual;
			actual.reserve(transformer.required_size(data.size()));

			auto blocks = SplitData<std::string>(data, 10);

			for (auto i = 0u; i < blocks.size(); ++i)
			{
				const auto& block = blocks[i];

				if (LOG_ENABLED)
				{
					std::cout << "value: " << std::string(block.begin(), block.end()) << ", size: " << block.size() << std::endl;
				}

				transformer.update(block.data(), block.size(), [&actual](const data_type* data, size_t size)
				{
					actual.insert(actual.end(), data, data + size);
				});
			}

			transformer.update_final([&actual](const data_type* data, size_t size)
			{
				actual.insert(actual.end(), data, data + size);
			});

			EXPECT_EQ(expected.size(), actual.size());
			EXPECT_EQ(expected, actual);
		}

		template<typename transformer_type, typename data_type = typename transformer_type::value_type>
		void ToStringTransformTest(const std::vector<data_type>& data, const std::string& expected)
		{
			transformer_type transformer;

			std::string actual;
			actual.reserve(transformer.required_size(data.size()));

			auto blocks = SplitData<std::vector<data_type>>(data, 10);

			for (auto i = 0u; i < blocks.size(); ++i)
			{
				const auto& block = blocks[i];

				if (LOG_ENABLED)
				{
					write_to_log<data_type>(block);
				}

				transformer.update(block.data(), block.size(), [&actual](const char* data, size_t size)
				{
					actual.insert(actual.end(), data, data + size);
				});
			}

			transformer.update_final([&actual](const char* data, size_t size)
			{
				actual.insert(actual.end(), data, data + size);
			});

			EXPECT_EQ(expected.size(), actual.size());
			EXPECT_EQ(expected, actual);
		}
	}
}

#endif