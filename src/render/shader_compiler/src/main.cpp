namespace sge {

class EditorApp : public NativeUIApp
{
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc_) override;
private:
};

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

}

}

int main(int argc, char* argv)
{
	using namespace sge;

	sge::EditorApp app;
	sge::EditorApp::CreateDesc desc;
	return app.run(desc);
}