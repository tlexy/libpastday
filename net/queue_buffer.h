
#ifndef QUEUE_BUFFER_H
#define QUEUE_BUFFER_H

//#include <boost/noncopyable.hpp>
#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>
//#include <unistd.h>  // ssize_t

	///
	/// @code
	/// +-------------------+------------------+------------------+
	/// | prependable bytes |  readable bytes  |  writable bytes  |
	/// |                   |     (CONTENT)    |                  |
	/// +-------------------+------------------+------------------+
	/// |                   |                  |                  |
	/// 0      <=      readerIndex   <=   writerIndex    <=     size
	/// @endcode
class QueueBuffer// : private boost::noncopyable
{
public:
	static const size_t kCheapPrepend = 8;
	static const size_t kInitialSize = 1024;

	explicit QueueBuffer(size_t initialSize = kInitialSize)
		: buffer_(kCheapPrepend + initialSize),
		readerIndex_(kCheapPrepend),
		writerIndex_(kCheapPrepend)
	{
		assert(readableBytes() == 0);
		assert(writableBytes() == initialSize);
		assert(prependableBytes() == kCheapPrepend);
		searchIndex_ = readerIndex_;
	}

	// implicit copy-ctor, move-ctor, dtor and assignment are fine
	// NOTE: implicit move-ctor is added in g++ 4.6

	void swap(QueueBuffer& rhs)
	{
		buffer_.swap(rhs.buffer_);
		std::swap(readerIndex_, rhs.readerIndex_);
		std::swap(writerIndex_, rhs.writerIndex_);
	}

	size_t readableBytes() const
	{
		return writerIndex_ - readerIndex_;
	}

	size_t writableBytes() const
	{
		return buffer_.size() - writerIndex_;
	}

	size_t usableBytes() const
	{
		return buffer_.size() - readableBytes();
	}

	size_t prependableBytes() const
	{
		return readerIndex_;
	}

	const char* peek() const
	{
		return begin() + readerIndex_;
	}

	size_t findrn()
	{
		/*for (int i = buffer_.size() - 1; i >= 3; --i)
		{
			if (buffer_[i] == '\n' && buffer_[i - 1] == '\r'
				&& buffer_[i - 2] == '\n' && buffer_[i - 3] == '\r')
			{
				return i - readerIndex_ + 1;
			}
		}*/
		int i = 0;
		for (i = searchIndex_; i <= writerIndex_ - 4; ++i)
		{
			if (buffer_[i] == '\r' && buffer_[i + 1] == '\n'
				&& buffer_[i + 2] == '\r' && buffer_[i + 3] == '\n')
			{
				return i + 3 - readerIndex_ + 1;
			}
		}
		searchIndex_ = i;
		return 0;
	}

	void resetrn()
	{
		searchIndex_ = readerIndex_;
	}

	void retrieve(size_t len)
	{
		assert(len <= readableBytes());
		if (len < readableBytes())
		{
			readerIndex_ += len;
		}
		else
		{
			retrieveAll();
		}
	}

	void retrieveUntil(const char* end)
	{
		assert(peek() <= end);
		assert(end <= beginWrite());
		retrieve(end - peek());
	}

	void retrieveInt64()
	{
		retrieve(sizeof(int64_t));
	}

	void retrieveInt32()
	{
		retrieve(sizeof(int32_t));
	}

	void retrieveInt16()
	{
		retrieve(sizeof(int16_t));
	}

	void retrieveInt8()
	{
		retrieve(sizeof(int8_t));
	}

	void erase(const std::size_t len)
	{
		retrieve(len);
	}

	void retrieveAll()
	{
		readerIndex_ = kCheapPrepend;
		writerIndex_ = kCheapPrepend;
	}

	std::string retrieveAllAsString()
	{
		return retrieveAsString(readableBytes());;
	}

	std::string retrieveAsString(size_t len)
	{
		assert(len <= readableBytes());
		std::string result(peek(), len);
		retrieve(len);
		return result;
	}

	void retrieve(size_t len, std::string& out_str)
	{
		assert(len <= readableBytes());
		//std::string result(peek(), len);
		for (char* ch = (char*)peek(); ch < peek() + len; ++ch)
		{
			out_str.push_back(*ch);
		}
		//out_str = result;
		retrieve(len);
	}

	void append(const char* /*restrict*/ data, size_t len)
	{
		ensureWritableBytes(len);
		std::copy(data, data + len, beginWrite());
		hasWritten(len);
	}

	void append(const void* /*restrict*/ data, size_t len)
	{
		append(static_cast<const char*>(data), len);
	}

	void push_back(const char *data, const std::size_t len)
	{
		append(data, len);
	}

