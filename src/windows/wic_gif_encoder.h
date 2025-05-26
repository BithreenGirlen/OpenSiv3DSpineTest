#ifndef WIC_GIF_ENCODER_H_
#define WIC_GIF_ENCODER_H_

class CWicGifEncoder
{
public:
	CWicGifEncoder();
	~CWicGifEncoder();

	bool Initialise(const wchar_t *filePath);
	bool HasBeenInitialised() const;

	/// <summary>
	/// 画素配列はRGBA32を想定。
	/// </summary>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="stride"></param>
	/// <param name="pixels">画素配列</param>
	/// <param name="hasAlpha">透過有無</param>
	/// <param name="delay">前フレームとの間隔(秒単位)</param>
	/// <returns></returns>
	bool CommitFrame(unsigned int width, unsigned int height, unsigned int stride, unsigned char* pixels, bool hasAlpha, float delay);

	bool Finalise();
private:
	class Impl;
	Impl* m_impl = nullptr;
};

#endif // !WIC_GIF_ENCODER_H_
