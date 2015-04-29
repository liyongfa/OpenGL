#include "Bitmap.h"
#include <assert.h>
#include <memory.h>
#include <stdio.h>

namespace e
{

#ifndef BI_RGB
#	define BI_RGB 0
#endif

	typedef struct _RGBQUAD
	{
		uint8 red;
		uint8 green;
		uint8 blue;
		uint8 reserved;
	}RGBQUAD;

	inline bool read(void * buffer, size_t size, FILE * fp)
	{
		return ::fread(buffer, 1, size, fp) == size;
	}

	inline bool write(void * buffer, size_t size, FILE * fp)
	{
		return ::fwrite(buffer, 1, size, fp) == size;
	}

	inline bool skip(size_t size, FILE * fp)
	{
		return ::fseek(fp, size, SEEK_SET) == 0;
	}

	template<class T> inline void _Swap(T & l, T & r)
	{
		T temp = l; l = r; r = temp;
	}

	//////////////////////////////////////////////////////////////////////////
	bool _LoadBitmap(Bitmap & bitmap, const char* fileName, bool turn)
	{
		bitmap.Clear();

		if (fileName == 0) return false;
		FILE * fp = NULL;
		fopen_s(&fp, fileName, "rb");
		if (fp == 0) return false;

		bool result = false;

		do
		{
			//read bitmap file header
			if (!read(&bitmap.bfType, sizeof(bitmap.bfType), fp)) break;
			if (bitmap.bfType != 0x4D42) break;

			if (!read(&bitmap.bfSize, sizeof(bitmap.bfSize), fp)) break;
			if (!read(&bitmap.bfReserved1, sizeof(bitmap.bfReserved1), fp)) break;
			if (!read(&bitmap.bfReserved2, sizeof(bitmap.bfReserved2), fp)) break;
			if (!read(&bitmap.bfOffBits, sizeof(bitmap.bfOffBits), fp)) break;

			//read bitmap info header
			if (!read(&bitmap.biSize, sizeof(bitmap.biSize), fp)) break;
			if (!read(&bitmap.biWidth, sizeof(bitmap.biWidth), fp)) break;
			if (!read(&bitmap.biHeight, sizeof(bitmap.biHeight), fp)) break;
			if (!read(&bitmap.biPlanes, sizeof(bitmap.biPlanes), fp)) break;
			if (!read(&bitmap.biBitCount, sizeof(bitmap.biBitCount), fp)) break;
			if (!read(&bitmap.biCompression, sizeof(bitmap.biCompression), fp))	break;
			if (!read(&bitmap.biSizeImage, sizeof(bitmap.biSizeImage), fp)) break;
			if (!read(&bitmap.biXPelsPerMeter, sizeof(bitmap.biXPelsPerMeter), fp)) break;
			if (!read(&bitmap.biYPelsPerMeter, sizeof(bitmap.biYPelsPerMeter), fp))	break;
			if (!read(&bitmap.biClrUsed, sizeof(bitmap.biClrUsed), fp)) break;
			if (!read(&bitmap.biClrImportant, sizeof(bitmap.biClrImportant), fp)) break;

			if (bitmap.biBitCount != 8 && bitmap.biBitCount != 24 && bitmap.biBitCount != 32)
			{
				break;
			}

			int lineBytes = WIDTHBYTES(bitmap.biWidth * bitmap.biBitCount);
			//assert(bitmap.biHeader.biSizeImage == lineBytes * bitmap.biHeader.biHeight);

			bitmap.size = lineBytes * bitmap.biHeight;
			bitmap.bits = new uint8[bitmap.size];
			if (bitmap.bits == 0) break;

			if (!skip(bitmap.bfOffBits, fp))break;

			if (!read(bitmap.bits, bitmap.size, fp))
			{
				delete bitmap.bits;
				bitmap.bits = 0;
				bitmap.size = 0;
				return false;
			}

			//需要将文件中的数据上下倒转过来
			if (turn)
			{
				bitmap.Mirror(true);
			}

			result = true;
		} while (0);

		fclose(fp);

		return result;
	}