	void ensureWritableBytes(size_t len)
	{
		if (writableBytes() < len)
		{
			makeSpace(len);
		}
		assert(writableBytes() >= len);
	}

	char* beginWrite()
	{
		return begin() + writerIndex_;
	}

	const char* beginWrite() const
	{
		return begin() + writerIndex_;
	}

	void hasWritten(size_t len)
	{
		assert(len <= writableBytes());
		writerIndex_ += len;
	}

	void unwrite(size_t len)
	{
		assert(len <= readableBytes());
		writerIndex_ -= len;
	}

	///
	/// Append int64_t using network endian
	///
	void appendInt64(int64_t x)
	{
		append(&x, sizeof x);
	}

	///
	/// Append int32_t using network endian
	///
	void appendInt32(int32_t x)
	{
		append(&x, sizeof x);
	}

	void appendInt16(int16_t x)
	{
		append(&x, sizeof x);
	}

	void appendInt8(int8_t x)
	{
		append(&x, sizeof x);
	}

	size_t read(char* buf, size_t len)
	{
		len = readableBytes() > len ? len : readableBytes();
		::memcpy(buf, peek(), len);
		retrieve(len);
		return len;
	}

	///
	/// Read int64_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int64_t readInt64()
	{
		int64_t result = peekInt64();
		retrieveInt64();
		return result;
	}

	///
	/// Read int32_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int32_t readInt32()
	{
		int32_t result = peekInt32();
		retrieveInt32();
		return result;
	}

	int16_t readInt16()
	{
		int16_t result = peekInt16();
		retrieveInt16();
		return result;
	}

	int8_t readInt8()
	{
		int8_t result = peekInt8();
		retrieveInt8();
		return result;
	}

	///
	/// Peek int64_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int64_t)
	int64_t peekInt64() const
	{
		assert(readableBytes() >= sizeof(int64_t));
		int64_t be64 = 0;
		::memcpy(&be64, peek(), sizeof be64);
		return be64;
	}

	///
	/// Peek int32_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int32_t peekInt32() const
	{
		assert(readableBytes() >= sizeof(int32_t));
		int32_t be32 = 0;
		::memcpy(&be32, peek(), sizeof be32);
		return be32;
	}

	int16_t peekInt16() const
	{
		assert(readableBytes() >= sizeof(int16_t));
		int16_t be16 = 0;
		::memcpy(&be16, peek(), sizeof be16);
		return be16;
	}

	int8_t peekInt8() const
	{
		assert(readableBytes() >= sizeof(int8_t));
		int8_t x = *peek();
		return x;
	}

	///
	/// Prepend int64_t using network endian
	///
	void prependInt64(int64_t x)
	{
		prepend(&x, sizeof x);
	}

	///
	/// Prepend int32_t using network endian
	///
	void prependInt32(int32_t x)
	{
		prepend(&x, sizeof x);
	}

	void prependInt16(int16_t x)
	{
		prepend(&x, sizeof x);
	}

	void prependInt8(int8_t x)
	{
		prepend(&x, sizeof x);
	}

	void prepend(const void* /*restrict*/ data, size_t len)
	{
		assert(len <= prependableBytes());
		readerIndex_ -= len;
		const char* d = static_cast<const char*>(data);
		std::copy(d, d + len, begin() + readerIndex_);
	}

	void shrink(size_t reserve)
	{
		// FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
		//QueueBuffer other;
		//other.ensureWritableBytes(readableBytes()+reserve);
		//other.append(toStringPiece());
		//swap(other);
		buffer_.shrink_to_fit();
	}

	size_t internalCapacity() const
	{
		return buffer_.capacity();
	}

private:

	char* begin()
	{
		return &*buffer_.begin();
	}

	const char* begin() const
	{
		return &*buffer_.begin();
	}

	void makeSpace(size_t len)
	{
		if (writableBytes() + prependableBytes() < len + kCheapPrepend)
		{
			// FIXME: move readable data
			buffer_.resize(writerIndex_ + len);
		}
		else
		{
			// move readable data to the front, make space inside buffer
			assert(kCheapPrepend < readerIndex_);
			size_t readable = readableBytes();
			std::copy(begin() + readerIndex_,
				begin() + writerIndex_,
				begin() + kCheapPrepend);
			readerIndex_ = kCheapPrepend;
			writerIndex_ = readerIndex_ + readable;
			assert(readable == readableBytes());
		}
	}

private:
	std::vector<char> buffer_;
	size_t readerIndex_;
	size_t writerIndex_;
	size_t searchIndex_;

	static const char kCRLF[];
};


#endif  // FLUYII_NET_BUFFER_H
