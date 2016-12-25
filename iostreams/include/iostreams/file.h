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

#ifndef _IOSTREAMS_FILE_H_
#define _IOSTREAMS_FILE_H_

#include "iostreams/stream.h"
#include <string>
#include <memory>
#include <sys/stat.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace iostreams
{
	enum class FileAccess : uint8_t
	{
		NONE = 0,
		READ,
		WRITE,
		READ_WRITE
	};

	enum class FileMode : uint8_t
	{
		NONE = 0,
		F_CREATE_NEW,
		F_CREATE_ALWAYS,
		F_OPEN_EXISTING,
		F_OPEN_ALWAYS,
		F_TRUNCATE_EXISTING
	};

	enum class FileShare
	{
#ifdef _WIN32
		NONE = 0,
		SHARE_DELETE = FILE_SHARE_DELETE,
		SHARE_READ = FILE_SHARE_READ,
		SHARE_WRITE = FILE_SHARE_WRITE,
		SHARE_READ_WRITE = FILE_SHARE_WRITE | FILE_SHARE_READ
#elif defined (__linux__) || defined (__APPLE__)
        NONE = 0,
		IRWXU = S_IRWXU,
		IRUSR = S_IRUSR,
		IWUSR = S_IWUSR,
		IXUSR = S_IXUSR,
		IRWXG = S_IRWXG,
		IRGRP = S_IRGRP,
		IWGRP = S_IWGRP,
		IXGRP = S_IXGRP,
		IRWXO = S_IRWXO,
		IROTH = S_IROTH,
		IWOTH = S_IWOTH,
		IXOTH = S_IXOTH
#endif
	};

    template<typename byte_type>
    class FileStream : public IStream<byte_type>
    {
    private:
		std::string path_;

		class FileImpl;
        std::unique_ptr<FileImpl> pimpl_;

		FileAccess file_access_;
		FileMode file_mode_;
		FileShare file_share_;

    public:
        using size_type = typename IStream<byte_type>::size_type;
		using count_type = typename IStream<byte_type>::count_type;
        using off_type = typename IStream<byte_type>::off_type;

		FileStream(FileStream&& stream);
		FileStream& operator=(FileStream&& stream);

		FileStream(const FileStream&) = delete;
		FileStream& operator=(const FileStream&) = delete;

        ~FileStream();

		static FileStream create(FILE* file);
		static FileStream open(const char* path, FileAccess file_access, FileMode file_mode, FileShare file_share, uint64_t flags = 0);

		const std::string& path() const { return path_; }

		FileAccess file_access() const { return file_access_; }
		FileMode file_mode() const { return file_mode_; }
		FileShare file_share() const { return file_share_; }

		bool auto_flush() const;
		void set_auto_flush(bool val);

		void close();

        size_type size() const override;
		size_type tell() const override;
		std::string to_string(IToStringTransform<byte_type>& transformer) const override;
        void seek(off_type off, std::ios_base::seekdir way = std::ios_base::beg) override;
        void resize(size_type size) override;
		count_type read(byte_type* buffer, count_type count) override;
		count_type write(const byte_type* data, count_type size) override;

	protected:
		FileStream(std::unique_ptr<FileImpl> pimpl, const char* path, FileAccess file_access, FileMode file_mode, FileShare file_share);
		FileStream(std::unique_ptr<FileImpl> pimpl);

		static void check_seek_range(off_type off, std::ios_base::seekdir way, size_type current_position, size_type size);
    };
}
#endif