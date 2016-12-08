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

#include "iostreams/stream.h"
#include <vector>
#include <cassert>

template<typename byte_type>
void iostreams::transform(IStream<byte_type>* source_stream, IStream<byte_type>* destination_stream, ITransform<byte_type, byte_type>& transformer)
{
	using size_type = typename ITransform<byte_type, byte_type>::size_type;
	static constexpr size_type BUFFER_SIZE{ 500 * 1024 };

	assert(source_stream != nullptr && destination_stream != nullptr);

	THROW_IF(source_stream == destination_stream, IOStreamsException(errors::BAD_TRANSFORM_DESTINATION));

	auto source_size = source_stream->size();
	std::vector<byte_type> buffer(static_cast<size_type>(source_size > BUFFER_SIZE ? BUFFER_SIZE : source_size));
	auto i = source_size / BUFFER_SIZE + (source_size % BUFFER_SIZE > 0 ? 1 : 0);

	while (i-- > 0)
	{
		auto read_bytes = source_stream->read(buffer.data(), buffer.size());
		transformer.update(buffer.data(), read_bytes, [destination_stream](const byte_type* data, size_type size)
		{
			destination_stream->write(data, size);
		});
	}

	transformer.update_final([destination_stream](const byte_type* data, size_type size)
	{
		destination_stream->write(data, size);
	});
	destination_stream->seek(0);
}

template void iostreams::transform<char>(IStream<char>* source_stream, IStream<char>* destination_stream, ITransform<char, char>& transformer);
template void iostreams::transform<uint8_t>(IStream<uint8_t>* source_stream, IStream<uint8_t>* destination_stream, ITransform<uint8_t, uint8_t>& transformer);