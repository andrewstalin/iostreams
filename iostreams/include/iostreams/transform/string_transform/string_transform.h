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

#ifndef _IOSTREAMS_STRING_TRANSFORM_H_
#define _IOSTREAMS_STRING_TRANSFORM_H_

#include "iostreams/transform/transform.h"
#include <vector>

namespace iostreams
{
	template<typename source_type, typename destination_type>
	class StringTransform : public ITransform<source_type, destination_type>
	{
	public:
		using base_type = ITransform<source_type, destination_type>;
		using transform_handler = typename base_type::TransformHandler;
		using size_type = typename base_type::size_type;

	protected:
		static constexpr size_type BUFFER_SIZE{ 4 * 1024 };

		size_type buffer_index_{ 0 };
		std::vector<destination_type> buffer_;

	public:
		virtual size_type required_size(size_type size) const = 0;

	protected:
		StringTransform()
			: StringTransform(StringTransform::BUFFER_SIZE)
		{}

		StringTransform(size_type buffer_size)
			: buffer_(buffer_size)
		{}

		StringTransform(const StringTransform&) = default;
		StringTransform& operator=(const StringTransform&) = default;

		void flush(const transform_handler& handler)
		{
			if (handler != nullptr)
			{
				handler(buffer_.data(), buffer_index_);
			}

			buffer_index_ = 0;
		}
	};

	template<typename byte_type>
	struct IFromStringTransform : public StringTransform<char, byte_type>
	{
		using base_type = StringTransform<char, byte_type>;
		using size_type = typename base_type::size_type;

		IFromStringTransform()
			: base_type()
		{}

		IFromStringTransform(size_type buffer_size)
			: base_type(buffer_size)
		{}

		IFromStringTransform(const IFromStringTransform&) = default;
		IFromStringTransform& operator=(const IFromStringTransform&) = default;
	};

	template<typename byte_type>
	struct IToStringTransform : public StringTransform<byte_type, char>
	{
		using base_type = StringTransform<byte_type, char>;
		using size_type = typename base_type::size_type;

		IToStringTransform()
			: base_type()
		{}

		IToStringTransform(size_type buffer_size)
			: base_type(buffer_size)
		{}

		IToStringTransform(const IToStringTransform&) = default;
		IToStringTransform& operator=(const IToStringTransform&) = default;
	};
}
#endif