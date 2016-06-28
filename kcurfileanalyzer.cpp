// -----------------------------------------------------------------------
//
// 文 件 名 ：kcurfileanalyzer.cpp
// 创 建 者 ：v8
// 创建时间 ：2011-1
// 功能描述 ：
//
// -----------------------------------------------------------------------
//#include "stdafx.h"
#include <QtGui>
#include "kcurfileanalyzer.h"
#include <QBitArray>

inline bool _IsAllBitsZero(QVector<uchar>& bits)
{
	if (bits.size() == 0)
		return true;
	uchar result = bits[0];
	for (int i = 1; result == 0 && i < bits.size(); ++i)
		result |= bits[i];
	return !result;
}


KCurFileAnalyzer::KCurFileAnalyzer()
{
	m_curEntry.xHotSpot = -1;
	m_curEntry.yHotSpot = -1;
}

KCurFileAnalyzer::~KCurFileAnalyzer()
{
}

QCursor KCurFileAnalyzer::loadFile(QString& fileName)
{
	QFile curFile(fileName);
	if (!curFile.exists())
		return QCursor();
	if (!curFile.open(QIODevice::ReadOnly))
		return QCursor();
	QDataStream stream(&curFile);
	if (!_LoadFileHeader(stream))
		return QCursor();

	// 这里没有做文件格式判断，应当被重构整理掉。

	QVector<uchar> XL = _ReadBitmap(m_curEntry.width, m_curEntry.height, stream);
	QVector<uchar> AL = _ReadBitmap(m_curEntry.width, m_curEntry.height, stream);
	if (stream.status() != QDataStream::Ok) 
		return QCursor();

	QVector<uchar> BL, ML;
	BL.reserve(XL.size());
	ML.reserve(XL.size());
	for (int i = 0; i < AL.size(); ++i)
	{
		// Qt 分为两个图层 B M
		// Cur分为两个图层 X A
		// Qt当M为0时，表示混合		Cur当A为1时表示混合，因此要翻转A作为M
		ML.push_back(~AL[i]);
		// 为混合时，Qt和Cur文件的XB图层定义相同，但是如果为非混合时，Qt B=1为黑色 B=0为白色，Cur X=0为黑色，1为白色
		BL.push_back(XL[i] ^ ML[i]);
	}

	QBitmap bmp1 = QBitmap::fromData(QSize(m_curEntry.width, m_curEntry.height), &BL[0], QImage::Format_Mono);
	QBitmap bmp2;
#ifndef X_OS_WINDOWS
	// 针对非Windows平台不支持xor模式光标的处理，将其转化为不使用xor的方式
	if (_IsAllBitsZero(ML)) {
		QImage img1(&BL[0], m_curEntry.width, m_curEntry.height, QImage::Format_Mono);
		QImage img2((int)m_curEntry.width, (int)m_curEntry.height, QImage::Format_Mono);
		img2.fill(0);
		img2.setColor(1, QColor(Qt::black).rgba());
		img2.setColor(0, QColor(Qt::white).rgba());
		for (int y = 1; y < img1.height() - 1; ++y) {
			for (int x = 1; x < img1.width() - 1; ++x) {
				if (img1.pixelIndex(x, y)) {
					img2.setPixel(x-1, y-1, 1);
					img2.setPixel(x, y-1, 1);
					img2.setPixel(x+1, y-1, 1);
					img2.setPixel(x-1, y, 1);
					img2.setPixel(x, y, 1);
					img2.setPixel(x+1, y, 1);
					img2.setPixel(x-1, y+1, 1);
					img2.setPixel(x, y+1, 1);
					img2.setPixel(x+1, y+1, 1);
				}
			}
		}
		img2.setColor(1, QColor(Qt::black).rgba());
		img2.setColor(0, QColor(Qt::white).rgba());
		bmp2.convertFromImage(img2);
	} else
#endif
	{
		bmp2 = QBitmap::fromData(QSize(m_curEntry.width, m_curEntry.height), &ML[0], QImage::Format_Mono);
	}

	return QCursor(bmp1, bmp2, m_curEntry.xHotSpot, m_curEntry.yHotSpot);
}


bool KCurFileAnalyzer::_LoadFileHeader(QDataStream &stream)
{
	char* pTemp = (char*)&m_curMark;
	stream.readRawData(pTemp, sizeof(m_curMark));
	if ((quint16)TYPE != m_curMark.type || (quint16)RESERVED != m_curMark.reserved)
		return false;

	pTemp = (char*)&m_curEntry;
	stream.readRawData(pTemp, sizeof(m_curEntry));

	pTemp = (char*)&m_curInfoHeader;
	stream.readRawData(pTemp, sizeof(m_curInfoHeader));
	if ((quint16)SIZE_OF_INFOHEADER != m_curInfoHeader.size)
		return false;

	return true;
}

QVector<uchar> KCurFileAnalyzer::_ReadBitmap(int width, int height, QDataStream &dataStream) const
{
	if (width % 8 != 0)
	{
		width = (width / 8 + 1) * 8;
	}
	int lsz = width / 8;
	int sz = height * lsz;
	QVector<uchar> buf;
	buf.resize(sz);
	for (int i = height - 1; i >= 0; --i)
	{
		// 从下往上。
		dataStream.readRawData((char*)&buf[i * lsz], lsz);
	}
	return buf;
}
