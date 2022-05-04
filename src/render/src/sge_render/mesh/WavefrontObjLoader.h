#pragma once

#include "EditMesh.h"

namespace sge {

class WavefrontObjLoader : public NonCopyable
{
public:
	WavefrontObjLoader() = default;
	~WavefrontObjLoader() = default;

	void load(EditMesh& mesh_, StrView filepath_);


private:
	Vector<Tuple3f> _tmpPositions;
	Vector<Tuple3f> _tmpNormals;
	Vector<Tuple2f> _tmpUvs;

	EditMesh* _pOutMesh = nullptr;
};

}