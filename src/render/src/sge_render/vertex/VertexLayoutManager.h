#pragma once

#include "Vertex.h"

namespace sge {

class VertexLayoutManager : public NonCopyable
{
public:
	VertexLayoutManager();
	~VertexLayoutManager();

	static VertexLayoutManager* current();

	const VertexLayout* getLayout(VertexType type_);
	VertexLayout* _createLayout(VertexType type_);

	template<class Vertex>
	void registerLayout()
	{
		VertexLayout* pVertexLayout = _createLayout(Vertex::s_type);
		pVertexLayout->stride = sizeof(Vertex);
		pVertexLayout->type = Vertex::s_type;
		Vertex::onRegister(pVertexLayout);
	}

private:
	static VertexLayoutManager* s_pCurrent;
	Map<VertexType, VertexLayout> _table;
};

}