	bool _SaveBitmap(const char* fileName, uint8 * bits, int width, int height, int bitCount, bool turn)
	{
		if (fileName == 0) return false;

		FILE * fp = NULL;
		fopen_s(&fp, fileName, "wb");

		if (fp == 0) return false;

		bool result = false;

		do
		{
			int lineBytes = WIDTHBYTES(width * bitCount);
			int paletteSize = (1 << bitCount) * sizeof(RGBQUAD);
			int imageSize = lineBytes * height;

			uint16  bfType = 0x4D42;
			uint32  bfSize = 0;
			uint16  bfReserved1 = 0;
			uint16  bfReserved2 = 0;
			uint32  bfOffBits = 0;

			if (bitCount == 8)
			{
				bfOffBits = 54 + paletteSize;
				bfSize = 54 + paletteSize + imageSize;
			}
			else if (bitCount == 24 || bitCount == 32)
			{
				bfOffBits = 54;
				bfSize = 54 + imageSize;
			}

			uint32 biSize = 40;
			uint32 biWidth = width;
			uint32 biHeight = height;
			uint16 biPlanes = 1;
			uint16 biBitCount = bitCount;
			uint32 biCompression = BI_RGB;
			uint32 biSizeImage = imageSize;
			uint32 biXPelsPerMeter = 3780;
			uint32 biYPelsPerMeter = 3780;
			uint32 biClrUsed = 0;
			uint32 biClrImportant = 0;

			if (!write(&bfType, sizeof(bfType), fp)) break;
			if (!write(&bfSize, sizeof(bfSize), fp)) break;
			if (!write(&bfReserved1, sizeof(bfReserved1), fp)) break;
			if (!write(&bfReserved2, sizeof(bfReserved2), fp)) break;
			if (!write(&bfOffBits, sizeof(bfOffBits), fp)) break;

			if (!write(&biSize, sizeof(biSize), fp)) break;
			if (!write(&biWidth, sizeof(biWidth), fp)) break;
			if (!write(&biHeight, sizeof(biHeight), fp)) break;
			if (!write(&biPlanes, sizeof(biPlanes), fp)) break;
			if (!write(&biBitCount, sizeof(biBitCount), fp)) break;
			if (!write(&biCompression, sizeof(biCompression), fp)) break;
			if (!write(&biSizeImage, sizeof(biSizeImage), fp)) break;
			if (!write(&biXPelsPerMeter, sizeof(biXPelsPerMeter), fp)) break;
			if (!write(&biYPelsPerMeter, sizeof(biYPelsPerMeter), fp)) break;
			if (!write(&biClrUsed, sizeof(biClrUsed), fp)) break;
			if (!write(&biClrImportant, sizeof(biClrImportant), fp)) break;

			if (bitCount == 8)
			{
				RGBQUAD * rgbQuad = new RGBQUAD[1 << bitCount];
				assert(rgbQuad != 0);

				for (int i = 0; i < (1 << bitCount); i++)
				{
					rgbQuad[i].red = i;
					rgbQuad[i].green = i;
					rgbQuad[i].blue = i;
				}

				if (!write(rgbQuad, sizeof(RGBQUAD) * (1 << bitCount), fp))
				{
					delete[] rgbQuad;
					break;
				}

				delete rgbQuad;
			}

			if (!turn)
			{
				if (!write(bits, imageSize, fp)) break;
			}
			else//位图倒转存储
			{
				for (uint y = 0; y < biHeight; y++)
				{
					uint8* p = bits + (biHeight - y - 1) * lineBytes;

					if (!write(p, lineBytes, fp)) goto _error;
				}
			}

			result = true;

		} while (0);

	_error:
		fclose(fp);

		return result;
	}

	bool _SaveBitmap(const char* fileName, const Bitmap & bitmap, bool turn)
	{
		return _SaveBitmap(fileName, bitmap.bits, bitmap.biWidth, bitmap.biHeight, bitmap.biBitCount, turn);
	}

	/////////////////////////////////////////////////////////////////////////
	Bitmap::Bitmap(void)
	{
		Initialize();
	}

	Bitmap::Bitmap(const Bitmap & r)
	{
		//BITMAP FILE HEADER
		this->bfType = r.bfType;
		this->bfSize = r.bfSize;
		this->bfReserved1 = r.bfReserved1;
		this->bfReserved2 = r.bfReserved2;
		this->bfOffBits = r.bfOffBits;
		//BITMAP INFO HEADER
		this->biSize = r.biSize;
		this->biWidth = r.biWidth;
		this->biHeight = r.biHeight;
		this->biPlanes = r.biPlanes;
		this->biBitCount = r.biBitCount;
		this->biCompression = r.biCompression;
		this->biSizeImage = r.biSizeImage;
		this->biXPelsPerMeter = r.biXPelsPerMeter;
		this->biYPelsPerMeter = r.biYPelsPerMeter;
		this->biClrUsed = r.biClrUsed;
		this->biClrImportant = r.biClrImportant;
		//BITMAP DATA
		this->size = r.size;
		this->bits = new uint8[size];
		assert(this->bits != 0);
		memcpy(this->bits, r.bits, this->size);
	}

