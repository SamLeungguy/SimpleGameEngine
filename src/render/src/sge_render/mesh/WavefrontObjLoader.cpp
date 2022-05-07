#include "WavefrontObjLoader.h"
#include <sge_core/file/MemMapFile.h>

namespace sge {

void WavefrontObjLoader::loadFile(EditMesh& mesh_, StrView filename_)
{
	MemMapFile mm;
	mm.open(filename_);
	loadMem(mesh_, mm);
}

void WavefrontObjLoader::loadMem(EditMesh& mesh_, Span<const u8> src_)
{
	WavefrontObjLoader loader;
	loader._loadMem(mesh_, src_);
}

void WavefrontObjLoader::_loadMem(EditMesh& mesh_, Span<const u8> src_)
{
	mesh_.clear();
	_pOutMesh = &mesh_;
	_pOutMesh->primitive = RenderPrimitiveType::Triangles;

	_source = StrView(reinterpret_cast<const char*>(src_.data()), src_.size() / sizeof(char));
	_souceRemain = _source;
	_lineNumber = 0;

	while (_souceRemain.size() > 0)
	{
		_nextLine();
		if (_currentLine.size() > 0)
		{
			_lineRemain = _currentLine;
			_nextToken();
			_parseLine();
		}
	}
}

void WavefrontObjLoader::_nextLine()
{
	auto pair = StringUtil::splitByChar(_souceRemain, "\n");
	_currentLine = pair.first;
	_souceRemain = pair.second;
	_lineNumber++;
}

void WavefrontObjLoader::_nextToken()
{
	auto pair	= StringUtil::splitByChar(_lineRemain, " \t\r");
	_token		= StringUtil::trimChar(pair.first, " \t\r");
	_lineRemain = StringUtil::trimChar(pair.second, " \t\r");
}

void WavefrontObjLoader::_parseLine()
{
	if (_token.size() <= 0)
		return;

	if (_token[0] == '#')
		return;

	if (_token == "v")	return _parseLine_v();
	if (_token == "vt") return _parseLine_vt();
	if (_token == "vn") return _parseLine_vn();
	if (_token == "f")	return _parseLine_f();
}

void WavefrontObjLoader::_parseLine_v()
{
	Tuple4f value(0.0f, 0.0f, 0.0f, 1.0f);
	for (size_t i = 0; i < 4; i++)
	{
		_nextToken();
		if (_token.size() <= 0)
			break;

		if (!StringUtil::tryParse(_token, value.data[i]))
		{
			_error("error read v");
		}
	}

	Tuple3f pos(value.x / value.w
		, value.y / value.w
		, value.z / value.w);
	_tmpPositions.emplace_back(pos);
}

void WavefrontObjLoader::_parseLine_vt()
{
	Tuple2f value(0.0f, 0.0f);
	for (size_t i = 0; i < 2; i++)
	{
		_nextToken();
		if (_token.size() <= 0)
			break;

		if (!StringUtil::tryParse(_token, value.data[i]))
		{
			_error("error read vt");
		}
	}
	_tmpUvs.emplace_back(value);
}

void WavefrontObjLoader::_parseLine_vn()
{
	Tuple3f value(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < 3; i++)
	{
		_nextToken();
		if (_token.size() <= 0)
			break;

		if (!StringUtil::tryParse(_token, value.data[i]))
		{
			_error("error read vn");
		}
	}
	_tmpNormals.emplace_back(value);
}

void WavefrontObjLoader::_parseLine_f()
{
	Vector_<int, 64> face_vi;
	Vector_<int, 64> face_vt;
	Vector_<int, 64> face_vn;

	while (_lineRemain.size() > 0)
	{
		_nextToken();
		if (_token.size() <= 0)
			break;

		int vi = 0, vt = 0, vn = 0;

		auto pair = StringUtil::splitByChar(_token, "/");
		if (!StringUtil::tryParse(pair.first, vi))
		{
			_error("error read f vi");
		}

		//vt
		pair = StringUtil::splitByChar(pair.second, '/');
		if (pair.first.size()) {
			if (!StringUtil::tryParse(pair.first, vt)) {
				_error("error read f vt");
			}
		}
		//vn
		pair = StringUtil::splitByChar(pair.second, '/');
		if (pair.first.size()) {
			if (!StringUtil::tryParse(pair.first, vn)) {
				_error("error read f vn");
			}
		}

		// could save the same pattern of v/vt/vn with index, then could use the same vertex

		// index start from 1 and Negative values indicate relative vertex numbers
		if (vi != 0) {
			if (vi > 0) { vi--; }
			else { vi = static_cast<int>(_tmpPositions.size()) + vi; }
			face_vi.emplace_back(vi);
		}

		if (vt != 0) {
			if (vt > 0) { vt--; }
			else { vt = static_cast<int>(_tmpUvs.size()) + vt; }
			face_vt.emplace_back(vt);
		}

		if (vn != 0) {
			if (vn > 0) { vn--; }
			else { vn = static_cast<int>(_tmpNormals.size()) + vn; }
			face_vn.emplace_back(vn);
		}
	}

	if (face_vi.size() < 3) {
		_error("face size < 3");
	}

	u32 indicesStart = static_cast<u32>(_pOutMesh->positions.size());

#if 0 // flip face front/back
	eastl::reverse(face_vi.begin(), face_vi.end());
	eastl::reverse(face_vt.begin(), face_vt.end());
	eastl::reverse(face_vn.begin(), face_vn.end());
#endif

	for (int i = 0; i < face_vi.size(); i++) {
		auto vi = face_vi[i];

		if (vi >= _tmpPositions.size()) _error("face vi out of range");
		_pOutMesh->positions.emplace_back(_tmpPositions[vi]);

		if (face_vt.size() >= face_vi.size()) {
			auto vt = face_vt[i];
			if (vt >= _tmpUvs.size()) _error("face vt out of range");
			_pOutMesh->uvs[0].emplace_back(_tmpUvs[vt]);
		}

		if (face_vt.size() >= face_vi.size()) {
			auto vn = face_vn[i];
			if (vn >= _tmpNormals.size()) _error("face vn out of range");
			_pOutMesh->normals.emplace_back(_tmpNormals[vn]);
		}
	}

	for (size_t i = 0; i < face_vi.size(); i++) {
		if (i >= 3) {
			_pOutMesh->indices.emplace_back(static_cast<u32>(indicesStart + 0));
			_pOutMesh->indices.emplace_back(static_cast<u32>(indicesStart + i - 1));
		}

		u32 vi = static_cast<u32>(indicesStart + i);
		if (vi >= _pOutMesh->positions.size()) {
			throw SGE_ERROR("vi out of range");
		}
		_pOutMesh->indices.emplace_back(vi);
	}
}

#if 0
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

