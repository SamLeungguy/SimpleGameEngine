#pragma once

#include "Render_Common.h"

namespace sge {

struct VertexLayout
{
	struct Element
	{
		VertexSematic sematic;
		RenderDataType type;

		// temp
		String name;		
		u32 count	= 0;
		u32 size	= 0;
		u32 offset	= 0;
	};

	VertexLayout() = default;
	
	void push_back(Element& element_)
	{
		u32 size = Utils::getRenderDataTypeSize(element_.type);
		element_.offset = totalBytes;
		element_.size = size;
		element_.name = Utils::getVertexSematicName(element_.sematic);
		totalBytes += size;

		elements.push_back(element_);
	}

	void reserve(int cap_) { elements.reserve(cap_); }

	inline u64 getCount() const					{ return elements.size(); }
	inline u64 getTotalBytes() const			{ return totalBytes; }

	Vector<Element>::iterator begin()				{ return elements.begin(); }
	Vector<Element>::iterator end()					{ return elements.end(); }

	Vector<Element>::const_iterator begin() const	{ return elements.cbegin(); }
	Vector<Element>::const_iterator end() const		{ return elements.cend(); }

	Vector<Element> elements;
	u64 totalBytes = 0;

private:
	void _processTotalElementBytes()
	{
		u32 offset = 0;
		for (auto& element : elements)
		{
			u32 size = Utils::getRenderDataTypeSize(element.type);

			element.offset = offset;
			element.size = size;
			element.name = Utils::getVertexSematicName(element.sematic);

			offset += size;
			totalBytes += size;
		}
	}
};

struct Vertex_PosColor
{
	Tuple3f position;
	Color4b color;

	~Vertex_PosColor()
	{
		delete s_pVertexLayout;
		s_pVertexLayout = nullptr;
	}

	static void init()
	{
		SGE_ASSERT(!s_pVertexLayout, "");
		if (s_pVertexLayout)
		{
			s_pVertexLayout = new VertexLayout;
			s_pVertexLayout->reserve(2);
			VertexLayout::Element element;
			element.sematic = VertexSematic::Pos;
			element.type = RenderDataType::Float32;
			s_pVertexLayout->push_back(element);

			element.sematic = VertexSematic::Color;
			element.type = RenderDataType::Float32x4;
			s_pVertexLayout->push_back(element);
		}
	}

	static VertexLayout* s_pVertexLayout;
};

}