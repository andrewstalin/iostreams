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
#include "iostreams/array.h"

using namespace iostreams;

TEST(array_stream_case, base64_create_test)
{
	FromBase64Transform<uint8_t> transformer;
	auto stream = ArrayStream<uint8_t>::create("AQIDBAUGBwgJCgsMDQ==", transformer);
	std::vector<uint8_t> actual(static_cast<size_t>(stream.size()));
	stream.read(actual.data(), actual.size());
	EXPECT_EQ(std::vector<uint8_t>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }), actual);
}

TEST(array_stream_case, to_string_test)
{
	ArrayStream<uint8_t> stream;
	tests::ToStringTest(stream);
}

TEST(array_stream_case, seek_test)
{
	ArrayStream<uint8_t> stream;
	tests::SeekTest(stream);
}

TEST(array_stream_case, seek_out_of_range_test)
{
	ArrayStream<uint8_t> stream;
	tests::SeekOutOffRangeTest(stream);
}

TEST(array_stream_case, read_write_test)
{
	ArrayStream<uint8_t> stream;
	tests::ReadWriteTest(stream);
}

TEST(array_stream_case, read_test)
{
	ArrayStream<uint8_t> stream;
	tests::ReadTest(stream);
}

TEST(array_stream_case, resize_test)
{
	ArrayStream<uint8_t> stream;
	tests::ResizeTest(stream);
}

TEST(array_stream_case, read_all_to_string_test)
{
	ArrayStream<uint8_t> stream;
	tests::ReadAllToStringTest(stream);
}

TEST(array_stream_case, read_all_to_vector_test)
{
	ArrayStream<uint8_t> stream;
	tests::ReadAllToVectorTest(stream);
}