	_tmpPositions.clear();
	_tmpNormals.clear();
	_tmpUvs.clear();

	String fileData;
	readFile(fileData, filepath_);

	String meshName;

	EditMesh& outMesh = mesh_;
	_pOutMesh = &mesh_;

	u64 currentLine = 0;
	u64 cursor = 0;

	u64 vertexCount = 0;
	u64 faceCount = 0;

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
		int a = 0;
		a++;
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

				//SGE_DUMP_VAR_3(_tmpPositions[i].x, _tmpPositions[i].y, _tmpPositions[i].z);

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
			bool hasNextUv = true;

			for (size_t i = 0; hasNextUv; i++)
			{
				s = StringUtil::splitByChar(curLineView, ' ');
				hasNextUv = s.first.size() > 0 && s.first.find(token_uv) != StrView::npos;
				if (!hasNextUv)
					break;

				s = StringUtil::splitByChar(s.second, ' ');
				isParseSuccess = StringUtil::tryParse(s.first, uv[0]);
				SGE_ASSERT(isParseSuccess);

				isParseSuccess = StringUtil::tryParse(s.second, uv[1]);
				SGE_ASSERT(isParseSuccess);

				_tmpUvs.emplace_back(uv[0], uv[1]);

				//SGE_DUMP_VAR(_tmpUvs[i].x, _tmpUvs[i].y);

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

				//SGE_DUMP_VAR_3(_tmpNormals[i].x, _tmpNormals[i].y, _tmpNormals[i].z);

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

		while (tempCurLineView.size() > 0 && tempCurLineView[0] == token_face[0])
		{
			s = StringUtil::splitByChar(tempNextLineView, token_newLine);
			tempCurLineView = s.first;
			tempNextLineView = s.second;
			faceCount++;
		}

		if (faceCount <= 0)
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

		constexpr int max_support_face_size = 6;

		outMesh.indices.reserve(faceCount * max_support_face_size);

		i64 v = 0;
		i64 vt = 0;
		i64 vn = 0;

		for (size_t i = 0; i < faceCount; i++)
		{
#if 0
			u8 faceSize = 0;

			// get face size
			if (curLineView.size() > 0 && curLineView[0] == token_face[0])
			{
				auto tmp_split = StringUtil::splitByChar(curLineView, ' ');

				while (tmp_split.second.size() > 0)
				{
					tmp_split = StringUtil::splitByChar(tmp_split.second, ' ');
					faceSize++;
				}
			}
			else
			{
				throw SGE_ERROR("no face in this line {}", currentLine);
				continue;
			}

			SGE_ASSERT(faceSize <= max_support_face_size && faceSize >= 3);
#endif // 0

			s = StringUtil::splitByChar(curLineView, ' ');
			s = StringUtil::splitByChar(s.second, ' ');

			for (EditMesh::IndexType f = 0; f < max_support_face_size; f++)
			{
				auto tempSplit = StringUtil::splitByChar(s.first, '/');

				if (tempSplit.second.size() == 0)	// break if no more v/vt/vn
				{
					break;
				}

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
					//SGE_DUMP_VAR(v, _tmpPositions[v].x, _tmpPositions[v].y, _tmpPositions[v].z);
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

				if (f < 3)
				{
					outMesh.indices.emplace_back(static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1));
				}
				else if (f < 4)
				{
					auto f3 = static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1);
					outMesh.indices.emplace_back(f3 - 1);
					outMesh.indices.emplace_back(f3);
					outMesh.indices.emplace_back(f3 - f);
				}
				else if (f < 5)
				{
					auto f4 = static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1);
					outMesh.indices.emplace_back(f4 - 1);
					outMesh.indices.emplace_back(f4);
					outMesh.indices.emplace_back(f4 - f);
				}
				else if (f < 6)
				{
					auto f5 = static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1);
					outMesh.indices.emplace_back(f5 - 1);
					outMesh.indices.emplace_back(f5);
					outMesh.indices.emplace_back(f5 - f);
				}
				else
				{
					throw SGE_ERROR("not support face size {}", f);
				}

