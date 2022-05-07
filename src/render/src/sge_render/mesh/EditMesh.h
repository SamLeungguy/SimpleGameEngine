#pragma once

#include "../vertex/Vertex.h"

namespace sge {

class EditMesh : public NonCopyable {
public:
	static const u8 s_uvCountMax = 4;

	RenderPrimitiveType	primitive = RenderPrimitiveType::Triangles;
	Vector<u32> indices;

	Vector<Tuple3f>	positions;
	Vector<Tuple2f>	uvs[s_uvCountMax];
	Vector<Color4b> colors;

	Vector<Tuple3f>	normals;
	Vector<Tuple3f>	tangents;
	Vector<Tuple3f>	binormals;

	void clear();
};

}