	Bitmap::Bitmap(const char* fileName)
	{
		bool ret = Load(fileName);
		assert(ret);
	}

	Bitmap::Bitmap(int width, int height, int bitCount, const uint8* bits)
	{
		Initialize();

		Alloc(width, height, bitCount, bits);
	}

	Bitmap::Bitmap(int width, int height, int bitCount, int initValue)
	{
		Initialize();

		Alloc(width, height, bitCount, initValue);
	}

	Bitmap::~Bitmap()
	{
		Cleanup();
	}

	uint8 & Bitmap::operator[](const uint index)
	{
		assert(index >= 0 && index < size);
		return bits[index];
	}

	const Bitmap & Bitmap::operator=(const Bitmap & r)
	{
		if (this != &r)
		{
			//BITMAP FILE HEADER
			this->bfType = r.bfType;
			this->bfSize = r.bfSize;
			this->bfReserved1 = r.bfReserved1;
			this->bfReserved2 = r.bfReserved2;
			this->bfOffBits = r.bfOffBits;
			//BITMAP INFO HEADER
			this->biSize = r.biSize;
			this->biWidth = r.biWidth;
			this->biHeight = r.biHeight;
			this->biPlanes = r.biPlanes;
			this->biBitCount = r.biBitCount;
			this->biCompression = r.biCompression;
			this->biSizeImage = r.biSizeImage;
			this->biXPelsPerMeter = r.biXPelsPerMeter;
			this->biYPelsPerMeter = r.biYPelsPerMeter;
			this->biClrUsed = r.biClrUsed;
			this->biClrImportant = r.biClrImportant;
			//BITMAP DATA
			if (this->bits != 0 && this->size != r.size)
			{
				delete[] this->bits;
				this->bits = 0;
			}

			this->size = r.size;
			this->bits = new uint8[size];
			assert(bits != 0);
			memcpy(bits, r.bits, size);
		}

		return *this;
	}

	void Bitmap::Initialize()
	{
		bits = 0;
		size = 0;

		bfType = 0;
		bfSize = 0;
		bfReserved1 = 0;
		bfReserved2 = 0;
		bfOffBits = 0;

		biSize = 0;
		biWidth = 0;
		biHeight = 0;
		biPlanes = 0;
		biBitCount = 0;
		biCompression = 0;
		biSizeImage = 0;
		biXPelsPerMeter = 0;
		biYPelsPerMeter = 0;
		biClrUsed = 0;
		biClrImportant = 0;
	}

	bool Bitmap::Alloc(int width, int height, int bitCount, const uint8* bits)
	{
		int lineBytes = WIDTHBYTES(width * bitCount);
		int imageSize = lineBytes * height;
		int paletteSize = (bitCount << 1) * sizeof(RGBQUAD);

		this->bfType = 0x4D42;

		if (bitCount == 8)
		{
			this->bfOffBits = 54 + paletteSize;
			this->bfSize = 54 + paletteSize + imageSize;
		}
		else if (bitCount == 24 || bitCount == 32)
		{
			this->bfOffBits = 54;
			this->bfSize = 54 + imageSize;
		}

		bool flag = (biWidth != width || biHeight != height || biBitCount != bitCount);

		this->biSize = 40;
		this->biWidth = width;
		this->biHeight = height;
		this->biBitCount = bitCount;
		this->biSizeImage = imageSize;
		this->biPlanes = 1;
		this->biCompression = BI_RGB;
		this->biClrUsed = 0;
		this->biClrImportant = 0;
		this->biXPelsPerMeter = 3780;
		this->biYPelsPerMeter = 3780;

		if (flag)//当宽与高发生改变时候才重新申请内存
		{
			uint8 * p = new uint8[imageSize];
			if (p == 0) return false;

			if (bits != 0)
			{
				memcpy(p, bits, imageSize * sizeof(uint8));
			}
			else
			{
				memset(p, 0, imageSize * sizeof(uint8));
			}

			if (this->bits != 0)
			{
				delete[] this->bits;
			}

			this->bits = p;
		}
		else
		{
			if (bits != 0)
			{
				memcpy(this->bits, bits, imageSize * sizeof(uint8));
			}
			else
			{
				memset(this->bits, 0, imageSize * sizeof(uint8));
			}
		}

		size = imageSize;

		return true;
	}

