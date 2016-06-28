// -----------------------------------------------------------------------
//
// �� �� �� ��kcurfileanalyzer.cpp
// �� �� �� ��v8
// ����ʱ�� ��2011-1
// �������� ��
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

	// ����û�����ļ���ʽ�жϣ�Ӧ�����ع��������

	QVector<uchar> XL = _ReadBitmap(m_curEntry.width, m_curEntry.height, stream);
	QVector<uchar> AL = _ReadBitmap(m_curEntry.width, m_curEntry.height, stream);
	if (stream.status() != QDataStream::Ok) 
		return QCursor();

	QVector<uchar> BL, ML;
	BL.reserve(XL.size());
	ML.reserve(XL.size());
	for (int i = 0; i < AL.size(); ++i)
	{
		// Qt ��Ϊ����ͼ�� B M
		// Cur��Ϊ����ͼ�� X A
		// Qt��MΪ0ʱ����ʾ���		Cur��AΪ1ʱ��ʾ��ϣ����Ҫ��תA��ΪM
		ML.push_back(~AL[i]);
		// Ϊ���ʱ��Qt��Cur�ļ���XBͼ�㶨����ͬ���������Ϊ�ǻ��ʱ��Qt B=1Ϊ��ɫ B=0Ϊ��ɫ��Cur X=0Ϊ��ɫ��1Ϊ��ɫ
		BL.push_back(XL[i] ^ ML[i]);
	}

	QBitmap bmp1 = QBitmap::fromData(QSize(m_curEntry.width, m_curEntry.height), &BL[0], QImage::Format_Mono);
	QBitmap bmp2;
#ifndef X_OS_WINDOWS
	// ��Է�Windowsƽ̨��֧��xorģʽ���Ĵ�������ת��Ϊ��ʹ��xor�ķ�ʽ
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
		// �������ϡ�
		dataStream.readRawData((char*)&buf[i * lsz], lsz);
	}
	return buf;
}
