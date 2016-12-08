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

#ifdef _WIN32
#include "iostreams/file.h"
#include "iostreams/error.h"
#include "babel/encoding.h"
#include <windows.h>
#include <io.h>

 namespace iostreams
 {
     template<typename byte_type>
	 class FileStream<byte_type>::FileImpl
	 {
	 private:
		 HANDLE handle_{ INVALID_HANDLE_VALUE };
		 bool auto_flush_{ false };

	 public:
		 using size_type = typename FileStream<byte_type>::size_type;
		 using count_type = typename FileStream<byte_type>::count_type;
		 using off_type = typename FileStream<byte_type>::off_type;

		 FileImpl(HANDLE handle)
			 : handle_(handle)
		 {}

		 FileImpl(FileImpl&& stream)
			 : handle_(stream.handle_)
			 , auto_flush_(stream.auto_flush_)
		 {
			 stream.handle_ = INVALID_HANDLE_VALUE;
		 }

		 FileImpl& operator=(FileImpl&& stream)
		 {
			 handle_ = stream.handle_;
			 auto_flush_ = stream.auto_flush_;
			 stream.handle_ = INVALID_HANDLE_VALUE;
			 return *this;
		 }

		 FileImpl(const FileImpl&) = delete;
		 FileImpl& operator=(const FileImpl&) = delete;

		 ~FileImpl()
		 {
			 close();
		 }

         static std::unique_ptr<FileImpl> create(FILE* file)
         {
             auto fd = _fileno(file);
             THROW_IF(fd == -1, IOStreamsException(errors::BAD_FILE));

             auto handle = _get_osfhandle(fd);
             THROW_IF(handle == -1, IOStreamsException(errors::BAD_FILE_DESCRIPTOR));

             return std::make_unique<FileImpl>(reinterpret_cast<HANDLE>(handle));
         }

		 static std::unique_ptr<FileImpl> open(const char* path, FileAccess file_access, FileMode file_mode, FileShare file_share, uint64_t flags)
		 {
			 DWORD desired_access{ 0 };
			 DWORD creation_disposition{ 0 };

			 switch (file_access)
			 {
			 case FileAccess::READ:
				 desired_access = FILE_GENERIC_READ;
				 break;
			 case FileAccess::WRITE:
				 desired_access = FILE_GENERIC_WRITE;
				 break;
			 case FileAccess::READ_WRITE:
				 desired_access = FILE_GENERIC_WRITE | FILE_GENERIC_READ;
				 break;
			 case FileAccess::NONE:
				 throw IOStreamsException(errors::BAD_FILE_ACCESS_VALUE);
			 }

			 switch (file_mode)
			 {
			 case FileMode::F_CREATE_ALWAYS:
				 creation_disposition = CREATE_ALWAYS;
				 break;
			 case FileMode::F_CREATE_NEW:
				 creation_disposition = CREATE_NEW;
				 break;
			 case FileMode::F_OPEN_EXISTING:
				 creation_disposition = OPEN_EXISTING;
				 break;
			 case FileMode::F_OPEN_ALWAYS:
				 creation_disposition = OPEN_ALWAYS;
				 break;
			 case FileMode::F_TRUNCATE_EXISTING:
				 creation_disposition = TRUNCATE_EXISTING;
				 break;
			 case FileMode::NONE:
				 throw IOStreamsException(errors::BAD_FILE_MODE_VALUE);
			 }

			 DWORD attributes = flags == 0 ? FILE_ATTRIBUTE_NORMAL : static_cast<DWORD>(flags);
			 auto wpath = babel::string_cast(path, strlen(path), "UTF-8");
			 auto handle = ::CreateFileW(wpath.c_str(), desired_access, static_cast<DWORD>(file_share), nullptr, creation_disposition, attributes, nullptr);
			 THROW_IF(handle == INVALID_HANDLE_VALUE, WIN32_ERROR("CreateFile"));
			 return std::make_unique<FileImpl>(handle);
		 }

		 bool auto_flush() const { return auto_flush_; }
		 void set_auto_flush(bool val) { auto_flush_ = val; }

		 void close()
		 {
			 if (handle_ != INVALID_HANDLE_VALUE)
			 {
				 ::CloseHandle(handle_);
				 handle_ = INVALID_HANDLE_VALUE;
			 }
		 }

		 size_type size() const
		 {
			 LARGE_INTEGER size = { 0 };
			 THROW_IF(!::GetFileSizeEx(handle_, &size), WIN32_ERROR("GetFileSizeEx"));
			 return static_cast<size_type>(size.QuadPart);
		 }

		 size_type tell() const
		 {
			 LARGE_INTEGER pos = { 0 };
			 THROW_IF(!::SetFilePointerEx(handle_, pos, &pos, static_cast<DWORD>(std::ios_base::cur)), WIN32_ERROR("SetFilePointerEx"));
			 return static_cast<size_type>(pos.QuadPart);
		 }

		 void seek(off_type off, std::ios_base::seekdir way = std::ios_base::beg)
		 {
			 FileStream<byte_type>::check_seek_range(off, way, tell(), size());

			 LARGE_INTEGER pos;
			 pos.QuadPart = off;
			 THROW_IF(!::SetFilePointerEx(handle_, pos, &pos, static_cast<DWORD>(way)), WIN32_ERROR("SetFilePointerEx"));
		 }

		 void resize(size_type new_size)
		 {
			 auto current_position = tell();

			 if (current_position != new_size)
			 {
				 LARGE_INTEGER pos;
				 pos.QuadPart = new_size;
				 THROW_IF(!::SetFilePointerEx(handle_, pos, &pos, FILE_BEGIN), WIN32_ERROR("SetFilePointerEx"));
			 }

			 THROW_IF(!::SetEndOfFile(handle_), WIN32_ERROR("SetEndOfFile"));

			 if (current_position != new_size)
			 {
				 DWORD way{ FILE_BEGIN };

				 LARGE_INTEGER pos;
				 pos.QuadPart = current_position;

				 if (current_position > new_size)
				 {
					 way = FILE_END;
					 pos.QuadPart = 0;
				 }

				 THROW_IF(!::SetFilePointerEx(handle_, pos, &pos, way), WIN32_ERROR("SetFilePointerEx"));
			 }
		 }

		 count_type read(byte_type* buffer, count_type count)
		 {
			 DWORD read_bytes{ 0 };
			 THROW_IF(!::ReadFile(handle_, buffer, count, &read_bytes, nullptr), WIN32_ERROR("ReadFile"));
			 return static_cast<count_type>(read_bytes);
		 }

		 count_type write(const byte_type* data, count_type size)
		 {
			 DWORD written_bytes{ 0 };
			 THROW_IF(!::WriteFile(handle_, data, size, &written_bytes, nullptr), WIN32_ERROR("WriteFile"));

			 if (auto_flush_)
			 {
				 THROW_IF(!::FlushFileBuffers(handle_), WIN32_ERROR("FlushFileBuffers"));
			 }

			 return static_cast<count_type>(written_bytes);
		 }
	 };
 }
 #endif