	bool Bitmap::Alloc(int width, int height, int bitCount, int initValue /* = 0x00 */)
	{
		int lineBytes = WIDTHBYTES(width * bitCount);
		int imageSize = lineBytes * height;
		int paletteSize = (bitCount << 1) * sizeof(RGBQUAD);

		this->bfType = 0x4D42;

		if (bitCount == 8)
		{
			this->bfOffBits = 54 + paletteSize;
			this->bfSize = 54 + paletteSize + imageSize;
		}
		else if (bitCount == 24 || bitCount == 32)
		{
			this->bfOffBits = 54;
			this->bfSize = 54 + imageSize;
		}

		bool flag = (biWidth != width || biHeight != height || biBitCount != bitCount);

		this->biSize = 40;
		this->biWidth = width;
		this->biHeight = height;
		this->biBitCount = bitCount;
		this->biSizeImage = imageSize;
		this->biPlanes = 1;
		this->biCompression = BI_RGB;
		this->biClrUsed = 0;
		this->biClrImportant = 0;
		this->biXPelsPerMeter = 3780;
		this->biYPelsPerMeter = 3780;

		if (flag)//当宽与高发生改变时候才重新申请内存
		{
			uint8 * p = new uint8[imageSize];
			if (p == 0) return false;

			memset(p, initValue, imageSize * sizeof(uint8));

			if (bits != 0)
			{
				delete[] bits;
				bits = 0;
			}

			bits = p;
		}
		else
		{
			memset(bits, initValue, imageSize  * sizeof(uint8));
		}

		size = imageSize;

		return true;
	}

	void Bitmap::SwapChannel(int channel0, int channel1)
	{
		assert(biBitCount == 24 || biBitCount == 32);
		assert(channel0 >= 0 && channel0 <= 3);
		assert(channel1 >= 0 && channel1 <= 3);

		int lineBytes = WIDTHBYTES(biBitCount * biWidth);

		for (uint32 y = 0; y < biHeight; y++)
		{
			uint8 * p = bits + y * lineBytes;

			for (uint32 x = 0; x < biWidth; x++)
			{
				uint8 temp = *(p + channel0);
				*(p + channel0) = *(p + channel1);
				*(p + channel1) = temp;

				p += biBitCount / 8;
			}
		}
	}

	void Bitmap::Clear(void)
	{
		Cleanup();
	}

	void Bitmap::SetColor8(uint8 color /* = 0xff */)
	{
		assert(bits != 0);
		memset(bits, color, size);
	}

	void Bitmap::SetColor24(uint32 color /* = 0x00000000 */)
	{
		assert(biBitCount == 24);

		uint8 r = (color & 0x000000ff);
		uint8 g = (color & 0x0000ff00) >> 8;
		uint8 b = (color & 0x00ff0000) >> 16;

		SetColor24(r, g, b);
	}

	void Bitmap::SetColor24(uint8 r, uint8 g, uint8 b)
	{
		assert(biBitCount == 24);
		assert(bits != 0);

		int lineBytes = WIDTHBYTES(biBitCount * biWidth);

		for (uint y = 0; y < biHeight; y++)
		{
			uint8* p = bits + y * lineBytes;

			for (uint x = 0; x < biWidth; x++)
			{
				*(p + 0) = b;	//B
				*(p + 1) = g;	//G
				*(p + 2) = r;	//R

				p += 3;
			}
		}
	}

	void Bitmap::SetColor32(uint32 color /* = 0x00000000 */)
	{
		assert(biBitCount == 32);

		uint8 r = (color & 0x000000ff);
		uint8 g = (color & 0x0000ff00) >> 8;
		uint8 b = (color & 0x00ff0000) >> 16;
		uint8 a = (color & 0xff000000) >> 24;

		SetColor32(r, g, b, a);
	}

