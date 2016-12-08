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

#ifdef USE_ZLIB

#include "tests.h"
#include "utils.h"
#include "iostreams/transform/compression/gzip_transform.h"
#include "iostreams/transform/compression/ungzip_transform.h"

using namespace iostreams;

TEST(gzip_transform_case, transform_test)
{
	auto gzip = GZipTransform<uint8_t>::create();

	std::vector<uint8_t> buffer;
	buffer.reserve(TEST_DATA.size() * 2);
	
	gzip.update(TEST_DATA.data(), TEST_DATA.size(), [&buffer](const uint8_t* data, GZipTransform<uint8_t>::size_type size)
	{
		buffer.insert(buffer.end(), data, data + size);
	});
	gzip.update_final([&buffer](const uint8_t* data, GZipTransform<uint8_t>::size_type size)
	{
		buffer.insert(buffer.end(), data, data + size);
	});

	std::vector<uint8_t> actual;
	actual.reserve(TEST_DATA.size());

	auto ungzip = UnGZipTransform<uint8_t>::create();
	ungzip.update(buffer.data(), buffer.size(), [&actual](const uint8_t* data, UnGZipTransform<uint8_t>::size_type size)
	{
		actual.insert(actual.end(), data, data + size);
	});
	ungzip.update_final([&actual](const uint8_t* data, UnGZipTransform<uint8_t>::size_type size)
	{
		actual.insert(actual.end(), data, data + size);
	});

	EXPECT_EQ(TEST_DATA.size(), actual.size());
	EXPECT_EQ(TEST_DATA, actual);
}
#endif