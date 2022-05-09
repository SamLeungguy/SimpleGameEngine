#include <sge_core/file/MemMapFile.h>

#include "Lexer.h"

namespace sge {

class EditorApp : public NativeUIApp
{
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc_) override;
private:
};

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
		StrView filename = "Assets/Shaders/test.shader";
		Lexer::parse(filename);
	}

	{
		
		MemMapFile mm;
		bool ret = FileStream::s_createDirectory(L"LocalTemp");
		ret = FileStream::s_createDirectory(L"LocalTemp/Imported");
		ret = FileStream::s_createDirectory(L"LocalTemp/Imported/test.shader");

		if (ret)
		{

			mm.openWrite("LocalTemp/Imported/test.shader/hello.txt", false);
			StrView a = "asdsadsd";
			Span<const u8> data(reinterpret_cast<const u8*>(a.data()), a.size());
			
			mm.writeBytes(data);
		}
		else
		{
			mm.openAppend("LocalTemp/Imported/test.shader/hello.txt");

			StrView a = "asdsadsd";
			Span<const u8> data(reinterpret_cast<const u8*>(a.data()), a.size());

			mm.writeBytes(data);
		}
	}
}
#endif // 0
}

int main(int argc, char* argv)
{
	using namespace sge;

	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;

	return app.run(desc);
}