#if 0
				if (faceSize == 3)
				{
					outMesh.indices.emplace_back(static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1));
				}
				else if (faceSize == 4)
				{
					if (f < 3)
					{
						outMesh.indices.emplace_back(static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1));
					}
					else
					{
						auto f3 = static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1);
						outMesh.indices.emplace_back(f3 - 1);
						outMesh.indices.emplace_back(f3);
						outMesh.indices.emplace_back(f3 - 3);
					}
				}
				else if (faceSize == 5)
				{
					if (f < 3)
					{
						outMesh.indices.emplace_back(static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1));
					}
					else if (f < 4)
					{
						auto f3 = static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1);
						outMesh.indices.emplace_back(f3 - 1);
						outMesh.indices.emplace_back(f3);
						outMesh.indices.emplace_back(f3 - 3);
					}
					else
					{
						auto f4 = static_cast<EditMesh::IndexType>(outMesh.positions.size() - 1);
						outMesh.indices.emplace_back(f4 - 1);
						outMesh.indices.emplace_back(f4);
						outMesh.indices.emplace_back(f4 - 4);
					}
				}
#endif // 0

				SGE_ASSERT(tempSplit.second.size() == 0);

				s = StringUtil::splitByChar(s.second, ' ');
			}

			s_moveFileCursor(currentLine, cursor, spilt, token_newLine);
			curLineView = spilt.first;
			nextLineView = spilt.second;
		}
	}
}
#endif // 0

}