	void Bitmap::SetColor32(uint8 r, uint8 g, uint8 b, uint8 a)
	{
		assert(biBitCount == 32);
		assert(bits != 0);

		int lineBytes = WIDTHBYTES(biBitCount * biWidth);

		for (uint y = 0; y < biHeight; y++)
		{
			uint8 *p = bits + y * lineBytes;

			for (uint x = 0; x < biWidth; x++)
			{
				*(p + 0) = b;
				*(p + 1) = g;
				*(p + 2) = r;
				*(p + 3) = a;

				p += 4;
			}
		}
	}

	bool Bitmap::ExtendAlpha(uint8 a)
	{
		if (this->biBitCount != 24 && this->biBitCount != 32)
		{
			return false;
		}

		if (this->biBitCount == 24)
		{
			Bitmap temp(biWidth, biHeight, 32);

			int lineBytes0 = this->GetRowBytes();
			int lineBytes1 = temp.GetRowBytes();

			for (uint y = 0; y < biHeight; y++)
			{
				uint8* p0 = this->bits + y * lineBytes0;
				uint8* p1 = temp.bits + y * lineBytes1;

				for (uint x = 0; x < biWidth; x++)
				{
					*(p1 + 0) = *(p0 + 0);
					*(p1 + 1) = *(p0 + 1);
					*(p1 + 2) = *(p0 + 2);
					*(p1 + 3) = a;

					p0 += 3;
					p1 += 4;
				}
			}

			Swap(temp);
		}
		else if (this->biBitCount == 32)
		{
			int lineBytes = this->GetRowBytes();

			for (uint y = 0; y < biHeight; y++)
			{
				uint8* p = this->bits + y * lineBytes;

				for (uint x = 0; x < biWidth; x++)
				{
					*(p + 3) = a;

					p += 4;
				}
			}
		}

		return true;
	}

	int Bitmap::GetPixelBytes(void) const
	{
		return biBitCount / 8;
	}

	int Bitmap::GetRowBytes(void) const
	{
		return WIDTHBYTES(biBitCount * biWidth);
	}

	int Bitmap::Width(void) const
	{
		return (int)biWidth;
	}

	int Bitmap::Height(void) const
	{
		return (int)biHeight;
	}

	uint8* Bitmap::Get(int x, int y)
	{
		assert(bits != 0);
		assert(x >= 0 && x < (int)biWidth);
		assert(y >= 0 && y < (int)biHeight);

		return bits + y * WIDTHBYTES(biBitCount * biWidth) + x * (biBitCount / 8);
	}

	void Bitmap::Mirror8(bool vertical)
	{
		int lineBytes = GetRowBytes();

		//垂直翻转
		if (vertical)
		{
			for (uint y = 0; y < biHeight/2; y++)
			{
				uint8* p0 = bits + y * lineBytes;
				uint8* p1 = bits + (biHeight - y - 1) * lineBytes;

				for (uint x = 0; x < biWidth; x++)
				{
					_Swap(p0[x], p1[x]);
				}
			}
		}
		else
		{
			for (uint y = 0; y < biHeight; y++)
			{
				uint8* l = bits + y * lineBytes;
				uint8* r = bits + y * lineBytes + lineBytes - 1;

				for (uint x = 0; x < biWidth / 2; x++)
				{
					_Swap(*l, *r);

					l++;
					r--;
				}
			}
		}
	}

	void Bitmap::Mirror24(bool vertical)
	{
		assert(biBitCount == 24);

		int lineBytes = GetRowBytes();

		if (vertical)
		{
			for (uint y = 0; y < biHeight/2; y++)
			{
				uint8* p0 = bits + y * lineBytes;
				uint8* p1 = bits + (biHeight - y - 1) * lineBytes;

				for (uint x = 0; x < biWidth; x++)
				{
					_Swap(*(p0 + 0), *(p1 + 0));
					_Swap(*(p0 + 1), *(p1 + 1));
					_Swap(*(p0 + 2), *(p1 + 2));

					p0 += 3;
					p1 += 3;
				}
			}
		}
		else
		{
			for (uint y = 0; y < biHeight; y++)
			{
				uint8* l = bits + y * lineBytes;
				uint8* r = bits + y * lineBytes + (biWidth - 1) * 3;

				for (uint x = 0; x < biWidth / 2; x++)
				{
					_Swap(*(l + 0), *(r + 0));
					_Swap(*(l + 1), *(r + 1));
					_Swap(*(l + 2), *(r + 2));

					l += 3;
					r -= 3;
				}
			}
		}
	}

