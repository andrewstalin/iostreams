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

#include "utils.h"
#include <random>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace iostreams
{
	namespace tests
	{
		size_t GenerateRandomNumber(size_t min_val, size_t max_val)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(min_val, max_val);
			return static_cast<size_t>(dis(gen));
		}

		template<typename byte_type>
		void write_to_log(const std::vector<byte_type>& bytes)
		{
			std::cout << "size: " << bytes.size();

			if (!bytes.empty())
			{
				std::cout << ", data: [";

				for (auto i = 0u; i < bytes.size() - 1; ++i)
				{
					std::cout << std::to_string(bytes[i]) << ", ";
				}

				std::cout << std::to_string(bytes.back()) << "]";
			}

			std::cout << std::endl;
		}

		template<typename T, typename data_type>
		std::vector<std::vector<data_type>> SplitData(const T& data, size_t max_chunk_size)
		{
			std::vector<std::vector<data_type>> result;
			size_t index{ 0 };

			while (index < data.size())
			{
				auto chunk_length = GenerateRandomNumber(1, max_chunk_size);

				if (index + chunk_length >= data.size())
				{
					chunk_length = data.size() - index;
				}

				auto chunk = std::vector<data_type>(data.data() + index, data.data() + index + chunk_length);
				result.push_back(chunk);
				index += chunk_length;
			}

			return result;
		}
	}
}

template void iostreams::tests::write_to_log<char>(const std::vector<char>& bytes);
template void iostreams::tests::write_to_log<uint8_t>(const std::vector<uint8_t>& bytes);

template std::vector<std::vector<uint8_t>> iostreams::tests::SplitData(const std::vector<uint8_t>&, size_t);
template std::vector<std::vector<char>> iostreams::tests::SplitData(const std::vector<char>&, size_t);
template std::vector<std::vector<char>> iostreams::tests::SplitData(const std::string&, size_t);