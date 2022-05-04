#include "EditMesh.h"

namespace sge {

void EditMesh::clear()
{
	indices.clear();

	positions.clear();
	for (auto& t : uvs) {
		t.clear();
	}
	colors.clear();
	normals.clear();
	tangents.clear();
	binormals.clear();
}

}