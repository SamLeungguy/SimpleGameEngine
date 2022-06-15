#pragma once

#include "Vertex.h"

namespace sge {

class VertexLayoutManager : public NonCopyable
{
public:
	VertexLayoutManager();
	~VertexLayoutManager();

	static VertexLayoutManager* instance();

	const VertexLayout* getLayout(VertexType type_);
	VertexLayout* _createLayout(VertexType type_);

	template<class Vertex>
	void registerLayout()
	{
		VertexLayout* pVertexLayout = _createLayout(Vertex::kType);
		pVertexLayout->stride = sizeof(Vertex);
		pVertexLayout->type = Vertex::kType;
		Vertex::onRegister(pVertexLayout);
	}

private:
	static VertexLayoutManager* s_pInstance;
	Map<VertexType, VertexLayout> _table;
};

}