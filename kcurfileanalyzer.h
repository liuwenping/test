// -----------------------------------------------------------------------
//
// 文 件 名 ：kcurfileanalyzer.h
// 创 建 者 ：v8
// 创建时间 ：2011-1
// 功能描述 ：
//
// -----------------------------------------------------------------------
#ifndef __KXCURANALYZER_H__
#define __KXCURANALYZER_H__
#include <QString>
#include <QDataStream>
#include <QImage>
#include <QFile>
#include <QBitArray>
#include <QPixmap>

// 这个类实现的相当不合理，有空应该重构掉。
// 这个类现在完全假定了图片的格式为双色两层位图格式，并且没有任何错误判断
//		-- by jinchizhong
class KCurFileAnalyzer
{
public:
	KCurFileAnalyzer();
	~KCurFileAnalyzer();

	QCursor loadFile(QString& fileName);

protected:
	QVector<uchar> _ReadBitmap(int width, 
		int height, 
		QDataStream &in) const;
	bool _LoadFileHeader(QDataStream &stream); 
protected:
	struct CUR_MARK
	{
		quint16 reserved;
		quint16 type;
		quint16 count;
	}m_curMark;

	struct CUR_ENTERY
	{
		quint8 width;
		quint8 height;
		quint8 colorCount;
		quint8 reserved;
		qint16 xHotSpot;
		qint16 yHotSpot;
		quint32 sizeInBytes;
		quint32 fileOffset;
	}m_curEntry;

	struct CUR_INFO_HEADER
	{
		quint32 size;
		quint32 width;
		quint32 height;
		quint16 plans;
		quint16 bitCount;
		quint32 compression;
		quint32 imageSise;
		quint32 xPixelsPerM;	// unused
		quint32 yPixelsPerM;	// unused
		quint32 colorUsed;		// unused
		quint32 colorImportant;	// unused
		quint64 colors;
	}m_curInfoHeader;
protected:
	enum
	{
		RESERVED = 0,
		TYPE = 2,
		SIZE_OF_INFOHEADER = 40,
	};
};
#endif // __KXCURANALYZER_H__
