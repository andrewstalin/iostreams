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

#ifndef _IOSTREAMS_TESTS_UTILS_H_
#define _IOSTREAMS_TESTS_UTILS_H_

#include "tests.h"

namespace iostreams
{
	namespace tests
	{
		size_t GenerateRandomNumber(size_t min_val, size_t max_val);

		template<typename byte_type>
		void write_to_log(const std::vector<byte_type>& bytes);

		template<typename T, typename data_type = typename T::value_type>
		std::vector<std::vector<data_type>> SplitData(const T& data, size_t max_chunk_size);
	}
}

#endif