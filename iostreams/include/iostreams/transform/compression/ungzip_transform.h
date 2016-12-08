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

#ifndef _IOSTREAMS_UNGZIP_TRANSFORM_H_
#define _IOSTREAMS_UNGZIP_TRANSFORM_H_

#ifdef USE_ZLIB
#include "iostreams/transform/transform.h"
#include "iostreams/transform/compression/compression_level.h"
#include "zlib.h"

namespace iostreams
{
	template<typename byte_type>
	class UnGZipTransform : public ITransform<byte_type, byte_type>
	{
	private:
		static constexpr size_t BUFFER_SIZE = 4 * 1024;

		z_stream inflate_stream_;
		byte_type buffer_[BUFFER_SIZE];
		bool is_close_{ false };

	public:
		using base_type = ITransform<byte_type, byte_type>;
		using size_type = typename base_type::size_type;
		using transform_handler = typename base_type::TransformHandler;

		UnGZipTransform(UnGZipTransform&& val)
			: inflate_stream_(val.inflate_stream_)
			, is_close_(val.is_close_)
		{
			std::copy(val.buffer_, val.buffer_ + UnGZipTransform::BUFFER_SIZE, buffer_);
			val.inflate_stream_ = { 0 };
			val.is_close_ = true;
		}

		UnGZipTransform(const UnGZipTransform&) = delete;
		UnGZipTransform& operator=(const UnGZipTransform&) = delete;

		~UnGZipTransform();

		static UnGZipTransform create();

		void update(const byte_type* data, size_type size, const transform_handler& handler) override;
		void update_final(const transform_handler& handler) override;

	private:
		UnGZipTransform(z_stream inflate_stream)
			: inflate_stream_(inflate_stream)
		{}

		void close();
	};
}
#endif
#endif