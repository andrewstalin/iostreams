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
#include "iostreams/file.h"

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace iostreams;

FileStream<uint8_t> CreateTempFile()
{
#ifdef _WIN32
	char buffer[L_tmpnam_s];
	tmpnam_s(buffer, L_tmpnam_s);
	return FileStream<uint8_t>::open(buffer, FileAccess::READ_WRITE, FileMode::F_CREATE_NEW, FileShare::SHARE_DELETE, FILE_FLAG_DELETE_ON_CLOSE);
#else
	return FileStream<uint8_t>::create(::tmpfile64());
#endif
}

TEST(file_stream_case, to_string_test)
{
	auto stream = CreateTempFile();
	tests::ToStringTest(stream);
}

TEST(file_stream_case, seek_test)
{
	auto stream = CreateTempFile();
	tests::SeekTest(stream);
}

TEST(file_stream_case, seek_out_of_range_test)
{
	auto stream = CreateTempFile();
	tests::SeekOutOffRangeTest(stream);
}

TEST(file_stream_case, read_write_test)
{
	auto stream = CreateTempFile();
	tests::ReadWriteTest(stream);
}

TEST(file_stream_case, read_test)
{
	auto stream = CreateTempFile();
	tests::ReadTest(stream);
}

TEST(file_stream_case, resize_test)
{
	auto stream = CreateTempFile();
	tests::ResizeTest(stream);
}

TEST(file_stream_case, read_all_to_string_test)
{
	auto stream = CreateTempFile();
	tests::ReadAllToStringTest(stream);
}

TEST(file_stream_case, read_all_to_vector_test)
{
	auto stream = CreateTempFile();
	tests::ReadAllToVectorTest(stream);
}