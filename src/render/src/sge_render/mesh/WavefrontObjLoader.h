#pragma once

#include "EditMesh.h"

namespace sge {

class WavefrontObjLoader : public NonCopyable
{
public:
	WavefrontObjLoader() = default;
	~WavefrontObjLoader() = default;

	static void loadFile(EditMesh& mesh_, StrView filename_);
	static void loadMem(EditMesh& mesh_, ByteSpan src_);

private:
	void _loadMem(EditMesh& mesh_, ByteSpan src_);

	void _nextLine();
	void _nextToken();
	void _parseLine();
	void _parseLine_v();
	void _parseLine_vt();
	void _parseLine_vn();
	void _parseLine_f();

	void _error(StrView msg_);

private:
	EditMesh* _pOutMesh = nullptr;

	StrView _source;
	StrView _token;
	StrView _currentLine;
	StrView _lineRemain;
	StrView _souceRemain;

	int _lineNumber = 0;

	Vector<Tuple3f> _tmpPositions;
	Vector<Tuple3f> _tmpNormals;
	Vector<Tuple2f> _tmpUvs;
};

inline void WavefrontObjLoader::_error(StrView msg_) { throw SGE_ERROR("{}: {}", _lineNumber, msg_); }
}