	void Bitmap::Mirror32(bool vertical)
	{
		assert(biBitCount == 32);

		int lineBytes = GetRowBytes();

		if (vertical)
		{
			for (uint y = 0; y < biHeight/2; y++)
			{
				uint32* p0 = (uint32*)(bits + y * lineBytes);
				uint32* p1 = (uint32*)(bits + (biHeight - y - 1) * lineBytes);

				for (uint x = 0; x < biWidth; x++)
				{
					_Swap(p0[x], p1[x]);
				}
			}
		}
		else
		{
			for (uint y = 0; y < biHeight; y++)
			{
				uint32* l = (uint32*)(bits + y * lineBytes);
				uint32* r = (uint32*)(bits + (y + 1) * lineBytes - 4);

				for (uint x = 0; x < biWidth / 2; x++)
				{
					_Swap(*l, *r);

					l++;
					r--;
				}
			}
		}
	}

	void Bitmap::Mirror(bool vertical /* = true */)
	{
		assert(bits != 0);

		switch (biBitCount)
		{
		case 8:
			Mirror8(vertical);
			break;
		case 24:
			Mirror24(vertical);
			break;
		case 32:
			Mirror32(vertical);
			break;
		default:
			assert(0);
			break;
		}
	}

	void Bitmap::Swap(Bitmap & r)
	{
		if (this != &r)
		{
			_Swap(bfType, r.bfType);
			_Swap(bfSize, r.bfSize);
			_Swap(bfReserved1, r.bfReserved1);
			_Swap(bfReserved2, r.bfReserved2);
			_Swap(bfOffBits, r.bfOffBits);
			_Swap(biSize, r.biSize);
			_Swap(biWidth, r.biWidth);
			_Swap(biHeight, r.biHeight);
			_Swap(biPlanes, r.biPlanes);
			_Swap(biBitCount, r.biBitCount);
			_Swap(biCompression, r.biCompression);
			_Swap(biSizeImage, r.biSizeImage);
			_Swap(biXPelsPerMeter, r.biXPelsPerMeter);
			_Swap(biYPelsPerMeter, r.biYPelsPerMeter);
			_Swap(biClrUsed, r.biClrUsed);
			_Swap(biClrImportant, r.biClrImportant);

			_Swap(size, r.size);
			_Swap(bits, r.bits);
		}
	}

	void Bitmap::Cleanup(void)
	{
		if (bits != 0)
		{
			delete[] bits;
		}

		Initialize();
	}

	bool Bitmap::Load(const char* fileName, bool turn /* = true */)
	{
		return _LoadBitmap(*this, fileName, turn);
	}

	bool Bitmap::Save(const char* fileName, bool turn /* = false */) const
	{
		return _SaveBitmap(fileName, *this, turn);
	}

	//////////////////////////////////////////////////////////////////////////
	bool SaveBitmap(HBITMAP hBitmap, const char* fileName)
	{
		assert(hBitmap != NULL);
		assert(fileName != NULL);

		BITMAP bm = { 0 };
		if (::GetObject(hBitmap, sizeof(BITMAP), &bm) == 0)
		{
			return false;
		}

		int bitCount = bm.bmPlanes * bm.bmBitsPixel;

		if (bitCount == 1)
		{
			bitCount = 1;
		}
		else if (bitCount <= 4)
		{
			bitCount = 4;
		}
		else if (bitCount <= 8)
		{
			bitCount = 8;
		}
		else if (bitCount <= 16)
		{
			bitCount = 16;
		}
		else if (bitCount <= 24)
		{
			bitCount = 24;
		}
		else
		{
			bitCount = 32;
		}

		return _SaveBitmap(fileName, (uint8*)bm.bmBits, bm.bmWidth, bm.bmHeight, bitCount, true);
	}

	bool Save2PPM(const Bitmap & bitmap, const char* fileName)
	{
		FILE * fp = 0;
		fopen_s(&fp, fileName, "wb");
		if (fp == NULL) return false;

		fprintf(fp, "P6\n");
		fprintf(fp, "%d %d\n", bitmap.biWidth, bitmap.biHeight);
		fprintf(fp, "%d\n", 255);

		int lineBytes = bitmap.GetRowBytes();

		for (uint y = 0; y < bitmap.biHeight; y++)
		{
			uint8* p = bitmap.bits + y * lineBytes;

			fwrite(p, 1, bitmap.biWidth * 3, fp);
		}

		fclose(fp);

		return true;
	}
}


