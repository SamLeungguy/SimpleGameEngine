#include "Lexer.h"
#include "HLSLCompiler.h"

#include <nlohmann/json.hpp>

namespace sge {

class EditorApp : public NativeUIApp
{
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc_) override;
private:
};

//template <typename T>
//struct adl_serializer {
//	static void to_json(json& j, const T& value) {
//		// calls the "to_json" method in T's namespace
//	}
//
//	static void from_json(const json& j, T& value) {
//		// same thing, but with the "from_json" method
//	}
//};

#if 1	// EditorApp_Impl
void EditorApp::onCreate(CreateDesc& desc_)
{
	{
		String file = getExecutableFilename();
		String path = FilePath::getDir(file);
		path.append("/../../../../../../examples/Test101");

		setCurrentDir(path);

		auto dir = getCurrentDir();
		SGE_LOG("dir = {}", dir);
	}

	Base::onCreate(desc_);
	auto dir = getCurrentDir();
	SGE_DUMP_VAR(dir);

	{
		//StrView filename = "Assets/Shaders/test.shader";
		StrView filename = "test.shader";
		//Lexer::parse(filename);
		HLSLCompiler compiler;
		compiler.compile(filename);
	}

#if 1
	{
		using json = nlohmann::ordered_json;

		MemMapFile mm;
		bool ret = FileStream::s_createDirectory(L"LocalTemp");
		ret = FileStream::s_createDirectory(L"LocalTemp/Imported");
		ret = FileStream::s_createDirectory(L"LocalTemp/Imported/test.shader");

		if (!ret)
		{
#if 0
			using json = nlohmann::ordered_json;

			json j;

			{
				// create an empty structure (null)

				// add a number that is stored as double (note the implicit conversion of j to an object)
				j["profile"] = "vs_5_0";
				j["csWorkgroupSize"] = { 1, 1, 1 };
	
				j["inputs"] = 
					{ 
							 {{"name", "a_pos"}
							 ,{"attrd", /*enum to string*/ "POSITITON0"}
							 ,{"dataType",	/*enum to string*/ "Float32x4"} }

							,{{"name", "a_uv"}
							 ,{"attrd", /*enum to string*/ "TEXCOORD0"}
							 ,{"dataType",	/*enum to string*/ "Float32x2"} }

							,{{"name", "a_normal"}
							 ,{"attrd", /*enum to string*/ "NORMAL0"}
							 ,{"dataType",	/*enum to string*/ "Float32x3"} }
					};

				j["params"] = {};

				{
					j["uniformBuffers"] = {
						{"name", "$Globals"}
						, {"bindPoint", 0}
						, {"bindCount", 1}
						, {"dataSize", 176}
						, {"variables", {}}
					};

					String name[3] = {"col", "mvp", "vp"};
					SGE_DUMP_VAR(j["uniformBuffers"]["variables"].size());
					for (int i = 0; i < 3; ++i)
					{
						auto& e = j["uniformBuffers"]["variables"];
						e.push_back( {
							  {"name", name[i].c_str()}
							, {"offset", i}
							, {"dataType", /*enum to string*/ "Float32_4x4"}
						});
					}
				}

				{
					j["textures"] = {
						{"name", ""}
						, {"bindPoint", 0}
						, {"bindCount", 1}
						, {"dataSize", 176}
					};
					/*for (size_t i = 0; i < 3; i++)
					{
						j["textures"]["variables"] = {
						{"name", ""}
						, {"bindPoint", 0}
						, {"bindCount", 1}
						};
					}*/
				}

				{
					j["samplers"] = {
						{"name", ""}
						, {"bindPoint", 0}
						, {"bindCount", 1}
						, {"dataSize", 176}
					};
					/*for (size_t i = 0; i < 3; i++)
					{
						j["samplers"]["variables"] = {
						{"name", ""}
						, {"bindPoint", 0}
						, {"bindCount", 1}
						};
					}*/
				}

				{
					j["storageBuffers"] = {
					{"name", "$Globals"}
					, {"bindPoint", 0}
					, {"bindCount", 1}
					, {"dataSize", 176}
					};
					//for (size_t i = 0; i < 3; i++)
					//{
					//	j["storageBuffers"]["variables"] = {
					//	{"name", "matMvp"}
					//	, {"offset", 0}
					//	, {"dataType", /*enum to string*/ "Float32_4x4"}
					//	};
					//}
				}

			}
			mm.openWrite("LocalTemp/Imported/test.shader/hello.json", false);

			auto str = j.dump(4);
			StrView a = { str.c_str(), str.size() };

			Span<const u8> data(reinterpret_cast<const u8*>(a.data()), a.size());

			mm.writeBytes(data);
#else
			
			/*mm.openRead("LocalTemp/Imported/test.shader/hello.json");

			json jf = json::parse(mm.data());

			json& js = jf.at("profile");
			auto& a = js.get<std::string>();

			js = jf.at("csWorkgroupSize");
			auto b = js.get<std::vector<int>>();

			SGE_DUMP_VAR(a.c_str());
			SGE_DUMP_VAR(b.size());
			SGE_DUMP_VAR(b[0], b[1], b[2]);*/

#endif // 1
		}
		else
		{
			mm.openAppend("LocalTemp/Imported/test.shader/hello.txt");

			StrView a = "asdsadsd";
			Span<const u8> data(reinterpret_cast<const u8*>(a.data()), a.size());

			mm.writeBytes(data);
		}
	}
#endif // 0

}
#endif // 0
}


int main(int argc, char* argv)
{
	using namespace sge;

	// for convenience


	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;
	return app.run(desc);
}