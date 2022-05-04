#include "WavefrontObjLoader.h"

#include <fstream>

namespace sge {

static void readFile(String& outStr_, const StrView& filepath_)
{
	outStr_.clear();

	TempString filename(filepath_);

	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		file.seekg(0, std::ios::end);
		size_t size = file.tellg();
		if (size != -1)
		{
			outStr_.resize(size);
			file.seekg(0, std::ios::beg);
			file.read(&outStr_[0], size);
		}
		file.close();
	}
	else
	{
		throw SGE_ERROR("Cannot open file");
	}
}

static bool s_moveFileCursor(u64& currentLine_, u64& cursor_, std::pair<StrView, StrView>& spilt_, StrView token_)
{
	currentLine_++;
	cursor_ += spilt_.first.size() + token_.size();		// count the spilt token(\n)
	spilt_ = StringUtil::splitByChar(spilt_.second, token_);

	return true;
}

void WavefrontObjLoader::load(EditMesh& mesh_, StrView filepath_)
{
	mesh_.clear();

	String fileData;
	readFile(fileData, filepath_);

	String meshName;

	EditMesh& outMesh = mesh_;
	_pOutMesh = &mesh_;

	u64 currentLine = 0;
	u64 cursor = 0;

	u64 vertexCount = 0;
	u64 faceCount = 0;
	u8 faceSize = 0;

	constexpr StrView token_newLine = "\n";
	constexpr char token_name = 'o';
	constexpr char token_pos = 'v';
	constexpr char* token_uv = "vt";
	constexpr char* token_normal = "vn";
	constexpr char* token_usemtl = "usemtl";
	constexpr char* token_s = "s";
	constexpr char* token_face = "f";

	auto spilt = StringUtil::splitByChar(fileData, token_newLine);
	StrView curLineView = spilt.first;
	StrView nextLineView = spilt.second;

	while (nextLineView.size() > 0 && nextLineView[0] != token_pos)
	{
		//currentLine++;
		//cursor += spilt.first.size() + token_newLine.size();		// count the spilt token(\n)
		//spilt = StringUtil::splitByChar(spilt.second, token_newLine);
		s_moveFileCursor(currentLine, cursor, spilt, token_newLine);

		curLineView = spilt.first;
		nextLineView = spilt.second;
	}

	// token 'o' get the name;
	{
		if (curLineView[0] == token_name)
		{
			auto s = StringUtil::splitByChar(curLineView, ' ');
			s = StringUtil::splitByChar(s.second, token_newLine);
			meshName = s.first;
		}
	}

	// get vertex count
	{
		if (nextLineView.find("v") == StrView::npos)
		{
			SGE_ASSERT("cannot find vertex position!");
		}

		auto s = StringUtil::splitByChar(nextLineView, token_newLine);
		StrView tempCurLineView = s.first;
		StrView tempNextLineView = s.second;

		while (tempCurLineView.size() > 0 && tempCurLineView.find('v') != StrView::npos && tempCurLineView[1] == ' ')
		{
			s = StringUtil::splitByChar(tempNextLineView, token_newLine);
			tempCurLineView = s.first;
			tempNextLineView = s.second;
			vertexCount++;
		}
	}

	// token 'v' get vertex position
	{
		s_moveFileCursor(currentLine, cursor, spilt, token_newLine);
		curLineView = spilt.first;
		nextLineView = spilt.second;
		// curLineView shd be first v

		auto s = StringUtil::splitByChar(curLineView, ' ');
		if (s.first.size() > 0 && s.first[0] == token_pos)
		{
			_tmpPositions.reserve(vertexCount);
			outMesh.positions.reserve(vertexCount);
			//constexpr size_t posArraySize = std::extent_v<get_VectorElementType<>>();
			constexpr size_t posArraySize = 3;
			float pos[posArraySize];

			bool isParseSuccess = true;

			for (size_t i = 0; i < vertexCount; i++)
			{
				s = StringUtil::splitByChar(curLineView, ' ');

				s = StringUtil::splitByChar(s.second, ' ');
				isParseSuccess = StringUtil::tryParse(s.first, pos[0]);
				SGE_ASSERT(isParseSuccess);

				s = StringUtil::splitByChar(s.second, ' ');
				isParseSuccess = StringUtil::tryParse(s.first, pos[1]);
				SGE_ASSERT(isParseSuccess);

				isParseSuccess = StringUtil::tryParse(s.second, pos[2]);
				SGE_ASSERT(isParseSuccess);

				_tmpPositions.emplace_back(pos[0], pos[1], pos[2]);

				SGE_DUMP_VAR_3(_tmpPositions[i].x, _tmpPositions[i].y, _tmpPositions[i].z);

				s_moveFileCursor(currentLine, cursor, spilt, token_newLine);
				curLineView = spilt.first;
				nextLineView = spilt.second;
			}
		}
		else
		{
			SGE_ASSERT(s.first.size() > 0 && s.first[0] == token_pos);
		}
	}

	// token 'vt' get vertex uv
	{
		// curLineView shd be first vt

		auto s = StringUtil::splitByChar(curLineView, ' ');
		if (s.first.size() > 0 && s.first.find(token_uv) != StrView::npos)
		{
			_tmpUvs.reserve(vertexCount);
			outMesh.uvs[0].reserve(vertexCount);

			constexpr size_t uvArraySize = 2;
			float uv[uvArraySize];

			bool isParseSuccess = true;

			for (size_t i = 0; i < vertexCount; i++)
			{
				s = StringUtil::splitByChar(curLineView, ' ');

				s = StringUtil::splitByChar(s.second, ' ');
				isParseSuccess = StringUtil::tryParse(s.first, uv[0]);
				SGE_ASSERT(isParseSuccess);

				isParseSuccess = StringUtil::tryParse(s.second, uv[1]);
				SGE_ASSERT(isParseSuccess);

				_tmpUvs.emplace_back(uv[0], uv[1]);

				SGE_DUMP_VAR(_tmpUvs[i].x, _tmpUvs[i].y);

				s_moveFileCursor(currentLine, cursor, spilt, token_newLine);
				curLineView = spilt.first;
				nextLineView = spilt.second;
			}
		}
	}

	// token 'vn' get vertex normal
	{
		// curLineView shd be first vn

		auto s = StringUtil::splitByChar(curLineView, ' ');
		if (s.first.size() > 0 && s.first.find(token_normal) != StrView::npos)
		{
			_tmpNormals.reserve(vertexCount);
			outMesh.normals.reserve(vertexCount);

			//constexpr size_t posArraySize = std::extent_v<get_VectorElementType<>>();
			constexpr size_t normalArraySize = 3;
			float normal[normalArraySize];

			bool isParseSuccess = true;
			bool hasNextNormal = true;

			for (size_t i = 0; hasNextNormal; i++)
			{
				s = StringUtil::splitByChar(curLineView, ' ');
				hasNextNormal = s.first.size() > 0 && s.first.find(token_normal) != StrView::npos;
				if (!hasNextNormal)
					break;

				s = StringUtil::splitByChar(s.second, ' ');
				isParseSuccess = StringUtil::tryParse(s.first, normal[0]);
				SGE_ASSERT(isParseSuccess);

				s = StringUtil::splitByChar(s.second, ' ');
				isParseSuccess = StringUtil::tryParse(s.first, normal[1]);
				SGE_ASSERT(isParseSuccess);

				isParseSuccess = StringUtil::tryParse(s.second, normal[2]);
				SGE_ASSERT(isParseSuccess);

				_tmpNormals.emplace_back(normal[0], normal[1], normal[2]);

				SGE_DUMP_VAR_3(_tmpNormals[i].x, _tmpNormals[i].y, _tmpNormals[i].z);

				s_moveFileCursor(currentLine, cursor, spilt, token_newLine);
				curLineView = spilt.first;
				nextLineView = spilt.second;
			}
		}
	}

	// token "usmtl"
	{
		auto s = StringUtil::splitByChar(curLineView, ' ');
		if (s.first.size() > 0 && s.first.find(token_usemtl) != StrView::npos)
		{
			s = StringUtil::splitByChar(curLineView, ' ');
		}
	}

	// token 's'
	{
		s_moveFileCursor(currentLine, cursor, spilt, token_newLine);
		curLineView = spilt.first;
		nextLineView = spilt.second;

		auto s = StringUtil::splitByChar(curLineView, ' ');
		if (s.first.size() > 0 && s.first.find(token_s) != StrView::npos)
		{
			s = StringUtil::splitByChar(curLineView, ' ');
		}
	}

	// get face count
	{
		auto s = StringUtil::splitByChar(nextLineView, token_newLine);
		StrView tempCurLineView = s.first;
		StrView tempNextLineView = s.second;

		// get face size
		if (tempCurLineView.size() > 0 && tempCurLineView[0] == token_face[0])
		{
			s = StringUtil::splitByChar(tempCurLineView, ' ');

			while (s.second.size() > 0)
			{
				s = StringUtil::splitByChar(s.second, ' ');
				faceSize++;
			}
		}

		while (tempCurLineView.size() > 0 && tempCurLineView[0] == token_face[0])
		{
			s = StringUtil::splitByChar(tempNextLineView, token_newLine);
			tempCurLineView = s.first;
			tempNextLineView = s.second;
			faceCount++;
		}

		if (faceCount <= 0 || faceSize <= 0)
		{
			SGE_ASSERT("invaild face count!");
		}
	}

	// token 'f' get faces (indices)
	{
		s_moveFileCursor(currentLine, cursor, spilt, token_newLine);
		curLineView = spilt.first;
		nextLineView = spilt.second;

		auto s = StringUtil::splitByChar(curLineView, ' ');

		outMesh.indices.reserve(faceCount);

		Vector_<u32, 16> face;
		face.resize(faceSize);

		i64 v = 0;
		i64 vt = 0;
		i64 vn = 0;

		for (size_t i = 0; i < faceCount; i++)
		{
			s = StringUtil::splitByChar(curLineView, ' ');
			s = StringUtil::splitByChar(s.second, ' ');

			for (size_t f = 0; f < faceSize; f++)
			{
				auto tempSplit = StringUtil::splitByChar(s.first, '/');

				if (StringUtil::tryParse(tempSplit.first, v))
				{
					SGE_ASSERT(static_cast<size_t>(v) <= _tmpPositions.size());

					if (v < 0)
					{
						SGE_ASSERT(static_cast<size_t>(-v) <= _tmpPositions.size());
						v += _tmpPositions.size();
					}
					else
						v -= 1;

					outMesh.positions.emplace_back(_tmpPositions[v]);
					face[f] = static_cast<u32>(v);
				}
				else
				{
					throw SGE_ERROR("invalid face vertex");
				}

				tempSplit = StringUtil::splitByChar(tempSplit.second, '/');
				if (StringUtil::tryParse(tempSplit.first, vt))
				{
					SGE_ASSERT(static_cast<size_t>(vt) <= _tmpUvs.size());

					if (vt < 0)
					{
						SGE_ASSERT(static_cast<size_t>(-vt) <= _tmpUvs.size());
						vt += _tmpUvs.size();
					}
					else
						vt -= 1;

					outMesh.uvs[0].emplace_back(_tmpUvs[vt]);
				}
				else
					outMesh.uvs[0].emplace_back(0.0f, 0.0f);

				tempSplit = StringUtil::splitByChar(tempSplit.second, '/');
				if (StringUtil::tryParse(tempSplit.first, vn))
				{
					SGE_ASSERT(static_cast<size_t>(vn) <= _tmpNormals.size());

					if (vn < 0)
					{
						SGE_ASSERT(static_cast<size_t>(-vn) <= _tmpNormals.size());
						vn += _tmpNormals.size();
					}
					else
						vn -= 1;

					outMesh.normals.emplace_back(_tmpNormals[vn]);
				}
				else
					outMesh.normals.emplace_back(0.0f, 0.0f, 0.0f);

				SGE_ASSERT(tempSplit.second.size() == 0);

				s = StringUtil::splitByChar(s.second, ' ');
			}

			if (faceSize == 3)
			{
				outMesh.indices.emplace_back(face[0]);
				outMesh.indices.emplace_back(face[1]);
				outMesh.indices.emplace_back(face[2]);
			}
			else if (faceSize == 4)
			{

			}
			else
			{
				SGE_ASSERT(0, "not suppported face size!");
			}

			s_moveFileCursor(currentLine, cursor, spilt, token_newLine);
			curLineView = spilt.first;
			nextLineView = spilt.second;
		}
	}
}

}