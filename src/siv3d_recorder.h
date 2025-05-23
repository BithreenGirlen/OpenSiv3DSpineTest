#ifndef SIV3d_RECORDER_H_
#define SIV3d_RECORDER_H_

#ifndef NO_S3D_USING
#define NO_S3D_USING
#endif
#include <Siv3D.hpp>

class CSiv3dRecorder
{
public:
	CSiv3dRecorder();
	~CSiv3dRecorder();

	enum class EOutputType
	{
		Gif,
		Video,
	};
	/// <summary>
	/// 録画開始。
	/// </summary>
	/// <param name="frameSize">フレーム寸法</param>
	/// <param name="outputType">出力形式</param>
	/// <param name="fps"></param>
	/// <returns></returns>
	bool Start(const s3d::Size &frameSize, EOutputType outputType, s3d::int32 fps = DefaultFps);

	bool IsUnderRecording() const;
	EOutputType GetOutputType() const;

	/// <summary>
	/// フレーム保存。設定FPSより早く呼び出された場合何もしない。
	/// </summary>
	/// <param name="frame">GPU資源</param>
	/// <returns>録画データとして蓄えた場合true</returns>
	bool CommitFrame(const s3d::RenderTexture& frame);

	/// <summary>
	/// 書き出し。
	/// </summary>
	/// <param name="filePath">出力先。拡張子はなくても可。</param>
	/// <returns>1フレームでも書き込めたらtrue</returns>
	bool End(s3d::FilePath& filePath);
private:
	static constexpr s3d::int32 DefaultFps = 30;

	s3d::int32 m_fps = DefaultFps;
	s3d::Optional<double> m_lastTime;

	s3d::Array<s3d::RenderTexture> m_frames;
	s3d::Size m_frameSize{};
	EOutputType m_outputType = EOutputType::Video;

	void Clear();
};
#endif // !SIV3d_RECORDER_H_
