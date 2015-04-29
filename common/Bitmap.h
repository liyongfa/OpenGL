#ifndef __CORE_BITMAP_H__
#define __CORE_BITMAP_H__

#include "Defines.h"
#include <windows.h>

typedef uint32 RGBA;

namespace e
{
	class Bitmap
	{
	public:
		Bitmap(void);
		Bitmap(const Bitmap & r);
		Bitmap(const char* fileName);
		Bitmap(int width, int height, int bitCount, const uint8* bits);
		Bitmap(int width, int height, int bitCount, int initValue = 0x00);
		virtual ~Bitmap(void);
		uint8 & operator[](const uint index);
		const Bitmap & operator=(const Bitmap & r);
		//默认分配是BGRA或者BGR格式
		bool Alloc(int width, int height, int bitCount, const uint8* bits);
		bool Alloc(int width, int height, int bitCount, int initValue = 0x00);
		void SwapChannel(int channel0, int channel1);
		void SetColor8(uint8 color = 0xff);
		void SetColor24(uint32 color = 0x00000000);//RGBA
		void SetColor24(uint8 r, uint8 g, uint8 b);
		void SetColor32(uint32 color = 0x00000000);//RGBA
		void SetColor32(uint8 r, uint8 g, uint8 b, uint8 a);
		bool ExtendAlpha(uint8 a);
		int  Width(void) const;
		int  Height(void) const;
		uint8* Get(int x, int y);
		int GetPixelBytes(void) const;
		int GetRowBytes(void) const;
		void Mirror(bool vertical = true);
		void Swap(Bitmap & r);
		void Clear(void);
		bool Load(const char* fileName, bool turn = false);
		bool Save(const char* fileName, bool turn = false) const;
	private:
		void Initialize(void);
		void Cleanup(void);
		//翻转
		void Mirror8(bool vertical);
		void Mirror24(bool vertical);
		void Mirror32(bool vertical);
	public:
		//pixel data
		uint8*  bits;
		uint32  size;
		//bitmap file header
		uint16  bfType;
		uint32  bfSize;
		uint16  bfReserved1;
		uint16  bfReserved2;
		uint32  bfOffBits;
		//bitmap info header
		uint32  biSize;
		uint32  biWidth;
		uint32  biHeight;
		uint16  biPlanes;
		uint16  biBitCount;
		uint32  biCompression;
		uint32  biSizeImage;
		uint32  biXPelsPerMeter;
		uint32  biYPelsPerMeter;
		uint32  biClrUsed;
		uint32  biClrImportant;
	};
	
	bool SaveBitmap(HBITMAP hBitmap, const char* fileName);

	bool SavePPM(const Bitmap & bitmap, const char* fileName);
}

#endif
